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

RUAPI const char* ruVersion() {
    return RU_VERSION;
}

RU_THREAD_LOCAL char ruError[RU_ERRBUF_SIZE];
RU_THREAD_LOCAL int ruErrInit = 0;

RUAPI const char* ruLastError() {
    if (!ruErrInit) {
        ruError[0] = '\0';
        ruErrInit = 1;
    }
    return &ruError[0];
}

void ruClearError() {
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

RUAPI const char* ruGetOs() {
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

RUAPI char* ruGetHostname() {
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

RUAPI void* ruReallocSize(void *buf, rusize count, rusize ofsize) {
    ruClearError();
    if (!count || !ofsize || !buf) return NULL;
    void *p = realloc(buf, count*ofsize);
    if (!p) {
        ruCritLogf("failed to reallocate %lu bytes", count*ofsize);
        ruAbort();
    }
    return p;
}

RUAPI void* ruMemDup(void *buf, rusize size) {
    ruClearError();
    if (!size || !buf) return NULL;
    void *dest = ruMallocSize(size, 1);
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

