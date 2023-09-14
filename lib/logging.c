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
static ruLogFunc logger_ = NULL;
static uint32_t logLevel_ = RU_LOG_NONE;
static perm_ptr userLogData_ = NULL;

RUAPI void ruSetLogger(ruLogFunc logger, uint32_t logLevel, perm_ptr userData) {
    logger_ = logger;
    logLevel_ = logLevel;
    userLogData_ = userData;
}

RUAPI void ruStdErrorLogger(perm_ptr udata, trans_chars msg) {
    if (msg) fputs(msg, stderr);
}

RUAPI void ruSetLogLevel(uint32_t logLevel) {
    logLevel_ = logLevel;
}

RUAPI uint32_t ruGetLogLevel(void) {
    return logLevel_;
}

RUAPI bool ruDoesLog(uint32_t log_level) {
    if (!logger_ || !logLevel_ || !log_level) return false;
    return logLevel_ >= log_level;
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
    if (!logPidEnd) ruThreadSetName(NULL);
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
    alloc_chars _log_str_ = ruMakeLogMsgV(log_level, filePath, func, line, format,
                                     args);
    va_end(args);
    logger_(userLogData_, _log_str_);
    free(_log_str_);
}

RUAPI void ruDoLogV(uint32_t log_level, trans_chars filePath, trans_chars func,
             int32_t line, trans_chars format, va_list args) {
    if (!ruDoesLog(log_level)) return;
    alloc_chars _log_str_ = ruMakeLogMsgV(log_level, filePath, func, line, format,
                                     args);
    logger_(userLogData_, _log_str_);
    free(_log_str_);
}

RUAPI void ruRawLog(uint32_t log_level, trans_chars msg) {
    if (!ruDoesLog(log_level)) return;
    logger_(userLogData_, msg);
}
