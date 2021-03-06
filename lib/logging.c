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
#include <time.h>
#include <math.h>
#include "lib.h"
#ifdef RUMS
#else
#include <unistd.h>
#endif

/* log context */
static ruLogFunc logger_ = NULL;
static u_int32_t logLevel_ = 0;
static void* userLogData_ = NULL;

void ruSetLogger(ruLogFunc logger, u_int32_t logLevel, void* userData) {
    logger_ = logger;
    logLevel_ = logLevel;
    userLogData_ = userData;
}

void ruStdErrorLogger(void* udata, const char *msg) {
    fputs(msg, stderr);
}

u_int32_t ruGetLogLevel() {
    if (!logger_) return RU_LOG_NONE;
    return logLevel_;
}

bool ruDoesLog(u_int32_t log_level) {
    if (!logger_ || !logLevel_ || !log_level) return false;
    return logLevel_ >= log_level;
}

static char* makeLogMsg(u_int32_t log_level, const char *filePath, const char *func,
                   int32_t line, const char *format, va_list args) {
    char *lv;
    char timeStr[20]; /* yyyy/mm/dd HH:MM:SS */
    struct tm tm;
    ruTimeVal tv;

    if (log_level >= RU_LOG_VERB)
        lv = "VERB";
    else if (log_level >= RU_LOG_INFO)
        lv = "INFO";
    else if (log_level >= RU_LOG_WARN)
        lv = "WARN";
    else if (log_level >= RU_LOG_CRIT)
        lv = "CRIT";
    else
        lv = "????";

    // https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
    ruGetTimeVal(&tv);
    // Round to nearest millis
    int millis = lrint(tv.usec / 1000.0);
    if (millis >= 1000) {
        // Allow for rounding up to nearest second
        millis -=1000;
        tv.sec++;
    }
#ifdef _WIN32
    _localtime32_s(&tm, &tv.sec);
#else
    localtime_r(&tv.sec, &tm);
#endif
    strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", &tm);
#ifdef RUMS
    DWORD pid = GetCurrentProcessId();
    #define prefix "%s.%03d [%ld] (%s:%d %s()) %s: "
#else
    pid_t pid = getpid();
    #ifdef _WIN32
        #ifdef _WIN64
            #define prefix "%s.%03d [%lld] (%s:%d %s()) %s: "
        #else
            #define prefix "%s.%03d [%d] (%s:%d %s()) %s: "
        #endif
    #else
        #define prefix "%s.%03d [%d] (%s:%d %s()) %s: "
    #endif
#endif
    char *file = ruBaseName((char*)filePath);

    // estimate
    char *ret = NULL;
    int32_t prefixSize = snprintf(ret, 0, prefix,
                                  timeStr, millis, pid, file, line, func, lv);
    va_list args2;
    va_copy(args2, args);
    int32_t msgsize  = vsnprintf(ret, 0, format, args2);
    va_end (args2);

    // do it +2 = \n\0
    ret = ruMalloc0(prefixSize+msgsize+2, char);
    char *ptr = ret;
    snprintf(ret, prefixSize+1, prefix, timeStr, millis, pid, file, line, func, lv);
    ptr += prefixSize;
#undef prefix
    vsnprintf(ptr, msgsize+1, format, args);
    ptr += msgsize;
    *ptr++ = '\n';
    *ptr = '\0';
    return ret;
}

char* ruMakeLogMsg(u_int32_t log_level, const char *filePath, const char *func,
                   int32_t line, const char *format, ...) {
    va_list args;
    va_start(args, format);
    char *out = makeLogMsg(log_level, filePath, func, line, format, args);
    va_end(args);
    return out;
}

void ruDoLog(u_int32_t log_level, const char *filePath, const char *func,
             int32_t line, const char *format, ...) {
    if (!ruDoesLog(log_level)) return;
    va_list args;
    va_start(args, format);
    char * _log_str_ = makeLogMsg(log_level, filePath, func, line, format, args);
    va_end(args);
    logger_(userLogData_, _log_str_);
    free(_log_str_);
}
