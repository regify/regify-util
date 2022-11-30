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

#ifdef BUILD_VERSION
    #define RU_VERSION BUILD_VERSION
#else
    #define RU_VERSION "0.0.0-dev"
#endif

RUAPI const char* ruVersion(void) {
    return RU_VERSION;
}

RU_THREAD_LOCAL char ruError[RU_ERRBUF_SIZE];
RU_THREAD_LOCAL int ruErrInit = 0;

RUAPI const char* ruLastError(void) {
    if (!ruErrInit) {
        ruError[0] = '\0';
        ruErrInit = 1;
    }
    return &ruError[0];
}

void ruClearError(void) {
    ruError[0] = '\0';
}

void ruSetError(const char *format, ...) {
    if (!format) {
        ruClearError();
        return;
    }
    va_list args;
    va_start (args, format);
    int32_t msgsize  = vsnprintf(ruError, RU_ERRBUF_SIZE, format, args);
    va_end (args);
    if (msgsize >= 0) {
        ruErrInit = 1;
    }
}

RUAPI const char* ruGetOs(void) {
#if defined (_WIN32)
    return "windows";
#elif defined(__ANDROID__)
    return "android";
#elif defined(__linux__)
    return "linux";
#elif defined(ITS_OSX)
    return "osx";
#elif defined(ITS_IOS)
    return "ios";
#elif defined(__unix__)
    return "unix";
#endif
}

RUAPI char* ruGetHostname(void) {
    char name[256] = "";
#ifdef _WIN32
    DWORD len = 256;
    if(GetComputerNameA(name, &len)) {
#else
    int len = 256;
    if(!gethostname(name, len)) {
#endif
        return ruStrdup(name);
    }
    return NULL;
}

RUAPI void* ruMallocSize(rusize count, rusize ofsize) {
    ruClearError();
    if (!count || !ofsize) return NULL;
    void *p = calloc(count, ofsize);
    if (!p) {
        ruCritLogf("failed to allocate %lu bytes", (count*ofsize));
        ruAbort();
    }
    return p;
}

RUAPI alloc_ptr ruReallocSize(alloc_ptr buf, rusize count, rusize ofsize) {
    ruClearError();
    if (!count || !ofsize || !buf) return NULL;
    alloc_ptr p = realloc(buf, count*ofsize);
    if (!p) {
        ruCritLogf("failed to reallocate %lu bytes", count*ofsize);
        ruAbort();
    }
    return p;
}

RUAPI alloc_ptr ruMemDup(trans_ptr buf, rusize size) {
    ruClearError();
    if (!size || !buf) return NULL;
    alloc_ptr dest = ruMallocSize(size, 1);
    memcpy(dest, buf, size);
    return dest;
}

RUAPI const char * ruGetenv(const char *variable) {
    ruClearError();
    if (!variable) return NULL;
    return getenv (variable);
}

RUAPI unsigned long ruSemiRandomNumber(unsigned long max, long offset) {
    static long threadcounter = 0;
    ruTimeVal tv;
    ruGetTimeVal(&tv);
    long value = (tv.usec ^ tv.sec ) + threadcounter++;
    return (value % max) + offset;
}

RUAPI int ruDateFormat(const char* format, rusize len, char* timeStr, long timesecs) {
    if (!format || ! len || !timeStr) return -1;
    struct tm tm;
    ruTimeVal tv;
    if (timesecs) {
        tv.sec = timesecs;
        tv.usec = 0;
    } else {
        // https://stackoverflow.com/questions/3673226/how-to-print-time-in-format-2009-08-10-181754-811
        ruGetTimeVal(&tv);
    }

#ifdef _WIN32
    _localtime32_s(&tm, &tv.sec);
#else
    localtime_r(&tv.sec, &tm);
#endif
    strftime(timeStr, len, format, &tm);
    return (int) tv.usec;
}

RUAPI int ruVersionComp(trans_chars ver1, trans_chars ver2) {
    // Cope with NULLs
    if (!ver1 && !ver2) return 0;
    if (!ver1) return -1;
    if (!ver2) return 1;
    // Courtesy
    // https://stackoverflow.com/questions/15057010/comparing-version-numbers-in-c/15059401#15059401
    // loop through each level of the version string
    while (true) {
        // extract leading version numbers
        char* tail1;
        char* tail2;
        unsigned long v1 = strtoul(ver1, &tail1, 10 );
        unsigned long v2 = strtoul(ver2, &tail2, 10 );
        // if numbers differ, then set the result
        if (v1 < v2) return -1;
        if (v1 > v2) return 1;

        // if numbers are the same, go to next level
        ver1 = tail1;
        ver2 = tail2;
        // if we reach the end of both, then they are identical
        if (*ver1 == '\0' && *ver2 == '\0') return 0;
        // if we reach the end of one only, it is the smaller
        if (*ver1 == '\0') return -1;
        if (*ver2 == '\0') return 1;
        //  not at end ... so far they match so keep going
        ver1++;
        ver2++;
    }
}

#if defined(_WIN32)
RUAPI int32_t ruGetVolumeInfo(const char* mountPoint,
        u_long* serialNo, u_long* maxCompLen, u_long* fsFlags,
        char** volumeName, char** fsName) {
    if (!mountPoint) return RUE_PARAMETER_NOT_SET;
    LPCWSTR root = (LPCWSTR)charToUni(mountPoint);

    UChar volbuf[256], fsbuf[256];
    u_long serNo = 0, compLen = 0, flags = 0;
    if (!GetVolumeInformationW(root,
            (wchar_t*)&volbuf,
            sizeof(volbuf) / sizeof(volbuf[0]),
            &serNo, &compLen, &flags,
            (wchar_t*)&fsbuf[0],
            sizeof(fsbuf) / sizeof(fsbuf[0]))) {
        ruVerbLogf("failed getting volume info for '%s'. EC: %d",
                mountPoint, GetLastError());
        return RUE_FILE_NOT_FOUND;
    }
    if (serialNo) *serialNo = serNo;
    if (maxCompLen) *maxCompLen = compLen;
    if (fsFlags) *fsFlags = flags;
    if (volumeName) *volumeName = uniToChar((UChar*)&volbuf);
    if (fsName) *fsName = uniToChar((UChar*)&fsbuf);
    ruFree(root);

    return RUE_OK;
}

RUAPI int32_t ruGetRegistryEntry(HKEY topKey, const char* tree, const char* leaf,
        char** value) {
    LPCWSTR wtree = (LPCWSTR)charToUni(tree);
    LPCWSTR wleaf = (LPCWSTR)charToUni(leaf);
    HKEY   hReg;
    UChar *buf = NULL;
    u_long bufSize = 0;
    u_long type;
    int32_t ret = RUE_PARAMETER_NOT_SET;
    if (!topKey || !tree || !leaf || !value) return ret;

    *value = NULL;
    LSTATUS rc = RegOpenKeyExW(topKey, wtree, 0,
            KEY_READ | KEY_QUERY_VALUE | KEY_WOW64_64KEY, &hReg);
    if (rc != ERROR_SUCCESS) {
        rc = RegOpenKeyExW(topKey, wtree, 0,
                            KEY_READ | KEY_QUERY_VALUE, &hReg);
        if (rc != ERROR_SUCCESS) {
            ruVerbLogf("Failed opening registry tree %s %d", tree, rc);
            return RUE_FILE_NOT_FOUND;
        }
    }
    do {
        ret = RUE_GENERAL;
        rc = RegQueryValueExW(hReg, wleaf, NULL, &type,
                              NULL, &bufSize);
        if (rc != ERROR_SUCCESS || !bufSize) {
            ruVerbLogf("Failed querying registry for '%s' %d", leaf, rc);
            break;
        }
        if (type == REG_SZ || type == REG_EXPAND_SZ || type == REG_MULTI_SZ) {
            buf = (UChar*)ruMallocSize(bufSize, sizeof(char));
            rc = RegQueryValueExW(hReg, wleaf, NULL, &type,
                                  (uchar*)buf, &bufSize);
            if (rc != ERROR_SUCCESS) {
                ruVerbLogf("Failed querying registry for '%s' %d", leaf, rc);
                break;
            }
            *value = uniNToChar(buf, (int32_t)bufSize);
            ret = RUE_OK;
        } else if (type == REG_DWORD) {
            bufSize = sizeof(int);
            int val = 0;
            rc = RegQueryValueExW(hReg, wleaf, NULL, &type,
                                  (uchar*)&val, &bufSize);
            if (rc != ERROR_SUCCESS) {
                break;
            }
            *value = ruDupPrintf("%d", val);
            ret = RUE_OK;
        } else if (type == REG_QWORD) {
            bufSize = sizeof(int64_t);
            int64_t val = 0;
            rc = RegQueryValueExW(hReg, wleaf, NULL, &type,
                                  (uchar*)&val, &bufSize);
            if (rc != ERROR_SUCCESS) {
                break;
            }
            *value = ruDupPrintf("%lld", val);
            ret = RUE_OK;
        }
    } while(0);

    RegCloseKey(hReg);
    ruFree(buf);

    return ret;
}
#endif

