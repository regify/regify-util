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

RUAPI perm_chars ruGetOs(void) {
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

RUAPI alloc_chars ruGetHostname(void) {
    char name[256] = "";
#ifdef _WIN32
    DWORD len = 256;
    if(GetComputerNameA(name, &len)) {
#else
    int len = 256;
    if(!gethostname(name, len)) {
#endif
        return ruStrDup(name);
    }
    return NULL;
}

RUAPI void* ruMallocSize(rusize count, rusize ofsize) {
    if (!count || !ofsize) return NULL;
    void *p = calloc(count, ofsize);
    if (!p) {
        ruAbortf("failed to allocate %lu bytes", (count*ofsize));
    }
    return p;
}

RUAPI alloc_ptr ruReallocSize(alloc_ptr buf, rusize count, rusize ofsize) {
    if (!count || !ofsize || !buf) return NULL;
    alloc_ptr p = realloc(buf, count*ofsize);
    if (!p) {
        ruAbortf("failed to reallocate %lu bytes", count*ofsize);
    }
    return p;
}

RUAPI alloc_ptr ruMemDup(trans_ptr buf, rusize size) {
    if (!size || !buf) return NULL;
    alloc_ptr dest = ruMallocSize(size, 1);
    memcpy(dest, buf, size);
    return dest;
}

RUAPI trans_chars ruGetenv(const char *variable) {
    if (!variable) return NULL;
    return getenv (variable);
}

RUAPI unsigned long ruSemiRandomNumber(unsigned long max, long offset) {
    static long threadcounter = 0;
    ruTimeVal tv;
    ruGetTimeVal(&tv);
    long value = (long)((tv.usec ^ tv.sec ) + threadcounter++);
    return (value % max) + offset;
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

#if !defined(__ANDROID__)

RUAPI ruList ruIpAddrs(int32_t ipfilter) {
    ruList out = NULL;
    // to free
#ifdef _WIN32
    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    // default to unspecified address family (both)
    ULONG family = AF_UNSPEC;
    if (ipfilter == RU_IP4) {
        family = AF_INET;
    } else if (ipfilter == RU_IP6) {
        family = AF_INET6;
    }

    PIP_ADAPTER_ADDRESSES ias = NULL;
    ULONG bufLen = 15000;

    DWORD ret = NO_ERROR;
    do {
        for (int tries = 1; tries <= 3; tries++) {
            ias = (IP_ADAPTER_ADDRESSES *) ruMalloc0(bufLen, char);
            ret = GetAdaptersAddresses(family, flags, NULL, ias, &bufLen);
            if (ret == ERROR_BUFFER_OVERFLOW) {
                ruFree(ias);
                ias = NULL;
            } else {
                break;
            }
        }
        if (ret != NO_ERROR) {
            if (ret == ERROR_NO_DATA) {
                ruDbgLog("No addresses were found for the requested parameters");
                break;
            }
            ruWarnLogf("Call to GetAdaptersAddresses failed with error: %d", ret);
            LPVOID lpMsgBuf = NULL;
            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                              FORMAT_MESSAGE_FROM_SYSTEM |
                              FORMAT_MESSAGE_IGNORE_INSERTS,
                              NULL, ret,
                              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                // Default language
                              (LPTSTR) &lpMsgBuf, 0, NULL)) {
                ruSetError("Error: %s", lpMsgBuf);
                LocalFree(lpMsgBuf);
                ruFree(ias);
                break;
            }
        }

        // If successful, output some information from the data we received
        PIP_ADAPTER_ADDRESSES ia = ias;
        while (ia) {
            PIP_ADAPTER_UNICAST_ADDRESS pUnicast = ia->FirstUnicastAddress;
            if (pUnicast != NULL) {
                for (int i = 0; pUnicast != NULL; i++) {
                    char buffer[INET6_ADDRSTRLEN] = {0,};
                    perm_chars ip = NULL;
                    LPSOCKADDR sa = pUnicast->Address.lpSockaddr;
                    ADDRESS_FAMILY af = sa->sa_family;
                    if (af == AF_INET && ipfilter & RU_IP4) {
                        ip = inet_ntop(af,
                                       &((struct sockaddr_in *) sa)->sin_addr,
                                       buffer, INET_ADDRSTRLEN);
                    } else if (af == AF_INET6 && ipfilter & RU_IP6) {
                        ip = inet_ntop(af,
                                       &((struct sockaddr_in6 *) sa)->sin6_addr,
                                       buffer, INET6_ADDRSTRLEN);
                    }
                    if (ip) {
                        if (!out) out = ruListNew(ruTypeStrDup());
                        ruDbgLogf("adding: %s", ip);
                        ruListAppend(out, ip);
                    }
                    pUnicast = pUnicast->Next;
                }
            }
            ia = ia->Next;
        }
    } while (0);

    ruFree(ias);
#else
    bool errset = false;
    struct ifaddrs* ias = NULL;

    do {
        if(getifaddrs(&ias)) {
            ruSetError("getifaddrs failed with %d - '%s'",
                       errno, strerror(errno));
           break;
        }
        for(struct ifaddrs* ia = ias; ia; ia = ia->ifa_next) {
            char buffer[INET6_ADDRSTRLEN] = {0, };
            perm_chars ip = NULL;
            if (!ia->ifa_addr) continue;
            sa_family_t af = ia->ifa_addr->sa_family;
            if (af == AF_INET && ipfilter & RU_IP4) {
                // Be aware that the `ifa_addr`, `ifa_netmask` and `ifa_data`
                // fields may be NULL
                ip = inet_ntoa(((struct sockaddr_in*)ia->ifa_addr)->sin_addr);
            } else if (af == AF_INET6 && ipfilter & RU_IP6) {
                ip = inet_ntop(af,
                          &((struct sockaddr_in6*)ia->ifa_addr)->sin6_addr,
                          buffer,INET6_ADDRSTRLEN);
            } else {
                continue;
            }
            if (!ip) {
                if (errset) {
                    ruWarnLogf("failed to get ip %d - '%s'",
                               errno, strerror(errno));
                } else {
                    ruSetError("failed to get ip %d - '%s'",
                               errno, strerror(errno));
                    errset = true;
                }
                continue;
            }
            if (!out) out = ruListNew(ruTypeStrDup());
            ruDbgLogf("adding from %s: %s", ia->ifa_name, ip);
            ruListAppend(out, ip);
        }
    } while (0);

    // cleanup
    if (ias) freeifaddrs(ias);
#endif
    return out;
}
#endif

RUAPI int32_t ruGetOptMap(ruMap* parms, trans_chars opts, int argc, char** argv) {
    if (!parms || !opts || !argc || !argv) return RUE_PARAMETER_NOT_SET;
    int32_t ret = RUE_FILE_NOT_FOUND;
    ruMap params = ruMapNew(ruTypeStrFree(), ruTypeStrDup());
    optind = 1;
    do {
        // needed for GNU implementations to be more strict and consistent
        putenv("POSIXLY_CORRECT=1");
        int c = getopt(argc, argv, opts);
        if (c == -1) {
            break;
        }
        if (c == (int)'?' || c == (int)':') {
            ret = RUE_INVALID_PARAMETER;
            continue;
        }
        ruMapPut(params, ruDupPrintf("%c", (uint8_t) c), optarg);
        if (ret == RUE_FILE_NOT_FOUND) ret = RUE_OK;
    } while (1);

    if (ret == RUE_OK) {
        *parms = params;
    } else {
        ruMapFree(params);
    }
    return ret;
}

#if defined(_WIN32)
RUAPI int32_t ruGetOptMapW(ruMap* parms, trans_chars opts, DWORD wargc, LPWSTR* wargv) {
    if (!parms || !opts || !wargc || !wargv) return RUE_PARAMETER_NOT_SET;
    int32_t ret = RUE_FILE_NOT_FOUND;
    int argc = (int)wargc;
    char** argv = ruMalloc0(argc, char*);
    for (int i = 0; i < argc; i++) {
        argv[i] = ruStrFromUtf16(wargv[i]);
    }
    ret = ruGetOptMap(parms, opts, argc, argv);
    for (int i = 0; i < argc; i++) {
        ruFree(argv[i]);
    }
    ruFree(argv);
    return ret;
}

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
        ruDbgLogf("failed getting volume info for '%s'. EC: %d",
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
                                  (alloc_bytes)buf, &bufSize);
            if (rc != ERROR_SUCCESS) {
                ruVerbLogf("Failed querying registry for '%s' %d", leaf, rc);
                break;
            }
            *value = uniToChar(buf);
            ret = RUE_OK;
        } else if (type == REG_DWORD) {
            bufSize = sizeof(int);
            int val = 0;
            rc = RegQueryValueExW(hReg, wleaf, NULL, &type,
                                  (alloc_bytes)&val, &bufSize);
            if (rc != ERROR_SUCCESS) {
                break;
            }
            *value = ruDupPrintf("%d", val);
            ret = RUE_OK;
        } else if (type == REG_QWORD) {
            bufSize = sizeof(int64_t);
            int64_t val = 0;
            rc = RegQueryValueExW(hReg, wleaf, NULL, &type,
                                  (alloc_bytes)&val, &bufSize);
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

