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

ruMakeTypeGetter(Mux, MuxMagic)

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

RUAPI ruMutex ruMutexInit(void) {
    ruClearError();
    Mux *mx = ruMalloc0(1, Mux);
    mx->type = MuxMagic;
#ifdef RUMS
    mx->mux = CreateMutex( NULL, FALSE, NULL);
    if (mx->mux == NULL) {
        ruSetError("mutex init failed: %d\n", GetLastError());
		ruFree(mx);
		return NULL;
    }
#else
    int ret;
    if ((ret = pthread_mutex_init(&mx->mux, NULL))) {
        ruSetError("mutex init failed ec: %d", ret);
        ruFree(mx);
        return NULL;
    }
#endif
    return (ruMutex)mx;
}

RUAPI void ruMutexFree(ruMutex m) {
    ruClearError();
    int32_t ret;
    Mux *mux = MuxGet(m, &ret);
    if (!mux) return;
#ifdef RUMS
    CloseHandle(mux->mux);
#else
    pthread_mutex_destroy(&mux->mux);
#endif
    memset(mux, 0, sizeof(Mux));
    ruFree(mux);
}

RUAPI void ruMutexLock(ruMutex m) {
    ruClearError();
    int32_t ret;
    Mux *mux = MuxGet(m, &ret);
    if (!mux) {
        ruCritLogf("failed getting mutex %d", ret);
        ruAbort();
    }
#ifdef RUMS
    if (WAIT_OBJECT_0 != WaitForSingleObject(mux->mux, INFINITE)) {
        ruCritLog("failed unlocking mutex");
        ruAbort();
    }
#else
    pthread_mutex_lock(&mux->mux);
#endif
}

RUAPI void ruMutexUnlock(ruMutex m) {
    ruClearError();
    int32_t ret;
    Mux *mux = MuxGet(m, &ret);
    if (!mux) {
        ruCritLogf("failed getting mutex %d", ret);
        ruAbort();
    }
#ifdef RUMS
    ReleaseMutex(mux->mux);
#else
    pthread_mutex_unlock(&mux->mux);
#endif
}

RUAPI int32_t ruGetTimeVal(ruTimeVal *result) {
    ruClearError();
    if (!result) return RUE_PARAMETER_NOT_SET;
#ifndef RUMS
    struct timeval r;

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

RUAPI char* ruGetLanguage(void) {
    char* lc = setlocale(LC_ALL, NULL);
    if (!lc) return NULL;
    return ruStrndup(lc, 2);
}

RUAPI void ruUsleep(unsigned long microseconds) {
#ifdef RUMS
  Sleep (microseconds / 1000);
#else
  struct timespec request, remaining;
  request.tv_sec = microseconds / 1000000;
  request.tv_nsec = 1000 * (microseconds % 1000000);
  while (nanosleep (&request, &remaining) == -1 && errno == EINTR) request = remaining;
#endif
}
