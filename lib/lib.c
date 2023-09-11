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

static void ruAbort(void) {
    // crash here
    abort();
}

void ruAbortf(trans_chars format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, EOL);
    ruAbort();
}

void ruAbortm(trans_chars msg) {
    fprintf(stderr, "%s", msg);
    fprintf(stderr, EOL);
    ruAbort();
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

RUAPI ptr ruClear(ptr o) {
    if (o) free(o);
    return NULL;
}

RUAPI ru_pid ruProcessId(void) {
#ifdef RUMS
    return GetCurrentProcessId();
#else
    return getpid();
#endif
}

RUAPI int32_t ruRunProg(const char **argv, sec_t timeout) {
    bool runDbg = 0;
    if (runDbg) {
        ruString com = ruStringNewf("-> run: ");
        for (int i = 0; argv[i]; i++) {
            ruStringAppendf(com, " '%s'", argv[i]);
        }
        ruDbgLogf("%s timeout: %d", ruStringGetCString(com), timeout);
        ruStringFree(com, false);
    }
#ifdef _WIN32
    if (timeout == RU_NO_TIMEOUT) {
        int ret = (int)_spawnve(_P_WAIT, argv[0], argv, NULL);
        if (ret == -1) {
            ruCritLogf("<- %s spawn failed with error: %d ret: %d",
                       argv[0], GetLastError(), RUE_RUN_FAILED);
            return RUE_RUN_FAILED;
        }
//        ruVerbLogf("<- %s returned: %d", argv[0], ret);
        return ret;
    }
    intptr_t spres = _spawnve(_P_NOWAIT, argv[0], argv, NULL);
    if (spres == -1) {
        ruCritLogf("<- %s spawn failed with error: %d ret: %d",
                   argv[0], GetLastError(), RUE_RUN_FAILED);
        return RUE_RUN_FAILED;
    }
    HANDLE pid = (HANDLE)spres;
    int32_t ret = 0;

    if (timeout > 0) {
        // blocking
        int32_t timetaken = 1;
        DWORD waitres;
        do {
            waitres = WaitForSingleObject(pid, 1000 );
            //ruVerbLogf("WaitForSingleObject returned: %x timetaken: %d", waitres, timetaken);
            if (waitres == WAIT_TIMEOUT) {
                // there is a timeout
                if (timetaken++ >= timeout) {
                    // it has been reached
                    ruWarnLogf("<- %s process timed out ret: %d",
                               argv[0], RUE_TIMEOUT);
                    TerminateProcess(pid, 3);
                    ret = RUE_TIMEOUT;
                    break;
                }
                ruSleep(1);
            } else {
                DWORD exitCode = 0;
                GetExitCodeProcess(pid, &exitCode);
                ret = (int32_t) exitCode;
                break;
            }
        } while(1);
    }
//    ruVerbLogf("<- %s returned: %d", argv[0], ret);
    CloseHandle(pid);
    return ret;
#else
    uint8_t chldfail = 234;
    int32_t status = 0;
    ru_pid pid = fork();
    if (pid == -1) {
        ruCritLogf("Failed to fork ret: %d", RUE_FORK_FAILED);
        return RUE_FORK_FAILED;
    }
    if (0 == pid) {
        // the child
        if (timeout == RU_NON_BLOCK) {
            // close file handles
            close(2);
            close(1);
            close(0);
            // child becomes daemon
            if (setsid() == -1) {
                ruCritLog("failed to become session leader");
                exit(chldfail);
            }
        }
        if (-1 == execve(argv[0], (char **)argv , NULL)) {
            // we're here so thing went bad
            // we must exit to prevent the child process from wreaking havoc
            ruCritLog("execve failed");
            exit(chldfail);
        }
        ruCritLogf("%s never reached failed ret: %d", argv[0], 0);
        // never reached
    }

    // the parent
    if (runDbg) ruVerbLogf("%s child pid: %d", argv[0], pid);
    if (timeout == RU_NON_BLOCK) {
        // hang a little trying to get the status
        ruSleepMs(10);
    }

    int32_t timetaken = 0;
    while (0 == waitpid(pid , &status , WNOHANG)) {
        if (timeout < 0) break; // non blocking
        // blocking
        if (timeout) {
            // there is a timeout
            if (timetaken++ > timeout) {
                // it has been reached
                ruWarnLogf("<- process [%d] timed out ret: %d", pid,  RUE_TIMEOUT);
                kill(pid, SIGKILL);
                return RUE_TIMEOUT;
            }
        }
        ruSleep(1);
    }

    if (runDbg) {
        ruDbgLogf("%s WEXITSTATUS %d WIFEXITED %d [status %d]",
                  argv[0], WEXITSTATUS(status), WIFEXITED(status), status);
    }
    if (WEXITSTATUS(status) == chldfail) {
        ruCritLogf("<- child execve failed ret: %d", RUE_RUN_FAILED);
        return RUE_RUN_FAILED;
    }
    if (runDbg) ruVerbLogf("<- %s returned: %d", argv[0], WEXITSTATUS(status));
    return WEXITSTATUS(status);
#endif
}

RUAPI alloc_chars ruGetLanguage(void) {
    alloc_chars lc = setlocale(LC_ALL, NULL);
    if (!lc) return NULL;
    return ruStrNDup(lc, 2);
}

RUAPI int32_t ruGetTimeVal(ruTimeVal *result) {
    if (!result) return RUE_PARAMETER_NOT_SET;
#ifndef RUMS
    ruZeroedStruct(struct timeval, r);

    gettimeofday (&r, NULL);
    result->sec = r.tv_sec;
    result->usec = r.tv_usec;
#else
    const int64_t delta = 116444736000000000;
    const int64_t m1 =   1000000;
    const int64_t m10 = 10000000;
    union {
        int64_t ns;
        FILETIME ft;
    } now;

    GetSystemTimeAsFileTime (&now.ft);
    result->sec = (sec_t)((now.ns - delta) / m10);
    result->usec = (usec_t)((now.ns / 10) % m1);
#endif
    return RUE_OK;
}

RUAPI usec_t ruTimeUs(void) {
    ruZeroedStruct(ruTimeVal, tv);
    ruGetTimeVal(&tv);
    usec_t micros = 1000000;
    micros *= tv.sec;
    return micros + tv.usec;
}

RUAPI msec_t ruTimeMs(void) {
    ruZeroedStruct(ruTimeVal, tv);
    ruGetTimeVal(&tv);
    // Round to nearest millis
    msec_t millis = 1000;
    millis *= tv.sec;
    millis += (tv.usec / 1000);
    return millis;
}

RUAPI sec_t ruTimeSec(void) {
    ruZeroedStruct(ruTimeVal, tv);
    ruGetTimeVal(&tv);
    return tv.sec;
}

RUAPI bool ruTimeUsEllapsed(usec_t stamp) {
    return stamp <= ruTimeUs();
}

RUAPI bool ruTimeMsEllapsed(msec_t stamp) {
    return stamp <= ruTimeMs();
}

RUAPI bool ruTimeEllapsed(sec_t stamp) {
    return stamp <= ruTimeSec();
}

RUAPI void ruSleepMs(msec_t milliseconds) {
#ifdef RUMS
    Sleep ((DWORD)milliseconds);
#else
    ruZeroedStruct(struct timespec, request);
    ruZeroedStruct(struct timespec, remaining);
    request.tv_sec = milliseconds / 1000;
    request.tv_nsec = 1000000 * (milliseconds % 1000);
    while (nanosleep (&request, &remaining) == -1 && errno == EINTR) request = remaining;
#endif
}

RUAPI void ruSleepUs(usec_t microseconds) {
#ifdef RUMS
    DWORD msecs = (DWORD)(microseconds / 1000);
    if ((microseconds % 1000) != 0) msecs++; // round up
    Sleep (msecs);
#else
    ruZeroedStruct(struct timespec, request);
    ruZeroedStruct(struct timespec, remaining);
    request.tv_sec = microseconds / 1000000;
    request.tv_nsec = 1000 * (microseconds % 1000000);
    while (nanosleep (&request, &remaining) == -1 && errno == EINTR) request = remaining;
#endif
}

#ifndef _WIN32

sec_t timeParse(trans_chars dateformat, trans_chars datestr, bool utc) {
    ruZeroedStruct(struct tm, t);
    if (!dateformat || !datestr) return -1;
    strptime(datestr, dateformat, &t);
    if (utc) {
        return timegm(&t);
    } else {
#ifdef __EMSCRIPTEN__
        // TODO: find a better way
        return 0;
#else
        return timelocal(&t);
#endif
    }
}
#endif

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

#ifdef _WIN32
    if (!timesecs) {
        ruGetTimeVal(&tv);
        timesecs = tv.sec;
    }
    const time_t tsecs = (time_t)timesecs;
    if (utc) {
        gmtime_s(&tm, &tsecs);
    } else {
        localtime_s(&tm, &tsecs);
    }
#else
    if (!timesecs) {
        // https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
        ruGetTimeVal(&tv);
    } else {
        tv.sec = timesecs;
    }
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

#ifdef _WIN32
RUAPI sec_t ruTimeLocalToUtc(sec_t stamp) {
    ruZeroedStruct(struct tm, lt);
    const time_t tsecs = (time_t)stamp;
    gmtime_s(&lt, &tsecs);
    return (sec_t)mktime(&lt);
}

RUAPI sec_t ruTimeUtcToLocal(sec_t stamp) {
    ruZeroedStruct(struct tm, lt);
    const time_t tsecs = (time_t)stamp;
    localtime_s(&lt, &tsecs);
    return (sec_t)_mkgmtime(&lt);
}
#else
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
#endif
