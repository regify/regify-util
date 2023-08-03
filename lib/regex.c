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

ruMakeTypeGetter(regex, MagicRegex)

RUAPI ruRegex ruRegexNew(const char* pattern, ruRegexFlag flags, int32_t* code) {
    ruClearError();

    if (!pattern) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL);
    if (!strlen(pattern)) ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL);

    UErrorCode errorcode = U_ZERO_ERROR;
    UChar *upat = NULL;
    UParseError upe;

    // make unicode
    upat = charToUni(pattern);
    if (!upat) {
        ruSetError("Failed to get unicode from input. Invalid UTF8?");
        ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL);
    }

    URegularExpression* ure  = uregex_open(upat, -1, flags, &upe, &errorcode);
    ruFree(upat);
    if (U_FAILURE(errorcode)) {
        // Handle any syntax errors in the regular expression here
        ruSetError("Syntax error in regex? error=%s", u_errorName(errorcode));
        uregex_close(ure);
        ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL);
    }
    regex* re = ruMalloc0(1, regex);
    re->type = MagicRegex;
    re->mux = ruMutexInit();
    re->ex = ure;
    ruRetWithCode(code, RUE_OK, (ruRegex)re);
}

RUAPI ruRegex ruRegexFree(ruRegex rr) {
    ruClearError();
    regex* re = regexGet(rr, NULL);
    if (!re) return NULL;
    ruMutexFree(re->mux);
    uregex_close((URegularExpression*)re->ex);
    ruFree(re);
    return NULL;
}

RUAPI char* ruRegexReplace(ruRegex rr, const char* original, const char* replacement,
                         int32_t* code) {
    ruClearError();
    if (!rr || !original || !replacement) {
        ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL);
    }
    if(*original == '\0') {
        // uregex_replaceAll segfaults on empty sources
        // with U_STRING_NOT_TERMINATED_WARNING
        char *res = ruMalloc0(1, char);
        ruRetWithCode(code, RUE_OK, res);
    }
    regex* re = regexGet(rr, code);
    if (!re) return NULL;

    int32_t ret = RUE_OK, dlen = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    UChar *urep = NULL, *usrc = NULL, *udst = NULL;

    do {
        ruMutexLock(re->mux);
        usrc = charToUni(original);
        if (!usrc) {
            ret = RUE_INVALID_PARAMETER;
            break;
        }
        uregex_setText(re->ex, usrc, -1, &errorCode);

        urep = charToUni(replacement);
        if (!urep) {
            ret = RUE_INVALID_PARAMETER;
            break;
        }
        int32_t needed = uregex_replaceAll(re->ex, urep,
                                           -1, udst,
                                           dlen,&errorCode);
        dlen = needed + 1; // terminator
        if (errorCode == U_BUFFER_OVERFLOW_ERROR) {
            udst = ruMalloc0(dlen, UChar);
            errorCode = U_ZERO_ERROR;
            uregex_replaceAll(re->ex, urep,
                              -1, udst,
                              dlen, &errorCode);
        }
        if (U_FAILURE(errorCode) || udst[needed] != 0) {
            ruSetError("error in uregex_replaceAll error=%s",
                       u_errorName(errorCode));
            ret = RUE_GENERAL;
            break;
        }
    } while(0);

    ruFree(usrc);
    ruFree(urep);
    char *out = NULL;
    if (ret == RUE_OK) {
        out = uniToChar(udst);
    }
    ruMutexUnlock(re->mux);
    ruFree(udst);
    ruRetWithCode(code, ret, out);
}

bool ruRegexSearch(ruRegex rr, const char* original, bool fully, ruList matches, int32_t* code) {
    UBool does = false;
    if (!rr || !original) {
        ruRetWithCode(code, RUE_PARAMETER_NOT_SET, does);
    }
    regex* re = regexGet(rr, code);
    if (!re) return NULL;

    int32_t ret = RUE_OK;
    UErrorCode errorCode = U_ZERO_ERROR;
    UChar *usrc = NULL, *ubuf = NULL;

    do {
        ruMutexLock(re->mux);
        usrc = charToUni(original);
        if (!usrc) {
            ret = RUE_INVALID_PARAMETER;
            break;
        }
        uregex_setText(re->ex, usrc, -1, &errorCode);

        if (fully) {
            does = uregex_matches64(re->ex, 0, &errorCode);
        } else {
            does = uregex_find64(re->ex, 0, &errorCode);
        }
        if (U_FAILURE(errorCode)) {
            ruSetError("error in uregex_matches64 error=%s",
                     u_errorName(errorCode));
            ret = RUE_GENERAL;
            break;
        }
        if (!does || !matches) break;
        int32_t cnt = uregex_groupCount(re->ex, &errorCode);
        if (U_FAILURE(errorCode)) {
            ruSetError("error in uregex_groupCount error=%s",
                       u_errorName(errorCode));
            ret = RUE_GENERAL;
            break;
        }
        for (int i = 0; i <= cnt; i++) {
            int32_t needLen = uregex_group(re->ex, i,
                                           NULL, 0,
                                           &errorCode);
            if (U_BUFFER_OVERFLOW_ERROR != errorCode) {
                ruSetError("error in uregex_group error=%s",
                           u_errorName(errorCode));
                ret = RUE_GENERAL;
                break;
            }
            needLen++; // terminator
            ubuf = ruMalloc0(needLen, UChar);
            errorCode = U_ZERO_ERROR;
            uregex_group(re->ex, i, ubuf,
                         needLen*(int32_t)sizeof(UChar),
                         &errorCode);
            if (U_FAILURE(errorCode)) {
                ruSetError("error in uregex_group error=%s",
                           u_errorName(errorCode));
                ret = RUE_GENERAL;
                break;
            }
            char *buf = uniToChar(ubuf);
            ruListAppend(matches, buf);
            ruFree(ubuf);
        }
    } while(0);
    if (ret != RUE_OK) does = false;

    ruMutexUnlock(re->mux);
    ruFree(ubuf);
    ruFree(usrc);
    ruRetWithCode(code, ret, does);
}

RUAPI bool ruRegexMatch(ruRegex rr, const char* original, int32_t* code) {
    ruClearError();
    return ruRegexSearch(rr, original, true, NULL, code);
}

RUAPI bool ruRegexFind(ruRegex rr, const char* original, int32_t* code) {
    ruClearError();
    return ruRegexSearch(rr, original, false, NULL, code);
}

RUAPI ruList ruRegexMatchGroups(ruRegex rr, const char* original, int32_t* code) {
    ruClearError();
    ruList out = ruListNew(ruTypePtrFree());
    if (!ruRegexSearch(rr, original, true, out, code)) {
        return ruListFree(out);
    }
    if (!ruListSize(out, NULL)) {
        return ruListFree(out);
    }
    return out;
}

RUAPI ruList ruRegexFindGroups(ruRegex rr, const char* original, int32_t* code) {
    ruClearError();
    ruList out = ruListNew(ruTypePtrFree());
    if (!ruRegexSearch(rr, original, false, out, code)) {
        return ruListFree(out);
    }
    if (!ruListSize(out, NULL)) {
        return ruListFree(out);
    }
    return out;
}

