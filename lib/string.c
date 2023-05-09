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
#include <stdlib.h>
#include "lib.h"

/*
 *  ruString class
 */
ruMakeTypeGetter(String, MagicString)

static void maybeAdd(String *str, rusize ilen) {
    rusize newlen = str->idx + ilen + 1;
    if (str->len < newlen) {
        newlen <<= 1; // double
        str->start = ruRealloc(str->start, newlen, char);
        str->len = newlen;
    }
}
static int32_t appendf(String *str, const char* format, va_list args) {
    int32_t ret = RUE_OK;
    if (!format) return RUE_PARAMETER_NOT_SET;
    static Mux* mux = NULL;
    if (!mux) mux = ruMuxInit();
    ruMuxLock(mux);
    va_list a2;
    va_copy(a2, args);
    // stream better be NULL else M$ crt craoks with -1
    int32_t size  = vsnprintf(NULL, 0, format, a2);
    va_end (a2);
    ruMuxUnlock(mux);
    if (size > 0) {
        maybeAdd(str, size);
        size = vsnprintf(str->start+str->idx, size+1, format, args);
        str->idx += size;
    }
    return ret;
}

static alloc_chars caseThis(trans_chars instr, rusize len, bool up) {
    ruClearError();
    if (!instr) return NULL;

    char *c, *out;
    char (*caser)(char) = up ? ruAsciiCharToUpper : ruAsciiCharToLower;

    if (len) {
        out = ruStrNDup(instr, len);
    } else {
        out = ruStrDup(instr);
    }
    for (c = out; *c; c++) *c = caser(*c);
    return out;
}

alloc_chars fixPath(const char *inPath) {
    if (!inPath) return NULL;
#ifdef RUMS
    char slash = '/';
#else
    char slash = '\\';
#endif
    char *c, *out = ruStrDup(inPath);
    for (c = out; *c; c++) if (*c == slash) *c = RU_SLASH;
    return out;
}

// Public
RUAPI void ruStrByteReplace(char* string, char search, char replace) {
    if (!string) return;
    for (char* c = string; *c; c++) {
        if (*c == search) *c = replace;
    }
}

RUAPI alloc_chars ruStrReplace(trans_chars instr, trans_chars search,
                               trans_chars replace) {
    if (!instr || !search) return NULL;
    // increment instr by search length - 1 to account for the ptr++ for incrementor
    rusize findInc = ruStrLen(search) - 1;
    rusize inLen = ruStrLen(instr);
    ruString buf = ruStringNewn("", inLen);
    for (trans_chars ptr = instr; *ptr; ptr++) {
        if (ruStrStartsWith(ptr, search, NULL)) {
            // replace
            ruStringAppend(buf, replace);
            ptr += findInc;
        } else {
            ruStringAppendn(buf, ptr, 1);
        }
    }
    alloc_chars out = ruStringGetCString(buf);
    ruStringFree(buf, true);
    return out;
}

RUAPI ruString ruStringNew(const char* instr) {
    return ruStringNewn(instr, 0);
}

RUAPI ruString ruStringNewn(const char* instr, rusize size) {
    ruClearError();
    if (!instr && !size) {
        ruSetError("Ignoring bogus invocation with NULL instr and 0 size");
        return NULL;
    }
    rusize len = size, slen = 0;
    if (instr) {
        len = slen = strlen(instr) + 1;
    }
    if (size > len) len = size;
    String *str = ruMalloc0(1, String);
    str->type = MagicString;
    str->start = ruMalloc0(len, char);
    if (instr) {
        strncpy(str->start, instr, slen);
        str->idx = slen - 1; // - \0
    }
    return (ruString)str;
}

RUAPI ruString ruStringNewf(const char* format, ...) {
    ruClearError();
    if (!format) {
        ruSetError("Ignoring bogus null format");
        return NULL;
    }
    String *str = ruMalloc0(1, String);
    str->type = MagicString;
    static Mux* mux = NULL;
    if (!mux) mux = ruMuxInit();
    ruMuxLock(mux);
    va_list args;
    va_start (args, format);
    int32_t size  = vsnprintf(str->start, 0, format, args);
    va_end (args);
    ruMuxUnlock(mux);
    if (size > 0) {
        size++; // terminator
        str->len = size * 2;
        str->start = ruMalloc0(str->len, char);
        va_start (args, format);
        size = vsnprintf(str->start, size, format, args);
        va_end (args);
        str->idx = size;
    }
    return (ruString)str;
}

RUAPI int32_t ruStringReset(ruString rs) {
    ruClearError();
    int32_t ret;
    String *str = StringGet(rs, &ret);
    if (!str) return ret;
    memset(str->start, 0, str->len);
    str->idx = 0;
    return RUE_OK;
}

RUAPI ruString ruStringFree(ruString rs, bool keepBuffer) {
    ruClearError();
    String *str = StringGet(rs, NULL);
    if (!str) {
        ruSetError("object to free was not a String, ignoring");
        return NULL;
    }
    if (!keepBuffer) ruFree(str->start);
    memset(str, 0, sizeof(String));
    ruFree(str);
    return NULL;
}

RUAPI int32_t ruStringAppend(ruString rs, const char* instr) {
    ruClearError();
    if (!instr) return RUE_PARAMETER_NOT_SET;
    return ruStringAppendn(rs, instr, strlen(instr));
}

RUAPI int32_t ruBufferAppendUriEncoded(ruString rs, const char* instr, rusize len) {
    ruClearError();
    int32_t ret;
    String *str = StringGet(rs, &ret);
    if (!str) return ret;
    if (!instr) return RUE_PARAMETER_NOT_SET;
    rusize i, olen, ilen;
    if (len) {
        ilen = len;
    } else {
        ilen = strlen(instr);
    }
    for (i = 0; i < ilen; i++) {
        char out[4];
        uchar c = instr[i];
        if (ruIsunreserved(c)) {
            olen = 1;
            out[0] = c;
        } else {
            olen = 3;
            snprintf(out, 4, "%%%02x", c);
        }
        maybeAdd(str, olen);
        memcpy(str->start + str->idx, out, olen);
        str->idx += olen;
    }
    *(str->start + str->idx) = '\0';
    return ret;
}

RUAPI int32_t ruStringAppendUriEncoded(ruString rs, const char* instr) {
    return ruBufferAppendUriEncoded(rs, instr, 0);
}

RUAPI int32_t ruStringAppendn(ruString rs, const char* instr, rusize len) {
    ruClearError();
    int32_t ret;
    String *str = StringGet(rs, &ret);
    if (!str) return ret;
    if (!instr) return RUE_PARAMETER_NOT_SET;
    rusize ilen;
    if (len == RU_SIZE_AUTO) {
        ilen = strlen(instr);
    } else {
        ilen = len;
    }
    maybeAdd(str, ilen);
    memcpy(str->start + str->idx, instr, ilen);
    str->idx += ilen;
    *(str->start + str->idx) = '\0';
    return ret;
}

RUAPI int32_t ruStringAppendf(ruString rs, const char* format, ...) {
    ruClearError();
    int32_t ret;
    String *str = StringGet(rs, &ret);
    if (!str) return ret;
    static Mux* mux = NULL;
    if (!mux) mux = ruMuxInit();
    ruMuxLock(mux);
    va_list args;
    va_start (args, format);
    ret = appendf(str, format, args);
    va_end (args);
    ruMuxUnlock(mux);
    return ret;
}

RUAPI char* ruStringGetCString(ruString rs) {
    ruClearError();
    String *str = StringGet(rs, NULL);
    if (!str) return NULL;
    return str->start;
}

RUAPI rusize ruStringLen(ruString rs, int32_t *code) {
    ruClearError();
    int32_t ret;
    String *str = StringGet(rs, &ret);
    if (!str) ruRetWithCode(code, ret, 0);
    ruRetWithCode(code, RUE_OK, str->idx);
}

RUAPI bool ruStringEndsWith(ruString rs, const char *suffix, int32_t *code) {
    ruClearError();
    int32_t ret;
    String *str = StringGet(rs, &ret);
    if (!str) ruRetWithCode(code, ret, false);
    if (!suffix) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, false);

    rusize slen = strlen(suffix);
    if(str->len >= slen) {
        if(!strcmp(str->start + str->idx - slen, suffix)) {
            ruRetWithCode(code, RUE_OK, true);
        }
    }
    ruRetWithCode(code, RUE_OK, false);
}

RUAPI char* ruDupvPrintf(const char* format, va_list arglist) {
    ruClearError();
    char *ret = NULL;
    if (!format) return ret;
    static Mux* mux = NULL;
    if (!mux) mux = ruMuxInit();
    ruMuxLock(mux);
    va_list args;
    va_copy (args, arglist);
    int32_t size  = vsnprintf(ret, 0, format, args);
    va_end (args);
    ruMuxUnlock(mux);
    if (size > 0) {
        size++; // terminator
        ret = ruMalloc0(size, char);
        va_copy (args, arglist);
        vsnprintf(ret, size, format, args);
        va_end (args);
    }
    return ret;
}

/*
 *  General string utilities
 *
 */
RUAPI char* ruDupPrintf(const char* format, ...) {
    char *ret = NULL;
    if (!format) return ret;
    static Mux* mux = NULL;
    if (!mux) mux = ruMuxInit();
    ruMuxLock(mux);
    va_list args;
    va_start (args, format);
    ret = ruDupvPrintf(format, args);
    va_end (args);
    ruMuxUnlock(mux);
    return ret;
}

RUAPI char* ruStrDup(const char* str) {
    ruClearError();
    if (!str) return NULL;
#if _WIN32
    char *ret = _strdup(str);
#else
    char *ret = strdup(str);
#endif
    if (!ret) {
        ruCritLog("failed to duplicate string");
        ruAbort();
    }
    return ret;
}

RUAPI rusize ruStrLen(trans_chars str) {
    if (!str) return 0;
    return strlen(str);
}

RUAPI char* ruStrNDup(trans_chars str, rusize len) {
    ruClearError();
    if (!str) return NULL;
    rusize inlen = strlen(str);
    if (len < inlen) inlen = len;
    char *ret = ruMalloc0(inlen + 1, char); // terminator
    memcpy(ret, str, inlen);
    return ret;
}

RUAPI int32_t ruStrCmp(trans_chars str1, trans_chars str2) {
    ruClearError();
    if (str1 == str2) return 0;      // same pointer both NULL
    if (!str1) return -1;
    if (!str2) return 1;
    int32_t ret = strcmp(str1, str2);
    if (!ret) return 0;
    return ret > 0 ? 1 : -1;
}

RUAPI bool ruStrEmpty(trans_chars str) {
    if (!str) return true;
    while (*str) {
        if (!isspace((unsigned char)*str)) return false;
        str++;
    }
    return true;
}

RUAPI bool ruStrEquals(trans_chars str1, trans_chars str2) {
    ruClearError();
    if (str1 == str2) return true;      // same pointer both NULL
    if (!str1 || !str2) return false;   // one NULL
    return strcmp(str1, str2) == 0;     // same string
}

RUAPI int32_t ruStrCaseCmp(trans_chars str1, trans_chars str2) {
    ruClearError();
    if (!str1 && !str2) return 0;
    if (!str1) return -1;
    if (!str2) return 1;
    int32_t ret = strcasecmp(str1, str2);
    if (!ret) return 0;
    return ret > 0 ? 1 : -1;
}

RUAPI bool ruStrCaseEquals(trans_chars str1, trans_chars str2) {
    ruClearError();
    if (str1 == str2) return true;      // same pointer both NULL
    if (!str1 || !str2) return false;   // one NULL
    return strcasecmp(str1, str2) == 0;     // same string
}

RUAPI bool ruStrStartsWith(trans_chars str, trans_chars prefix, int32_t *code) {
    ruClearError();
    if (!str || !prefix) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, false);

    rusize len = strlen(str);
    rusize slen = strlen(prefix);
    if (!slen) ruRetWithCode(code, RUE_OK, true);
    if(len >= slen) {
        if(!strncmp(str, prefix, slen)) {
            ruRetWithCode(code, RUE_OK, true);
        }
    }
    ruRetWithCode(code, RUE_OK, false);
}

RUAPI bool ruStrCaseStartsWith(trans_chars str, trans_chars prefix, int32_t *code) {
    ruClearError();
    if (!str || !prefix) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, false);

    rusize len = strlen(str);
    rusize slen = strlen(prefix);
    if (!slen) ruRetWithCode(code, RUE_OK, true);
    if(len >= slen) {
        if(!strncasecmp(str, prefix, slen)) {
            ruRetWithCode(code, RUE_OK, true);
        }
    }
    ruRetWithCode(code, RUE_OK, false);
}

RUAPI bool ruStrEndsWith(const char* str, const char *suffix, int32_t *code) {
    ruClearError();
    if (!str || !suffix) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, false);

    rusize len = strlen(str);
    rusize slen = strlen(suffix);
    if (!slen) ruRetWithCode(code, RUE_OK, true);
    if(len >= slen) {
        if(!strcmp(str + len - slen, suffix)) {
            ruRetWithCode(code, RUE_OK, true);
        }
    }
    ruRetWithCode(code, RUE_OK, false);
}

RUAPI bool ruStrCaseEndsWith(trans_chars str, trans_chars suffix, int32_t *code) {
    ruClearError();
    if (!str || !suffix) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, false);

    rusize len = strlen(str);
    rusize slen = strlen(suffix);
    if (!slen) ruRetWithCode(code, RUE_OK, true);
    if(len >= slen) {
        if(!strcasecmp(str + len - slen, suffix)) {
            ruRetWithCode(code, RUE_OK, true);
        }
    }
    ruRetWithCode(code, RUE_OK, false);
}

RUAPI bool ruStrHasChar(trans_chars haystack, char needle) {
    if (!haystack) return false;
    while(*haystack) {
        if (needle == *haystack) return true;
        haystack++;
    }
    return false;
}

RUAPI perm_chars ruStrStrip(perm_chars instr, trans_chars unwanted,
                            alloc_chars* newstr) {
    if (!instr || !newstr) return NULL;
    if (unwanted && *unwanted == '\0') return instr;
    if (*instr == '\0') return instr;

    rusize len = ruStrLen(instr);
    trans_bytes firstChar = (trans_bytes)instr;
    trans_bytes inPast = firstChar + len;
    trans_bytes lastChar = inPast - 1;
    alloc_bytes new = NULL;
    trans_bytes readChar = firstChar;
    alloc_bytes writeChar = (alloc_bytes)readChar;

    if (unwanted) {
        for (readChar = firstChar; readChar <= lastChar; readChar++) {
            trans_bytes haystack = (trans_bytes)unwanted;
            bool skip = false;
            while(*haystack) {
                if (*readChar == *haystack) {
                    skip = true;
                    break;
                }
                haystack++;
            }
            if (skip) {
                // found
                if (!new) {
                    new = (alloc_bytes)ruStrDup(instr);
                    readChar = writeChar = new + (readChar - firstChar);
                    lastChar = new + len - 1;
                }
                continue;
            }
            if (new) *writeChar = *readChar;
            writeChar++;
        }

    } else {
        for (readChar = firstChar; readChar <= lastChar; readChar++) {
            if (isspace(*readChar)) {
                // found
                if (!new) {
                    new = (alloc_bytes)ruStrDup(instr);
                    readChar = writeChar = new + (readChar - firstChar);
                    lastChar = new + len - 1;
                }
                continue;
            }
            if (new) *writeChar = *readChar;
            writeChar++;
        }
    }

    if (new) {
        *writeChar = '\0';
        *newstr = (alloc_chars)new;
        return (perm_chars)new;
    }
    return instr;
}

RUAPI perm_chars ruStrTrim(perm_chars instr, trans_chars unwanted,
                           enum ruTrimSide ends, alloc_chars* newstr) {
    if (!instr || !newstr) return NULL;
    if (unwanted && *unwanted == '\0') return instr;

    // size up the string
    rusize len = ruStrLen(instr);
    trans_chars inPast = instr + len;
    trans_bytes inLast = (trans_bytes)(inPast - 1);
    trans_bytes trimStart = (trans_bytes)instr;
    trans_bytes trimEnd = inLast;

    if (ends != ruTrimStart) {
        // check end
        if (unwanted) {
            while (trimEnd > trimStart) {
                trans_bytes haystack = (trans_bytes)unwanted;
                while(*haystack) {
                    if (*trimEnd == *haystack) break;
                    haystack++;
                }
                if (!*haystack) break;
                trimEnd--;
            }
        } else {
            while (trimEnd > trimStart) {
                if (!isspace(*trimEnd)) break;
                trimEnd--;
            }
        }
    }

    if (ends != ruTrimEnd) {
        // check start
        if (unwanted) {
            while (trimStart < trimEnd) {
                trans_bytes haystack = (trans_bytes)unwanted;
                while(*haystack) {
                    if (*trimStart == *haystack) break;
                    haystack++;
                }
                if (!*haystack) break;
                trimStart++;
            }
        } else {
            while(trimStart < trimEnd) {
                if (!isspace(*trimStart)) break;
                trimStart++;
            }
        }
    }
    if (trimEnd < inLast) {
        *newstr = ruStrNDup((trans_chars)trimStart, trimEnd-trimStart+1);
        return *newstr;
    }
    *newstr = NULL;
    return (trans_chars)trimStart;
}

RUAPI trans_chars ruStrTrimBounds(trans_chars inStart, rusize inLen, rusize* outLen) {
    trans_chars outStart = NULL;
    if (outLen) *outLen = 0;
    if (!inLen || ruStrEmpty(inStart)) return outStart;

    rusize len = (inLen == RU_SIZE_AUTO)? strlen(inStart) : inLen;
    trans_chars inPast = inStart + len;
    trans_bytes inLast = (trans_bytes)(inPast - 1);

    trans_bytes trimStart = (trans_bytes)inStart;
    while(isspace(*trimStart) && trimStart < inLast) trimStart++;
    if (!isspace(*trimStart)) {
        outStart = (trans_chars)trimStart;
        if (outLen) {
            trans_bytes last = inLast; // not terminator
            while(last > trimStart && isspace(*last)) last--;
            if (!isspace(*last)) {
                *outLen = last - trimStart + 1;
            }
        }
    }
    return outStart;
}

RUAPI bool ruStrFindKeyVal(trans_chars inStart, rusize inLen, trans_chars delim,
                           trans_chars* keyStart, rusize* keyLen,
                           trans_chars* valStart, rusize* valLen) {
    if (keyStart) {
        *keyStart = NULL;
        if (keyLen) *keyLen = 0;
    }
    if (valStart) {
        *valStart = NULL;
        if (valLen) *valLen = 0;
    }
    if (!inStart || ! inLen) return false;

    rusize len = inLen;
    if (inLen == RU_SIZE_AUTO) len = strlen(inStart);

    trans_chars inEnd = inStart + len;
    rusize delimLen = delim? strlen(delim) : 0;
    trans_chars delimStart = ruStrStrLen(inStart, delim, len);

    if (keyStart) {
        // trim the key start
        rusize kLen = len;
        if (delimStart) kLen = delimStart - inStart;
        *keyStart = ruStrTrimBounds(inStart, kLen, keyLen);
    }
    if (!delimStart) return false;
    if (valStart) {
        // trim the value
        trans_chars vStart = delimStart + delimLen;
        rusize vLen = inEnd - vStart;
        *valStart = ruStrTrimBounds(vStart, vLen, valLen);
    }
    return true;
}

RUAPI trans_chars ruStrCaseStrLen(trans_chars haystack, trans_chars needle, rusize len) {
    ruClearError();
    if (!haystack || !needle) return NULL;
    rusize needleLen = strlen (needle);
    rusize haystackLen = strlen (haystack);
    if (len > 0 && len < haystackLen) {
        haystackLen = len;
    }

    if (!needleLen) return (char*)haystack;

    if (haystackLen < needleLen) return NULL;
    char *p = (char*)haystack;
    const char *stackEnd = haystack + haystackLen - needleLen;

    alloc_chars pin = ruAsciiToLower(needle);
    while (p <= stackEnd) {
        for (rusize i = 0; i < needleLen; i++) {
            if (ruAsciiCharToLower(p[i]) != pin[i]) goto next;
        }
        ruFree(pin);
        return (char *)p;
        next:
        p++;
    }
    ruFree(pin);
    return NULL;
}

RUAPI trans_chars ruStrStrLen(trans_chars haystack, trans_chars needle, rusize len) {
    ruClearError();
    if (!haystack || !needle) return NULL;
    rusize needleLen = strlen (needle);
    rusize haystackLen = strlen (haystack);
    if (len > 0 && len < haystackLen) {
        haystackLen = len;
    }

    if (!needleLen) return (char*)haystack;

    if (haystackLen < needleLen) return NULL;
    char *p = (char*)haystack;
    const char *stackEnd = haystack + haystackLen - needleLen;

    while (p <= stackEnd) {
        for (rusize i = 0; i < needleLen; i++) {
            if (p[i] != needle[i]) goto next;
        }
        return (char *)p;
        next:
        p++;
    }
    return NULL;
}

RUAPI bool ruStrNEquals(trans_chars str1, rusize s1len, trans_chars str2) {
    ruClearError();
    if (!s1len) return true;              // with no len they must match
    if (str1 == str2) return true;      // same pointer or both NULL
    if (!str1 || !str2) return false;   // one NULL
    return ruStrStrLen(str1, str2, s1len) == str1; // same string
}

RUAPI trans_chars ruStrStr(trans_chars haystack, trans_chars needle) {
    return ruStrStrLen(haystack, needle, 0);
}

RUAPI trans_chars ruLastSubStrLen(trans_chars haystack, trans_chars needle, rusize len) {
    ruClearError();
    if (!haystack || !needle) return NULL;

    rusize needleLen = strlen (needle);
    rusize haystackLen = strlen (haystack);
    if (len > 0 && len < haystackLen) {
        haystackLen = len;
    }
    if (!needleLen) return (char*)haystack;

    if (haystackLen < needleLen) return NULL;

    trans_chars p = (char*)haystack + haystackLen - needleLen;
    rusize needleEnd = needleLen - 1;

    while (p >= haystack) {
        rusize i = needleEnd;
        do {
            if (p[i] != needle[i]) goto next;
        } while (i--);
        return (char *)p;
        next:
        p--;
    }
    return NULL;
}

RUAPI trans_chars ruLastSubStr(trans_chars haystack, trans_chars needle) {
    return ruLastSubStrLen(haystack, needle, 0);
}

RUAPI ruList ruStrNSplit(trans_chars instr, rusize inlen, trans_chars delim, int32_t maxCnt) {
    ruClearError();
    if (!instr || !inlen || !delim || !delim[0]) return NULL;

    if (maxCnt < 1) maxCnt = INT_MAX;
    if (inlen == RU_SIZE_AUTO) inlen = strlen(instr);
    trans_chars inPast = instr + inlen;

    ruList strList = ruListNew(free);
    const char *remainder = instr;
    char *ptr = strstr(remainder, delim);
    if (ptr && ptr < inPast) {
        rusize delLen = strlen(delim);
        while (--maxCnt && ptr && ptr < inPast) {
            ruListAppend(strList, ruStrNDup(remainder, ptr - remainder));
            remainder = ptr + delLen;
            ptr = strstr(remainder, delim);
        }
    }
    if (remainder) {
        ruListAppend(strList, ruStrNDup(remainder, inPast - remainder));
    }
    return strList;
}

RUAPI ruList ruStrSplit (trans_chars instr, trans_chars delim, int32_t maxCnt) {
    return ruStrNSplit(instr, RU_SIZE_AUTO, delim, maxCnt);
}

RUAPI char ruAsciiCharToLower(char in) {
    ruClearError();
    if (in >= 'A' && in <= 'Z') return in + 0x20;
    return in;
}

RUAPI char ruAsciiCharToUpper(char in) {
    ruClearError();
    if (in >= 'a' && in <= 'z') return in - 0x20;
    return in;
}

RUAPI alloc_chars ruAsciiToLower(trans_chars instr) {
    return caseThis(instr, 0, false);
}

RUAPI char* ruAsciiToUpper(trans_chars instr) {
    return caseThis(instr, 0, true);
}

RUAPI alloc_chars ruAsciiNToLower(trans_chars instr, rusize len) {
    return caseThis(instr, len, false);
}

RUAPI alloc_chars ruAsciiNToUpper(trans_chars instr, rusize len) {
    return caseThis(instr, len, true);
}

RUAPI alloc_chars ruUtf8ToLower(trans_chars instr) {
    return utf8SwitchCase(instr, false);
}

RUAPI alloc_chars ruUtf8ToUpper(trans_chars instr) {
    return utf8SwitchCase(instr, true);
}

int32_t parseInteger(trans_chars start, perm_chars* endptr,
                     uint32_t intBitSize, uint32_t base, int64_t* out) {
    ruClearError();
    if (!out) return RUE_PARAMETER_NOT_SET;
    *out = 0;
    if (!start) return RUE_PARAMETER_NOT_SET;
    int64_t maxInt = 1;
    if (intBitSize < 1 || intBitSize > sizeof(maxInt)*8-1) return RUE_INVALID_PARAMETER;
    if (base == 1 || base > 36) return RUE_INVALID_PARAMETER;

    char* myStart = (char*)start;
    char* numStart = myStart; // after white space and prefix

    do {
        // determine max integer size
        for (uint32_t i = 1; i < intBitSize; i++) maxInt = maxInt | maxInt << 1;

        // Skip white space.
        char* cur = myStart;
        while (*cur == ' ' || *cur == '\n' || *cur == '\r' || *cur == '\t') ++cur;

        if (*cur == '\0') break; // done

        /* Check for a sign.  */
        bool negative = false;
        if (*cur == '-') {
            negative = true;
            ++cur;
        } else if (*cur == '+') {
            ++cur;
        }

        // Recognize number prefix and if BASE is zero, figure it out ourselves.
        if (*cur == '0') {
            if ((base == 0 || base == 16) &&
                (cur[1] == 'x' || cur[1] == 'X')) {
                cur += 2;
                base = 16;
            } else if (base == 0) {
                base = 8;
            }
        } else if (base == 0) {
            base = 10;
        }

        // Save the pointer so we can check later if anything happened.
        numStart = cur;
        int64_t cutoff = maxInt / base;
        int64_t cutlim = maxInt % base;
        if (!negative) cutlim++; // positive side gets one more

        bool overflow = false;
        int64_t parsedNum = 0;
        uchar c = *cur;
        for (; c; c = *++cur) {
            if (c >= '0' && c <= '9') {
                c -= '0'; // add number
            } else if (c >= 'A' && c <= 'Z') {
                c = c - 'A' + 10;
            } else if (c >= 'a' && c <= 'z') {
                c = c - 'a' + 10;
            } else {
                break;
            }
            if (c >= base) break;
            // Check for overflow.
            if (parsedNum > cutoff || (parsedNum == cutoff && c > cutlim)) {
                overflow = true;
                break;
            } else {
                parsedNum *= base;
                parsedNum += c;
            }
        }

        // Check if anything actually happened.
        if (cur == numStart) break;

        // Store in ENDPTR the address of one character
        // past the last character we converted.
        if (endptr) *endptr = cur;

        if (negative) {
            *out = -parsedNum;
        } else {
            *out = parsedNum;
        }
        if (overflow) {
            return RUE_OVERFLOW;
        }
        return RUE_OK;

    } while(0);

    // We must handle a special case here: the base is 0 or 16 and the
    // first two characters are '0' and 'x', but the rest are no
    // hexadecimal digits.  This is no error case.  We return 0 and
    // ENDPTR points to the `x`.
    if (endptr) {
        if (numStart - myStart >= 2 &&
            (numStart[-1] == 'x' || numStart[-1] == 'X') &&
             numStart[-2] == '0') {
            *endptr = &numStart[-1];
        } else {
            /*  There was no number to convert.  */
            *endptr = myStart;
        }
    }
    *out = 0;
    return RUE_OK;
}

RUAPI int32_t ruStrParseInt64(trans_chars str, perm_chars* endptr, uint32_t base, int64_t* num) {
    if (!str || !num) return RUE_PARAMETER_NOT_SET;
    int64_t out = 0;
    int32_t ret = parseInteger(str, endptr, sizeof(int64_t)*8-1, base, &out);
    *num = out;
    return ret;
}

RUAPI int64_t ruStrToInt64(trans_chars str) {
    if (!str) return 0;
    int64_t out = 0;
    parseInteger(str, NULL, sizeof(int64_t)*8-1, 10, &out);
    return out;
}

RUAPI int32_t ruStrParseLong(trans_chars str, perm_chars* endptr, uint32_t base, long* num) {
    if (!str || !num) return RUE_PARAMETER_NOT_SET;
    int64_t out = 0;
    int32_t ret = parseInteger(str, endptr, sizeof(long)*8-1, base, &out);
    *num = (long)out;
    return ret;
}

RUAPI long ruStrToLong(trans_chars str) {
    if (!str) return 0;
    int64_t out = 0;
    parseInteger(str, NULL, sizeof(long)*8-1, 10, &out);
    return (long)out;
}

RUAPI int32_t ruStrParseInt(trans_chars str, perm_chars* endptr, uint32_t base, int32_t* num) {
    if (!str || !num) return RUE_PARAMETER_NOT_SET;
    int64_t out = 0;
    int32_t ret = parseInteger(str, endptr, sizeof(int32_t)*8-1, base, &out);
    *num = (int32_t)out;
    return ret;
}

RUAPI int32_t ruStrToInt(trans_chars str) {
    if (!str) return 0;
    int64_t out = 0;
    parseInteger(str, NULL, sizeof(int32_t)*8-1, 10, &out);
    return (int32_t)out;
}
