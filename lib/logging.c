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

// <editor-fold desc="sink internals">
typedef struct {
    ru_int type;
    alloc_chars filePath;
    ruCloseFunc closeCb;
    perm_ptr closeCtx;
    sec_t checkTime;
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

static void fileOpen(sinkCtx* sc) {
    int32_t ret;
    sc->wh = ruFOpen(sc->filePath, bAppendMode, &ret);
    if (ret != RUE_OK) return;
    setCheckTime(sc);
#ifdef _WIN32
    ruReplace(sc->curPath, ruStrDup(sc->filePath));
#else
    sc->curNode = getInode(sc);
#endif
}

static void fileClose(sinkCtx* sc) {
    if (sc->wh) fclose(sc->wh);
    sc->wh = NULL;
    sc->checkTime = 0;
#ifdef _WIN32
    ruFree(sc->curPath);
#else
    sc->curNode = 0;
#endif
    if (sc->closeCb) sc->closeCb(sc->closeCtx);
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
// </editor-fold>

// <editor-fold desc="sink public">
RUAPI ruSinkCtx ruSinkCtxNew(trans_chars filePath, ruCloseFunc closeCb,
                             perm_ptr closeCtx) {
    if (!filePath) return NULL;
    sinkCtx* sc = ruMalloc0(1, sinkCtx);
    sc->type = MagicSinkCtx;
    sc->filePath = ruStrDup(filePath);
    sc->closeCb = closeCb;
    sc->closeCtx = closeCtx;
    return sc;
}

RUAPI int32_t ruSinkCtxPath(ruSinkCtx rsc, trans_chars filePath) {
    if (!filePath) return RUE_PARAMETER_NOT_SET;
    int32_t ret;
    sinkCtx* sc = sinkCtxGet(rsc, &ret);
    if (ret != RUE_OK) return ret;
    ruReplace(sc->filePath, ruStrDup(filePath));
    return ret;
}

RUAPI ruSinkCtx ruSinkCtxFree(ruSinkCtx rsc) {
    sinkCtx* sc = sinkCtxGet(rsc, NULL);
    if (sc) {
        sc->type = 0;
        ruFree(sc->filePath);
#ifdef _WIN32
        ruFree(sc->curPath);
#endif
        memset(sc, 0, sizeof(sinkCtx));
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
    if (!msg) return;
    if (!sc->wh) fileOpen(sc);
    if (sc->wh) {
        fputs(msg, sc->wh);
    } else {
        fputs(msg, stderr);
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
    // sync flag
    volatile bool syncing;
} logSinkCtx;

typedef struct {
    uint32_t logLevel;
    alloc_chars msg;
} logMsg;

static logSinkCtx* ls_ = NULL;
#define MAX_LOG_LEN 2048

static ptr freeMsg(ptr p) {
    logMsg* m = (logMsg*)p;
    if (!m) return NULL;
    ruFree(m->msg);
    ruFree(m);
    return NULL;
}

static void asyncQ(perm_ptr userData, uint32_t logLevel, trans_chars msg) {
    logSinkCtx* ls = (logSinkCtx*)userData;
    if (!ls->queue) return;
    logMsg* m = ruMalloc0(1, logMsg);
    m->logLevel = logLevel;
    m->msg = ruStrDup(msg);
    int32_t ret = ruListPush(ls->queue, m);
    if (ret != RUE_OK) freeMsg(m);
}

static rusize_s cb2Writer (perm_ptr ctx, trans_ptr buf, rusize len) {
    ruString io = (ruString)ctx;
    if (RUE_OK == ruBufferAppend(io, buf, len)) return (rusize_s)len;
    return -1;
}

static void syncQ(perm_ptr userData, uint32_t logLevel, trans_chars msg) {
    logSinkCtx* ls = (logSinkCtx*)userData;
    perm_chars out = msg;
    if (ls->cleaner) {
        ruStringReset(ls->clnBuf);
        ruCleanToWriter(ls->cleaner, msg, 0,
                        &cb2Writer, ls->clnBuf);
        out = ruStringGetCString(ls->clnBuf);
    }
    ls->logger(ls->ctx, ls->level, out);
}

static void freeLogger(logSinkCtx* ls) {
    if (!ls) return;
    ls->queuer = NULL;
    ls->level = RU_LOG_NONE;
    ls->logger = NULL;
    ls->cleaner = NULL;
    ls->ctx = NULL;
    if (ls->clnBuf) ls->clnBuf = ruBufferFree(ls->clnBuf, false);
    if (ls->queue) ls->queue = ruListFree(ls->queue);
    ruFree(ls);
}

static ptr logThread(ptr p) {
    logSinkCtx* ls = (logSinkCtx*)p;
    msec_t to = 10;
    bool flushed = false;
    int32_t ret = RUE_OK;
    while (true) {
        logMsg* m = ruListTryPop(ls->queue, to, &ret);
        if (m) {
            syncQ(ls, m->logLevel, m->msg);
            freeMsg(m);
            flushed = false;
        } else if (!flushed) {
            // send flush signal to logger
            ls->logger(ls->ctx, ls->level, NULL);
            flushed = true;
        }
        // when quitting continue while we pop messages to flush the queue
        if (ls->syncing && ret != RUE_OK) break;
    }
    // send termination signal to log sink
    ls->logger(ls->ctx, ls->level, NULL);
    ls->logThread = NULL;
    ls->syncing = false;
    return NULL;
}

static logSinkCtx* newLogger(ruLogFunc logger, uint32_t logLevel, perm_ptr userData,
                             ruCleaner cleaner, bool threaded) {
    logSinkCtx* ls = ruMalloc0(1, logSinkCtx);
    ls->logger = logger;
    ls->level = logLevel;
    ls->ctx = userData;
    if (cleaner) {
        ls->clnBuf = ruBufferNew(MAX_LOG_LEN);
        ls->cleaner = cleaner;
    }
    if (threaded) {
        ls->queue = ruListNew(ruTypePtr(freeMsg));
        ls->queuer = asyncQ;
        ls->logThread = ruThreadCreateBg(
                logThread, ruStrDup("logger"), ls);
    } else {
        ls->queuer = syncQ;
    }
    return ls;
}
// </editor-fold>

// <editor-fold desc="log public">
RUAPI void ruSetLogger(ruLogFunc logger, uint32_t logLevel, perm_ptr userData,
                       ruCleaner cleaner, bool threaded) {
    logSinkCtx *ps = ls_;
    if (logger) {
        ls_ = newLogger(logger, logLevel, userData, cleaner, threaded);
    } else {
        ls_ = NULL;
    }
    if (!ps) return;
    if (ps->logThread) {
        ps->syncing = true;
        while (ps->syncing) ruSleepMs(1);
    } else {
        // hopefully time for all logging to migrate
        ruSleepMs(10);
        // closing call for the last logger
        ps->logger(ps->ctx, ps->level, NULL);
    }
    freeLogger(ps);
}

RUAPI void ruStopLogger(void) {
    ruSetLogger(NULL, RU_LOG_NONE, NULL,
                NULL, false);
}

RUAPI void ruSetLogLevel(uint32_t logLevel) {
    if (ls_) ls_->level = logLevel;
}

RUAPI uint32_t ruGetLogLevel(void) {
    if (ls_) return ls_->level;
    return RU_LOG_NONE;
}

RUAPI bool ruDoesLog(uint32_t log_level) {
    if (!ls_ || !ls_->level || !log_level) return false;
    return ls_->level >= log_level;
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

#ifdef __EMSCRIPTEN__
    #define prefix "%s: %s(%s:%d): "
#else
    char timeStr[20]; /* yyyy/mm/dd HH:MM:SS */
    struct tm tm;
    ruTimeVal tv;
    // https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
    ruGetTimeVal(&tv);
    int micros = (int)tv.usec;

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
#endif
    char *file = (char*)ruBaseName((char*)filePath);
#ifndef __EMSCRIPTEN__
    char* pidEnd = "]:";
    if (!logPidEnd) setPidEnd();
    if (logPidEnd) {
        pidEnd = logPidEnd;
    }
#endif

    // estimate
    char *ret = NULL;
#ifdef __EMSCRIPTEN__
    int32_t prefixSize = snprintf(ret, 0, prefix, lv, func, file, line);
#else
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
    snprintf(ret, prefixSize+1, prefix, lv, func, file, line);
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
    alloc_chars _log_str_ = ruMakeLogMsgV(log_level, filePath, func, line, format,
                                     args);
    ls_->queuer(ls_, log_level, _log_str_);
    free(_log_str_);
}

RUAPI void ruRawLog(uint32_t log_level, trans_chars msg) {
    if (!ruDoesLog(log_level)) return;
    ls_->queuer(ls_, log_level, msg);
}

RUAPI void ruFlushLog(void) {
    ls_->queuer(ls_, RU_LOG_NONE, NULL);
}
// </editor-fold>
