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

RUAPI alloc_chars ruStrStrip(alloc_chars instr, trans_chars asciichars) {
    if (!instr || !asciichars) return instr;

    char* p = instr + strlen(instr);
    while (p > instr) {
        if (!ruStrHasChar(asciichars, *--p)) break;
        *p = '\0';
    }
    return instr;
}

RUAPI alloc_chars ruStrTrim(alloc_chars instr) {
    if (instr) {
        char* p = instr + strlen(instr);
        while (p > instr && isspace((unsigned char) (*--p))) *p = '\0';
    }
    return instr;
}

RUAPI perm_chars ruStrTrimDup(trans_chars instr, alloc_chars* newstr) {
    if (!instr) ruRetWithCode(newstr, NULL, NULL);
    rusize inlen = strlen(instr);
    rusize outlen = 0;
    trans_chars start = ruStrTrimBounds(instr, inlen, &outlen);
    if (inlen != outlen || start != instr) {
        alloc_chars copy = ruStrNDup(start, outlen);
        ruRetWithCode(newstr, copy, copy);
    }
    ruRetWithCode(newstr, NULL, instr);
}

RUAPI trans_chars ruStrTrimBounds(trans_chars inStart, rusize inLen, rusize* outLen) {
    trans_chars outStart = NULL;
    if (outLen) *outLen = 0;
    if (!inLen || ruStrEmpty(inStart)) return outStart;

    rusize len = (inLen == RU_SIZE_AUTO)? strlen(inStart) : inLen;
    trans_chars inPast = inStart + len;
    trans_chars inLast = inPast - 1;

    trans_chars trimStart = inStart;
    while(isspace(*trimStart) && trimStart < inLast) trimStart++;
    if (!isspace(*trimStart)) {
        outStart = trimStart;
        if (outLen) {
            trans_chars last = inLast; // not terminator
            while(isspace(*last) && last > trimStart) last--;
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

RUAPI trans_chars ruLastSubstrLen(trans_chars haystack, trans_chars needle, rusize len) {
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

RUAPI trans_chars ruLastSubstr(trans_chars haystack, trans_chars needle) {
    return ruLastSubstrLen(haystack, needle, 0);
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
    return ruStrNSplit(instr, -1, delim, maxCnt);
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

RUAPI alloc_chars ruUcsToUtf8(const uint16_t* ucs) {
    return uniToChar((UChar*)ucs);
}

RUAPI int64_t ruStrParseInt64(const char *start, char **endptr, int base) {
    ruClearError();
    bool negative = false;
    bool overflow;
    int64_t cutoff;
    int64_t cutlim;
    int64_t i64;
    char *s, *save;
    uchar c;
    char *ptr = (char*)start;
    errno = 0;

    if (!start || base == 1 || base > 36) {
        errno = EINVAL;
        if (endptr) *endptr = ptr;
        return 0;
    }

    do {
        save = s = ptr;

        /* Skip white space.  */
        while (*s == ' ' || *s == '\n' || *s == '\r' || *s == '\t') ++s;

        if (*s == '\0') break; // done

        /* Check for a sign.  */
        if (*s == '-') {
            negative = true;
            ++s;
        } else if (*s == '+') {
            ++s;
        }

        /* Recognize number prefix and if BASE is zero, figure it out ourselves.  */
        if (*s == '0') {
            if ((base == 0 || base == 16) && (s[1] == 'x' || s[1] == 'X')) {
                s += 2;
                base = 16;
            } else if (base == 0) {
                base = 8;
            }
        } else if (base == 0) {
            base = 10;
        }

        /* Save the pointer so we can check later if anything happened.  */
        save = s;
        cutoff = RU_MAX_INT64 / base;
        cutlim = RU_MAX_INT64 % base;

        overflow = false;
        i64 = 0;
        c = *s;
        for (; c; c = *++s) {
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
            /* Check for overflow.  */
            if (i64 > cutoff || (i64 == cutoff && c > cutlim)) {
                overflow = true;
            } else {
                i64 *= base;
                i64 += c;
            }
        }

        /* Check if anything actually happened.  */
        if (s == save) break;

        /* Store in ENDPTR the address of one character
           past the last character we converted.  */
        if (endptr) *endptr = (char *) s;

        if (overflow) {
            errno = ERANGE;
            return RU_MAX_UINT64;
        }

        if (negative) return -i64;
        return i64;

    } while(0);

    /* We must handle a special case here: the base is 0 or 16 and the
       first two characters are '0' and 'x', but the rest are no
       hexadecimal digits.  This is no error case.  We return 0 and
       ENDPTR points to the `x`.  */
    if (endptr) {
        if (save - ptr >= 2 && (save[-1] == 'x' || save[-1] == 'X') && save[-2] == '0') {
            *endptr = &save[-1];
        } else {
            /*  There was no number to convert.  */
            *endptr = ptr;
        }
    }
    return 0;
}

RUAPI int64_t ruStrToInt64(trans_chars numstr) {
    return ruStrParseInt64(numstr, NULL, 10);
}

static long parseLong(trans_chars numstr, bool strict) {
    char* end = (char*)numstr;
    int64_t num = ruStrParseInt64(numstr, &end, 10);
    if (!strict) return (long)num;
    if (!numstr || *end) return 0;
    if (sizeof(long) < sizeof(int64_t)) {
        int64_t lng = (long)num;
        if (lng < num) {
            errno = ERANGE;
        }
    }
    if (errno == ERANGE) return 0;
    return (long)num;
}

RUAPI long ruStrParseLong(trans_chars numstr) {
    return parseLong(numstr,true);
}

RUAPI long ruStrToLong(trans_chars numstr) {
    return parseLong(numstr,false);
}

static int32_t parseInt(trans_chars numstr, bool strict) {
    char* end = (char*)numstr;
    int64_t num = ruStrParseInt64(numstr, &end, 10);
    if (!strict) return (int32_t)num;
    if (!numstr || *end) return 0;
    int32_t intout = (int32_t)num;
    if (intout < num) {
        errno = ERANGE;
        return 0;
    }
    return intout;
}

RUAPI int32_t ruStrParseInt(trans_chars numstr) {
    return parseInt(numstr,true);
}

RUAPI int32_t ruStrToInt(trans_chars numstr) {
    return parseInt(numstr,false);
}

RUAPI bool ruStrEmpty(trans_chars str) {
    if (!str) return true;
    while (*str) {
        if (!isspace(*str)) return false;
        str++;
    }
    return true;
}

RUAPI void ruStripChars(alloc_chars instr, trans_chars chars) {
    if (!instr || !strlen(instr) || ! chars || !strlen(chars)) return;
    char *end = instr + strlen(instr);
    const char *cend = chars + strlen(chars);
    char *o = instr;
    for (char *p = instr; p < end; p++) {
        bool skip = false;
        for (const char *c = chars; c < cend; c++) {
            if (*p == *c) {
                skip = true;
                break;
            }
        }
        if (!skip) {
            *o = *p;
            o++;
        }
    }
    *o = '\0';
}
