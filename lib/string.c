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
    va_list a2;
    va_copy(a2, args);
    // stream better be NULL else M$ crt craoks with -1
    int32_t size  = vsnprintf(NULL, 0, format, a2);
    va_end (a2);
    if (size > 0) {
        maybeAdd(str, size);
        size = vsnprintf(str->start+str->idx, size+1, format, args);
        str->idx += size;
    }
    return ret;
}

static char* caseThis(const char *instr, rusize len, bool up) {
    ruClearError();
    if (!instr) return NULL;

    char *c, *out;
    char (*caser)(char) = up ? ruAsciiCharToUpper : ruAsciiCharToLower;

    if (len) {
        out = ruStrndup(instr, len);
    } else {
        out = ruStrdup(instr);
    }
    for (c = out; *c; c++) *c = caser(*c);
    return out;
}

char* fixPath(const char *inPath) {
    if (!inPath) return NULL;
#ifdef RUMS
    char slash = '/';
#else
    char slash = '\\';
#endif
    char *c, *out = ruStrdup(inPath);
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
    va_list args;
    if (!format) {
        ruSetError("Ignoring bogus null format");
        return NULL;
    }
    String *str = ruMalloc0(1, String);
    str->type = MagicString;
    va_start (args, format);
    int32_t size  = vsnprintf(str->start, 0, format, args);
    va_end (args);
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
    if (len) {
        ilen = len;
    } else {
        ilen = strlen(instr);
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
    va_list args;
    String *str = StringGet(rs, &ret);
    if (!str) return ret;
    va_start (args, format);
    ret = appendf(str, format, args);
    va_end (args);
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
    va_list args;
    char *ret = NULL;
    if (!format) return ret;
    va_copy (args, arglist);
    int32_t size  = vsnprintf(ret, 0, format, args);
    va_end (args);
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
    va_list args;
    char *ret = NULL;
    if (!format) return ret;
    va_start (args, format);
    ret = ruDupvPrintf(format, args);
    va_end (args);
    return ret;
}

RUAPI char* ruStrdup(const char* str) {
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

RUAPI char* ruStrndup(trans_chars str, rusize len) {
    ruClearError();
    if (!str) return NULL;
    rusize inlen = strlen(str);
    if (len < inlen) inlen = len;
    char *ret = ruMalloc0(inlen + 1, char); // terminator
    memcpy(ret, str, inlen);
    return ret;
}

RUAPI int32_t ruStrcmp(trans_chars str1, trans_chars str2) {
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

RUAPI int32_t ruStrcasecmp(trans_chars str1, trans_chars str2) {
    ruClearError();
    if (!str1 && !str2) return 0;
    if (!str1) return -1;
    if (!str2) return 1;
    int32_t ret = strcasecmp(str1, str2);
    if (!ret) return 0;
    return ret > 0 ? 1 : -1;
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

RUAPI bool ruStrEndsCaseWith(const char* str, const char *suffix, int32_t *code) {
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

RUAPI char* ruStrStrLen(const char* haystack, const char* needle, rusize len) {
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

RUAPI char* ruStrStr(const char* haystack, const char* needle) {
    return ruStrStrLen(haystack, needle, 0);
}

RUAPI char* ruLastSubstrLen(const char* haystack, const char* needle, rusize len) {
    ruClearError();
    if (!haystack || !needle) return NULL;

    rusize needleLen = strlen (needle);
    rusize haystackLen = strlen (haystack);
    if (len > 0 && len < haystackLen) {
        haystackLen = len;
    }
    if (!needleLen) return (char*)haystack;

    if (haystackLen < needleLen) return NULL;

    char *p = (char*)haystack + haystackLen - needleLen;
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

RUAPI char* ruLastSubstr(const char* haystack, const char* needle) {
    return ruLastSubstrLen(haystack, needle, 0);
}

RUAPI ruList ruStrsplit (const char *instr, const char *delim, int32_t maxCnt) {
    ruClearError();
    if (!instr || !delim || !delim[0]) return NULL;

    if (maxCnt < 1) maxCnt = INT_MAX;

    ruList strList = ruListNew(free);
    const char *remainder = instr;
    char *ptr = strstr(remainder, delim);
    if (ptr) {
        rusize delLen = strlen(delim);
        while (--maxCnt && ptr) {
            rusize len = ptr - remainder;
            ruListAppend(strList, ruStrndup (remainder, len));
            remainder = ptr + delLen;
            ptr = strstr(remainder, delim);
        }
    }
    if (remainder) {
        ruListAppend(strList, ruStrdup(remainder));
    }
    return strList;
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

RUAPI char* ruAsciiToLower(char *instr) {
    return caseThis(instr, 0, false);
}

RUAPI char* ruAsciiToUpper(char *instr) {
    return caseThis(instr, 0, true);
}

RUAPI char* ruAsciiNToLower(char *instr, rusize len) {
    return caseThis(instr, len, false);
}

RUAPI char* ruAsciiNToUpper(char *instr, rusize len) {
    return caseThis(instr, len, true);
}

RUAPI char* ruUtf8ToLower(const char *instr) {
    return utf8SwitchCase(instr, false);
}

RUAPI char* ruUtf8ToUpper(const char *instr) {
    return utf8SwitchCase(instr, true);
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
            } else if (c == ',') {
                // ignore thousand separators
                continue;
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

RUAPI int64_t ruStrToInt64(const char* numstr) {
    return ruStrParseInt64(numstr, NULL, 10);
}

static long parseLong(const char* numstr, bool strict) {
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
    return num;
}

RUAPI long ruStrParseLong(const char* numstr) {
    return parseLong(numstr,true);
}

RUAPI long ruStrToLong(const char* numstr) {
    return parseLong(numstr,false);
}

RUAPI char* ruStrTrim(char* instr) {
    if (instr) {
        char* p = instr + strlen(instr);
        while (p > instr && isspace((unsigned char) (*--p))) *p = '\0';
    }
    return instr;
}

RUAPI bool ruStrEmpty(trans_chars str) {
    if (!str) return true;
    while (*str) {
        if (!isspace(*str)) return false;
        str++;
    }
    return true;
}

RUAPI void ruStripChars(char *instr, const char* chars) {
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
