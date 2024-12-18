/*
 * Copyright regify
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "lib.h"

// <editor-fold desc="common internals">
#if defined(_WIN32)
static perm_chars bAppendMode = "ab";
// no inodes here
#elif defined(ITS_OSX) || defined(ITS_IOS)
typedef ino_t ru_inode;
#elif defined(__EMSCRIPTEN__)
typedef ino_t ru_inode;
#else
// linux android
#ifdef __x86_64__
typedef __ino64_t ru_inode;
#else
typedef __ino_t ru_inode;
#endif
#endif

#define RU_LOG_CLOSE RU_LOG_NONE
#define RU_LOG_FLUSH RU_LOG_CRIT

static void setFlushMark(bool callback);

typedef struct {
    uint32_t logLevel;
    alloc_chars msg;
} logMsg;

static ptr logMsgFree(ptr p) {
    logMsg* le = (logMsg*)p;
    if (!le) return NULL;
    ruFree(le->msg);
    ruFree(le);
    return NULL;
}

static logMsg* logMsgNew(uint32_t logLevel, trans_chars msg) {
    logMsg* le = ruMalloc0(1, logMsg);
    le->logLevel = logLevel;
    le->msg = ruStrDup(msg);
    return le;
}

// </editor-fold>

// <editor-fold desc="pre logger">
typedef struct {
    ru_int type;
    ruList logs;
} preCtx;

ruMakeTypeGetter(preCtx, MagicPreCtx)

RUAPI ruPreCtx ruPreCtxNew(void) {
    preCtx* pc = ruMalloc0(1, preCtx);
    pc->type = MagicPreCtx;
    pc->logs = ruListNew(ruTypePtr(logMsgFree));
    return pc;
}

RUAPI ruPreCtx ruPreCtxFree(ruPreCtx rpc, bool flush) {
    preCtx* pc = preCtxGet(rpc, NULL);
    if (!pc) return NULL;
    if (flush) {
        bool logged = false;
        ruIterator li = ruListIter(pc->logs);
        logMsg* lm;
        for(ruIterTo(li, lm); li; ruIterTo(li, lm)) {
            if (ruDoesLog(lm->logLevel)) {
                if (!logged) logged = true;
                ruRawLog(lm->logLevel, lm->msg);
            }
        }
        if (logged) {
            ruInfoLog("* * * End of pre log messages");
        }
    }
    pc->logs = ruListFree(pc->logs);
    ruFree(pc);
    return NULL;
}

RUAPI void ruPreLogSink(perm_ptr rpc, uint32_t logLevel, trans_chars msg) {
    if (!msg) return;
    preCtx* pc = preCtxGet((ptr)rpc, NULL);
    if (!pc) {
        fputs(msg, stderr);
        return;
    }
    logMsg* le = logMsgNew(logLevel, msg);
    ruListAppend(pc->logs, le);
}
// </editor-fold>

// <editor-fold desc="sink internals">
typedef struct {
    ru_int type;
    alloc_chars filePath;
    alloc_chars newFilePath;
    ruWriteFunc writeCb;
    ruCloseFunc closeCb;
    perm_ptr closeCtx;
    sec_t checkTime;
    ruMutex fmux;
    FILE* wh;
#if defined(_WIN32)
    alloc_chars curPath;
#else
    ru_inode curNode;
#endif
} sinkCtx;

ruMakeTypeGetter(sinkCtx, MagicSinkCtx)

static void setCheckTime(sinkCtx* sc) {
    sc->checkTime = ruTimeSec() + 3;
}

#ifndef WIN32
static perm_chars bAppendMode = "a";

static ru_inode getInode(sinkCtx* sc) {
    ruStat_t st;
    int32_t ret = ruStat(sc->filePath, &st);
    if (ret == RUE_OK) {
        return st.st_ino;
    }
    return 0;
}
#endif

static void fileOpen(sinkCtx* sc, trans_chars msg) {
    ruMutexLock(sc->fmux);
    while (!sc->wh && sc->filePath) {
        int32_t ret;
        sc->wh = ruFOpen(sc->filePath, bAppendMode, &ret);
        logDbg("0x%p opened: %s ret: %d for %s", sc, sc->filePath, ret, msg);
        if (ret != RUE_OK) break;
        setCheckTime(sc);
#ifdef _WIN32
        ruReplace(sc->curPath, ruStrDup(sc->filePath));
#else
        sc->curNode = getInode(sc);
#endif
        break;
    }
    ruMutexUnlock(sc->fmux);
}

static bool fileGone(sinkCtx* sc) {
    setCheckTime(sc);
#ifdef _WIN32
    return !ruStrEquals(sc->filePath, sc->curPath);
#else
    ru_inode curNode = getInode(sc);
    return curNode != sc->curNode;
#endif
}

static void checkCb(sinkCtx* sc) {
    if (sc->closeCb) {
        logDbg("0x%p calling sc->closeCb", sc);
        sc->closeCb(sc->closeCtx);
        logDbg("0x%p sc->closeCb done", sc);
    }
    if (sc->newFilePath) {
        // This is only for ruSinkCtxPath calls.
        // It is done here to better define the cut over moment.
        ruReplace(sc->filePath, sc->newFilePath);
        logDbg("0x%p new sc->filePath: %s", sc, sc->filePath);
        sc->newFilePath = NULL;
    }
}

static void fileClose(sinkCtx* sc) {
    if (sc->wh) {
        logDbg("0x%p closing: %s", sc, sc->filePath);
        fclose(sc->wh);
    }
    sc->wh = NULL;
    sc->checkTime = 0;
#ifdef _WIN32
    ruFree(sc->curPath);
#else
    sc->curNode = 0;
#endif
}
// </editor-fold>

// <editor-fold desc="sink public">
RUAPI ruSinkCtx ruSinkCtxNew(trans_chars filePath, ruCloseFunc closeCb,
                             perm_ptr closeCtx) {
    if (!filePath) return NULL;
    sinkCtx* sc = ruMalloc0(1, sinkCtx);
    sc->type = MagicSinkCtx;
    sc->filePath = ruStrDup(filePath);
    sc->fmux = ruMutexInit();
    sc->writeCb = &fputs;
    sc->closeCb = closeCb;
    sc->closeCtx = closeCtx;
    logDbg("0x%p created filePath: %s", sc, sc->filePath);
    return sc;
}

RUAPI int32_t ruSinkWriteCb(ruSinkCtx rsc, ruWriteFunc writeCb) {
    int32_t ret;
    sinkCtx* sc = sinkCtxGet(rsc, &ret);
    if (ret != RUE_OK) return ret;
    if (writeCb) {
        sc->writeCb = writeCb;
    } else {
        sc->writeCb = &fputs;
    }
    return RUE_OK;
}

RUAPI int32_t ruSinkCtxPath(ruSinkCtx rsc, trans_chars filePath) {
    if (ruStrEmpty(filePath)) return RUE_PARAMETER_NOT_SET;
    int32_t ret;
    sinkCtx* sc = sinkCtxGet(rsc, &ret);
    if (ret != RUE_OK) return ret;
    if (!ruStrEquals(sc->filePath, filePath)) {
        ruReplace(sc->newFilePath, ruStrDup(filePath));
        logDbg("0x%p pre ruLastLog '%s' -> '%s'",
               sc, sc->filePath, sc->newFilePath);
        ruLastLog();
        logDbg("0x%p post ruLastLog", sc);
    }
    return ret;
}

RUAPI ruSinkCtx ruSinkCtxFree(ruSinkCtx rsc) {
    sinkCtx* sc = sinkCtxGet(rsc, NULL);
    if (sc) {
        sc->type = 0;
        ruFree(sc->filePath);
        ruFree(sc->newFilePath);
#ifdef _WIN32
        ruFree(sc->curPath);
#endif
        sc->fmux = ruMutexFree(sc->fmux);
        memset(sc, 0, sizeof(sinkCtx));
        logDbg("freed 0x%p", sc);
        ruFree(sc);
    }
    return NULL;
}

RUAPI void ruFileLogSink(perm_ptr rsc, uint32_t logLevel, trans_chars msg) {
    sinkCtx* sc = sinkCtxGet((ptr)rsc, NULL);
    if (!sc) {
        if (msg) {
            fputs(msg, stderr);
        }
        return;
    }
    if (sc->wh) {
        if (!msg) {
            fileClose(sc);
        } else if (ruTimeEllapsed(sc->checkTime)) {
            if (fileGone(sc)) {
                fileClose(sc);
            } else {
                fflush(sc->wh);
            }
        }
    }
    if (!msg) {
        if (logLevel == RU_LOG_CLOSE) checkCb(sc);
        return;
    }
    if (!sc->wh) fileOpen(sc, msg);
    if (sc->wh) {
        sc->writeCb(msg, sc->wh);
    } else {
        sc->writeCb(msg, stderr);
    }
}

RUAPI void ruStdErrLogSink(perm_ptr udata, uint32_t logLevel, trans_chars msg) {
    if (msg) fputs(msg, stderr);
}

// </editor-fold>

// <editor-fold desc="log internals">
/* log context */
typedef struct {
    // submit function syncQ or asyncQ
    ruLogFunc queuer;
    // users log sink
    ruLogFunc logger;
    uint32_t level;
    // user context
    perm_ptr ctx;
    // potential cleaner instance
    ruCleaner cleaner;
    // buffer to use for potential cleaning
    ruString clnBuf;
    // asyncQ queue if logger is threaded
    ruList queue;
    // thread reference
    ruThread logThread;
    // flush flag
    volatile bool flushReq;
    // termination flag
    volatile bool quitting;
} loggerCtx;

static loggerCtx l1_, l2_;
static loggerCtx* lc_ = NULL;
static ruMutex lmux_ = NULL;
// public cleaner singleton to be used by logger
ruCleaner pwCleaner_ = NULL;

#define MAX_LOG_LEN 2048

static ruCleaner getCleaner(void) {
    if (!pwCleaner_) {
        pwCleaner_ = ruCleanNew(0);
        logDbg("new cleaner instance 0x%p created", pwCleaner_);
    }
    return pwCleaner_;
}

static rusize_s cb2Writer (perm_ptr ctx, trans_ptr buf, rusize len) {
    ruString io = (ruString)ctx;
    if (RUE_OK == ruBufferAppend(io, buf, len)) return (rusize_s)len;
    return -1;
}

static void noQ(perm_ptr userData, uint32_t logLevel, trans_chars msg) {
    logQDbg("logger: 0x%p level: %d msg: %s", userData, logLevel, msg);
}

static void asyncQ(perm_ptr userData, uint32_t logLevel, trans_chars msg) {
    logQDbg("logger: 0x%p level: %d msg: %s", userData, logLevel, msg);
    loggerCtx* ls = (loggerCtx*)userData;
    perm_chars out = msg;
    ruString clnBuf = NULL;
    if (1 && ls->cleaner && out) {
        clnBuf = ruBufferNew(MAX_LOG_LEN);
        ruCleanToWriter(ls->cleaner, msg, 0, &cb2Writer, clnBuf);
        out = ruStringGetCString(clnBuf);
    }
    logMsg* lm = logMsgNew(logLevel, out);
    if (clnBuf) ruBufferFree(clnBuf, false);
    int32_t ret = ruListPush(ls->queue, lm);
    if (ret != RUE_OK) logMsgFree(lm);
}

static void syncQ(perm_ptr userData, uint32_t logLevel, trans_chars msg) {
    logQDbg("logger: 0x%p level: %d msg: %s", userData, logLevel, msg);
    loggerCtx* ls = (loggerCtx*)userData;
    perm_chars out = msg;
    // when threaded, asyncQ does the cleaning else we do
    if (!ls->logThread && ls->cleaner && out) {
        ruStringReset(ls->clnBuf);
        ruCleanToWriter(ls->cleaner, msg, 0,
                        &cb2Writer, ls->clnBuf);
        out = ruStringGetCString(ls->clnBuf);
    }
    ls->logger(ls->ctx, logLevel, out);
}

static bool loggerValid(loggerCtx* lc) {
    return lc->logger != NULL;
}

static void initLogger(loggerCtx* lc) {
    memset(lc, 0, sizeof(loggerCtx));
    lc->queuer = noQ;
}

static void initLog(void) {
    if (lc_) return;
    lmux_ = ruMutexInit();
    initLogger(&l1_);
    initLogger(&l2_);
    lc_ = &l1_;
    logDbg("l1_: 0x%p l2_: 0x%p lc_: 0x%p", &l1_, &l2_, lc_);
}

static void freeLogger(loggerCtx* lc) {
    logDbg("freeing 0x%p", lc);
    if (lc->logThread) {
        ruThreadWait(lc->logThread, 1, NULL);
        lc->logThread = NULL;
    }
    if (lc->clnBuf) lc->clnBuf = ruBufferFree(lc->clnBuf, false);
    if (lc->queue) lc->queue = ruListFree(lc->queue);
    initLogger(lc);
    logDbg("0x%p freed", lc);
}

// compulsory file close interval for our windows users
#define FLUSH_INT 60

static ptr logThread(ptr p) {
#ifndef _WIN32
    // don't dabble with signals
    sigset_t set;
    sigfillset(&set);
    pthread_sigmask(SIG_SETMASK, &set, NULL);
#endif
    loggerCtx* ls = (loggerCtx*)p;
    logDbg("0x%p starting", ls);
    msec_t to = 250;
    bool flushed = false;
    // this is only needed on windows, but is fine on *nix
    sec_t flushTime = ruTimeSec() + FLUSH_INT;
    int32_t ret = RUE_OK;
    while (true) {
        logMsg* m = ruListTryPop(ls->queue, to, &ret);
        if (m) {
            syncQ(ls, m->logLevel, m->msg);
            if (!m->msg && ls->flushReq) {
                ls->flushReq = false;
                flushTime = ruTimeSec() + FLUSH_INT;
                flushed = true;
            } else {
                flushed = false;
            }
            logMsgFree(m);
        } else {
            if (!flushed) {
                flushTime = 0;
            }
        }
        if (ruTimeEllapsed(flushTime)) {
            logDbg("0x%p flushing log because flushTime is: %ld", ls, flushTime);
            // send flush signal to logger
            ls->logger(ls->ctx, RU_LOG_FLUSH, NULL);
            flushTime = ruTimeSec() + FLUSH_INT;
            flushed = true;
        }
        // when quitting continue while we pop messages to flush the queue
        if (ls->quitting) {
            uint32_t sz = ruListSize(ls->queue, &ret);
            if (!sz) {
                logDbg("0x%p breaking loop size: %u ret: %d", ls, sz, ret);
                break;
            }
        }
    }
    // send termination signal to log sink
    ls->logger(ls->ctx, RU_LOG_CLOSE, NULL);
    ls->quitting = false;
    ls->flushReq = false; // just in case
    logDbg("0x%p finished", ls);
    return NULL;
}

static void newLogger(loggerCtx* ls, ruLogFunc logger, uint32_t logLevel,
                      perm_ptr userData, bool cleaned, uint32_t bufLen) {
    logDbg("0x%p created ud: 0x%p threaded: %u", ls, userData, bufLen);
    ls->logger = logger;
    ls->level = logLevel;
    ls->ctx = userData;
    if (cleaned) {
        ls->clnBuf = ruBufferNew(MAX_LOG_LEN);
        ls->cleaner = ruGetCleaner();
    }
    if (bufLen) {
        ls->queue = ruListNewBound(ruTypePtr(logMsgFree), bufLen, true);
        ls->queuer = asyncQ;
        ls->logThread = ruThreadCreateBg(
                logThread, ruStrDup((ls == &l1_) ? "logger1" : "logger2"), ls);
    } else {
        ls->queuer = syncQ;
    }
}

static void setFlushMark(bool callback) {
    if (!lc_) initLog();
    loggerCtx* lc = lc_;
    if (lc->logThread) lc->flushReq = true;
    lc->queuer(lc, callback ? RU_LOG_CLOSE : RU_LOG_FLUSH, NULL);
    while (lc->flushReq) ruSleepMs(1);
}
// </editor-fold>

// <editor-fold desc="log public">
RUAPI ruCleaner ruGetCleaner(void) {
    return getCleaner();
}

RUAPI perm_ptr ruGetLogCtx(void) {
    return lc_->ctx;
}

RUAPI void ruSetLogger(ruLogFunc logger, uint32_t logLevel, perm_ptr userData,
                       bool cleaned, bool threaded) {
    if (!lc_) initLog();
    ruMutexLock(lmux_);
    loggerCtx* pc = lc_;
    loggerCtx* lc = (lc_ == &l1_) ? &l2_ : &l1_;
    initLogger(lc);
    if (logger) {
        newLogger(lc, logger, logLevel, userData, cleaned,
                  threaded? RU_LOG_BUFFER_LINE_COUNT : 0);
    }
    lc_ = lc;
    logDbg("now using logger 0x%p", lc_);
    if (loggerValid(pc)) {
        ruListBind(pc->queue, false);
        if (pc->logThread) {
            logDbg("quitting 0x%p", pc);
            pc->quitting = true;
            while (pc->quitting) ruSleepMs(1);
            logDbg("0x%p quit", pc);
        } else {
            // hopefully time for all logging to migrate
            logDbg("flushing 0x%p", pc);
            ruSleepMs(10);
            // closing call for the last logger
            pc->logger(pc->ctx, RU_LOG_CLOSE, NULL);
            logDbg("0x%p flushed", pc);
        }
    }
    freeLogger(pc);
    ruMutexUnlock(lmux_);
}

RUAPI void ruLoggerUnblock(void) {
    if (!lc_) return;
    loggerCtx* lc = lc_;
    int32_t ret = ruListBind(lc->queue, false);
    ruInfoLogf("Unblocked logger 0x%p ec: %d", lc, ret);
}

RUAPI void ruStopLogger(void) {
    ruSetLogger(NULL, RU_LOG_NONE, NULL, false, false);
}

RUAPI void ruSetLogLevel(uint32_t logLevel) {
    if (!lc_) initLog();
    lc_->level = logLevel;
}

RUAPI uint32_t ruGetLogLevel(void) {
    if (!lc_) initLog();
    return lc_->level;
}

RUAPI bool ruDoesLog(uint32_t log_level) {
    if (!log_level) return false;
    if (!lc_) initLog();
    return lc_->level >= log_level;
}

// tracks recursive ruMakeLogMsgV within a thread
RU_THREAD_LOCAL int lmCall = 0;

RUAPI alloc_chars ruMakeLogMsgV(uint32_t log_level, trans_chars filePath,
                                trans_chars func, int32_t line,
                                trans_chars format, va_list args) {
    if (lmCall) {
        ruAbortm("ruMakeLogMsgV is called recursively");
    }
    lmCall++;
    char *lv;
    if (log_level >= RU_LOG_DBUG)
        lv = "DBUG";
    else if (log_level >= RU_LOG_VERB)
        lv = "VERB";
    else if (log_level >= RU_LOG_INFO)
        lv = "INFO";
    else if (log_level >= RU_LOG_WARN)
        lv = "WARN";
    else if (log_level >= RU_LOG_CRIT)
        lv = "CRIT";
    else
        lv = "????";

    char *file = (char*)ruBaseName((char*)filePath);

    char timeStr[20]; /* yyyy/mm/dd HH:MM:SS */
    struct tm tm;
    ruTimeVal tv;
    // https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
    ruGetTimeVal(&tv);
    int micros = (int)tv.usec;

    alloc_chars ret = NULL;

#ifdef __EMSCRIPTEN__
    localtime_r(&tv.sec, &tm);
    #define prefix "%s.%06d %s: %s(%s:%d): "
    strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", &tm);
    // estimate
    int32_t prefixSize = snprintf(ret, 0, prefix,
                                  timeStr, micros, lv, func, file, line);
#else
#ifdef _WIN32
    _localtime32_s(&tm, &tv.sec);
#else
    localtime_r(&tv.sec, &tm);
#endif
    strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", &tm);
#ifdef RUMS
    DWORD pid = GetCurrentProcessId();
    #define prefix "%s.%06d [%ld%s %s: %s(%s:%d): "
#else
    pid_t pid = getpid();
    #ifdef _WIN32
        #ifdef _WIN64
            #define prefix "%s.%06d [%lld%s %s: %s(%s:%d): "
        #else
            #define prefix "%s.%06d [%d%s %s: %s(%s:%d): "
        #endif
    #else
        #define prefix "%s.%06d [%d%s %s: %s(%s:%d): "
    #endif
#endif

    perm_chars pidEnd = "]:";
    if (!logPidEnd) setPidEnd();
    if (logPidEnd) {
        pidEnd = logPidEnd;
    }

    // estimate
    int32_t prefixSize = snprintf(ret, 0, prefix,
                                  timeStr, micros, pid, pidEnd, lv, func, file, line);
#endif
    va_list args2;
    va_copy(args2, args);
    int32_t msgsize  = vsnprintf(ret, 0, format, args2);
    va_end (args2);

    // do it +2 = \n\0
    ret = ruMalloc0(prefixSize+msgsize+2, char);
    char *ptr = ret;
#ifdef __EMSCRIPTEN__
    snprintf(ret, prefixSize+1, prefix,
             timeStr, micros, lv, func, file, line);
#else
    snprintf(ret, prefixSize+1, prefix,
             timeStr, micros, pid, pidEnd, lv, func, file, line);
#endif
    ptr += prefixSize;
#undef prefix
    vsnprintf(ptr, msgsize+1, format, args);
    ptr += msgsize;
    // trim WS
    while (ptr > ret && (*(ptr-1) == '\n' || *(ptr-1) == '\r' || *(ptr-1) == '\t' || *(ptr-1) == ' ')) ptr--;
    *ptr++ = '\n';
    *ptr = '\0';
    lmCall--;
    return ret;
}

RUAPI alloc_chars ruMakeLogMsg(uint32_t log_level, trans_chars filePath, trans_chars func,
                   int32_t line, trans_chars format, ...) {
    va_list args;
    va_start(args, format);
    char *out = ruMakeLogMsgV(log_level, filePath, func, line, format, args);
    va_end(args);
    return out;
}

RUAPI void ruLogDbg(trans_chars filePath, trans_chars func, int32_t line, trans_chars format, ...) {
    va_list args;
    va_start(args, format);
    alloc_chars msg = ruMakeLogMsgV(RU_LOG_CRIT, filePath, func, line, format, args);
    va_end(args);
    printf("%s", msg);
    fflush(stdout);
    ruFree(msg);
}

RUAPI void ruDoLog(uint32_t log_level, trans_chars filePath, trans_chars func,
                   int32_t line, trans_chars format, ...) {
    if (!ruDoesLog(log_level)) return;
    va_list args;
    va_start(args, format);
    ruDoLogV(log_level, filePath, func, line, format, args);
    va_end(args);
}

RUAPI void ruDoLogV(uint32_t log_level, trans_chars filePath, trans_chars func,
             int32_t line, trans_chars format, va_list args) {
    if (!ruDoesLog(log_level)) return;
    loggerCtx* lc = lc_;
    alloc_chars _log_str_ = ruMakeLogMsgV(log_level, filePath, func, line, format,
                                     args);
    lc->queuer(lc, log_level, _log_str_);
    free(_log_str_);
}

RUAPI void ruRawLog(uint32_t log_level, trans_chars msg) {
    if (!ruDoesLog(log_level)) return;
    loggerCtx* lc = lc_;
    lc->queuer(lc, log_level, msg);
}

RUAPI void ruFlushLog(void) {
    setFlushMark(false);
}

RUAPI void ruLastLog(void) {
    setFlushMark(true);
}
// </editor-fold>
