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
#include <locale.h>
#include "lib.h"

void ruAbort(void) {
    exit(1);
}

#ifdef TRACING
void U_CALLCONV traceData( const void *context, int32_t fnNumber, int32_t level,
        const char *fmt, va_list args) {

    char        buf[1000];
    const char *fnName;

    fnName = utrace_functionName(fnNumber);
    utrace_vformat(buf, sizeof(buf), 0, fmt, args);
    ruVerbLogf("trace '%s' - %s", fnName, buf);
}
#endif

/*
 * Source libcurl - escape.c -- Thank you Daniel.
 * Portable character check (remember EBCDIC). Do not use isalnum() because
 * its behavior is altered by the current locale.
 * See https://tools.ietf.org/html/rfc3986#section-2.3
 */
bool ruIsunreserved(unsigned char in) {
    switch(in) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case 'a': case 'b': case 'c': case 'd': case 'e':
        case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o':
        case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
        case 'A': case 'B': case 'C': case 'D': case 'E':
        case 'F': case 'G': case 'H': case 'I': case 'J':
        case 'K': case 'L': case 'M': case 'N': case 'O':
        case 'P': case 'Q': case 'R': case 'S': case 'T':
        case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
        case '-': case '.': case '_': case '~':
            return true;
        default:
            break;
    }
    return false;
}

RUAPI bool ruIsInt64(const char* numstr) {
    if (!numstr) {
        errno = EFAULT;
        return false;
    }
    errno = 0;
    char* end = (char*)numstr;
    strtoll(numstr, &end, 10);
    if (*end || errno == ERANGE) return false;
    return true;
}

RUAPI int32_t ruGetTimeVal(ruTimeVal *result) {
    ruClearError();
    if (!result) return RUE_PARAMETER_NOT_SET;
#ifndef RUMS
    ruZeroedStruct(struct timeval, r);

    gettimeofday (&r, NULL);
    result->sec = r.tv_sec;
    result->usec = r.tv_usec;
#else
    FILETIME ft;
    uint64_t time64;

    GetSystemTimeAsFileTime (&ft);
    memmove (&time64, &ft, sizeof (uint64_t));

    time64 -= 116444736000000000;
    time64 /= 10;

    result->sec = (long)(time64 / 1000000);
    result->usec = (long)(time64 % 1000000);
#endif
    return RUE_OK;
}

RUAPI sec_t ruTimeSec(void) {
    ruClearError();
    ruZeroedStruct(ruTimeVal, tv);
    ruGetTimeVal(&tv);
    return tv.sec;
}

RUAPI bool ruTimeEllapsed(sec_t stamp) {
    return stamp <= ruTimeSec();
}

sec_t timeParse(trans_chars dateformat, trans_chars datestr, bool utc) {
    ruZeroedStruct(struct tm, t);
    if (!dateformat || !datestr) return -1;
    strptime(datestr, dateformat, &t);
    if (utc) {
        return timegm(&t);
    } else {
        return timelocal(&t);
    }
}

RUAPI sec_t ruTimeParse(trans_chars dateformat, trans_chars datestr) {
    return timeParse(dateformat, datestr, false);
}

RUAPI sec_t ruUtcParse(trans_chars dateformat, trans_chars datestr) {
    return timeParse(dateformat, datestr, true);
}

int timeFormat(trans_chars format, rusize len, alloc_chars timeStr, sec_t timesecs, bool utc) {
    if (!format || ! len || !timeStr) return -1;
    ruZeroedStruct(struct tm, tm);
    ruZeroedStruct(ruTimeVal, tv);
    if (timesecs) {
        tv.sec = timesecs;
        tv.usec = 0;
    } else {
        // https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
        ruGetTimeVal(&tv);
    }

#ifdef _WIN32
    if (utc) {
        gmtime_r(&tv.sec, &tm);
    } else {
        _localtime32_s(&tm, &tv.sec);
    }
#else
    if (utc) {
        gmtime_r(&tv.sec, &tm);
    } else {
        localtime_r(&tv.sec, &tm);
    }
#endif
    strftime(timeStr, len, format, &tm);
    return (int) tv.usec;
}

RUAPI int ruTimeFormat(trans_chars format, rusize len, alloc_chars timeStr, sec_t timesecs) {
    return timeFormat(format, len, timeStr, timesecs, false);
}

RUAPI int ruUtcFormat(trans_chars format, rusize len, alloc_chars timeStr, sec_t timesecs) {
    return timeFormat(format, len, timeStr, timesecs, true);
}

RUAPI sec_t ruTimeLocalToUtc(sec_t stamp) {
    ruZeroedStruct(struct tm, lt);
    localtime_r(&stamp, &lt);
    return stamp - lt.tm_gmtoff;
}

RUAPI sec_t ruTimeUtcToLocal(sec_t stamp) {
    ruZeroedStruct(struct tm, lt);
    localtime_r(&stamp, &lt);
    return stamp + lt.tm_gmtoff;
}

RUAPI msec_t ruTimeMs(void) {
    ruClearError();
    ruZeroedStruct(ruTimeVal, tv);
    ruGetTimeVal(&tv);
    // Round to nearest millis
    msec_t millis = tv.sec * 1000;
    millis += tv.usec / 1000;
    return millis;
}

RUAPI bool ruTimeMsEllapsed(msec_t stamp) {
    return stamp <= ruTimeMs();
}

RUAPI usec_t ruTimeUs(void) {
    ruClearError();
    ruZeroedStruct(ruTimeVal, tv);
    ruGetTimeVal(&tv);
    usec_t micros = tv.sec * 1000000;
    return micros + tv.usec;
}

RUAPI bool ruTimeUsEllapsed(usec_t stamp) {
    return stamp <= ruTimeUs();
}

RUAPI alloc_chars ruGetLanguage(void) {
    alloc_chars lc = setlocale(LC_ALL, NULL);
    if (!lc) return NULL;
    return ruStrNDup(lc, 2);
}

RUAPI void ruSleepUs(usec_t microseconds) {
#ifdef RUMS
  Sleep (microseconds / 1000);
#else
    ruZeroedStruct(struct timespec, request);
    ruZeroedStruct(struct timespec, remaining);
    request.tv_sec = microseconds / 1000000;
    request.tv_nsec = 1000 * (microseconds % 1000000);
    while (nanosleep (&request, &remaining) == -1 && errno == EINTR) request = remaining;
#endif
}

RUAPI void ruSleepMs(msec_t milliseconds) {
#ifdef RUMS
    Sleep (milliseconds);
#else
    ruZeroedStruct(struct timespec, request);
    ruZeroedStruct(struct timespec, remaining);
    request.tv_sec = milliseconds / 1000;
    request.tv_nsec = 1000000 * (milliseconds % 1000);
    while (nanosleep (&request, &remaining) == -1 && errno == EINTR) request = remaining;
#endif
}
