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
} ruLogSink;

typedef struct {
    uint32_t logLevel;
    alloc_chars msg;
} logMsg;

static ruLogSink* ls_ = NULL;
#define MAX_LOG_LEN 2048

static ptr freeMsg(ptr p) {
    logMsg* m = (logMsg*)p;
    if (!m) return NULL;
    ruFree(m->msg);
    ruFree(m);
    return NULL;
}

static void asyncQ(perm_ptr userData, uint32_t logLevel, trans_chars msg) {
    ruLogSink* ls = (ruLogSink*)userData;
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
    ruLogSink* ls = (ruLogSink*)userData;
    perm_chars out = msg;
    if (ls->cleaner) {
        ruStringReset(ls->clnBuf);
        ruCleanToWriter(ls->cleaner, msg, 0,
                        &cb2Writer, ls->clnBuf);
        out = ruStringGetCString(ls->clnBuf);
    }
    ls->logger(ls->ctx, ls->level, out);
}

static void freeLogger(ruLogSink* ls) {
    if (!ls) return;
    ls->queuer = NULL;
    ls->level = RU_LOG_NONE;
    ls->logger = NULL;
    ls->cleaner = NULL;
    ls->ctx = NULL;
    if (ls->clnBuf) ls->clnBuf = ruBufferFree(ls->clnBuf, false);
    if (ls->queue) ls->queue = ruListFree(ls->queue);
    ruFree(ls);
    return;
}

static ptr logThread(ptr p) {
    ruLogSink* ls = (ruLogSink*)p;
    msec_t to = 10;
    int32_t ret = RUE_OK;
    while (true) {
        logMsg* m = ruListTryPop(ls->queue, to, &ret);
        if (m) {
            syncQ(ls, m->logLevel, m->msg);
            freeMsg(m);
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

static ruLogSink* newLogger(ruLogFunc logger, uint32_t logLevel, perm_ptr userData,
                            ruCleaner cleaner, bool threaded) {
    ruLogSink* ls = ruMalloc0(1, ruLogSink);
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

RUAPI void ruSetLogger(ruLogFunc logger, uint32_t logLevel, perm_ptr userData,
                       ruCleaner cleaner, bool threaded) {
    ruLogSink *ps = ls_;
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

RUAPI void ruStdErrorLogger(perm_ptr udata, uint32_t logLevel, trans_chars msg) {
    if (msg) fputs(msg, stderr);
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
