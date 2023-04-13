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

#ifdef __GNUC__
// A long story, not having this caused uregex_open to crash at
// umutex.cpp:175 initCondition->notify_all(); when statically linking.
// This workaround mitigates that.
// See C++11's condition variables fail with static linking
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=58909#c9
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull"
void pthread_cond_bug() {
    pthread_cond_signal((pthread_cond_t *) NULL);
    pthread_cond_init((pthread_cond_t *) NULL,
                      (const pthread_condattr_t *) NULL);
    pthread_cond_destroy((pthread_cond_t *) NULL);
    pthread_cond_timedwait((pthread_cond_t *) NULL, (pthread_mutex_t *)
            NULL, (const struct timespec *) NULL);
    pthread_cond_wait((pthread_cond_t *) NULL, (pthread_mutex_t *) NULL);
}
#pragma GCC diagnostic pop
#endif

UConverter* getConverter(void) {
    UErrorCode errorCode = U_ZERO_ERROR;
#ifdef TRACING
    const void* context = NULL;
    utrace_setFunctions(context, NULL, NULL, traceData);
    utrace_setLevel(UTRACE_VERBOSE);
#endif

    // get a converter
    UConverter *conv = ucnv_open("utf8", &errorCode);
    if(U_FAILURE(errorCode)) {
        ruCritLogf("error in ucnv_open error=%s\n", u_errorName(errorCode));
        ruAbort();
    }
    return conv;

}

UChar* convToUni(UConverter *conv, trans_chars  instr, int32_t inlen) {
    if (!conv || !instr) return NULL;
    if (inlen < 0) inlen = -1;
    UErrorCode errorCode = U_ZERO_ERROR;
    int32_t needed = ucnv_toUChars(conv, NULL, 0,
                                   instr, inlen, &errorCode);
    int32_t uclen = needed + 1; // terminator
    // make unicode
    UChar *usrc = ruMalloc0((rusize)uclen, UChar);
    errorCode = U_ZERO_ERROR;
    ucnv_toUChars(conv, usrc, uclen,
                  instr, inlen, &errorCode);
    if(U_FAILURE(errorCode)) {
        ruSetError("error in ucnv_toUChars error=%s\n",
                   u_errorName(errorCode));
        ruFree(usrc);
        return NULL;
    }
    return usrc;
}

alloc_chars convToStr(UConverter *conv, UChar *usrc, int32_t uclen) {
    UErrorCode errorCode = U_ZERO_ERROR;
    // convert it back
    if (uclen < 0) {
        // make sure
        uclen = -1;
    } else {
        // we accept bytes, but ucnv seems to want chars
        uclen /= 2;
        if (!usrc[uclen-1]) uclen -= 1; // subtract null teminator
    }
    int32_t needed = ucnv_fromUChars(conv, NULL, 0, usrc, uclen, &errorCode);
    int32_t olen = needed + 1; // terminator
    char *out = ruMalloc0((rusize)olen, char);
    errorCode = U_ZERO_ERROR;
    ucnv_fromUChars(conv, out, olen, usrc, uclen, &errorCode);
    if(U_FAILURE(errorCode)) {
        ruSetError("error in ucnv_fromUChars error=%s\n", u_errorName(errorCode));
        ruFree(out);
        return NULL;
    }
    return out;
}

UChar* charToUni(const char *instr) {
    UConverter *conv = getConverter();
    UChar *upat = convToUni(conv, instr, -1);
    ucnv_close(conv);
    return upat;
}

alloc_chars uniToChar(UChar *usrc) {
    UConverter *conv = getConverter();
    alloc_chars upat = convToStr(conv, usrc, -1);
    ucnv_close(conv);
    return upat;
}

UChar* uniSwitchCase(UChar* usrc, bool isUpper) {
    UErrorCode errorCode = U_ZERO_ERROR;
    // do the transform
    int32_t slen = u_strlen(usrc);
    int32_t len = slen + 1; // terminator
    UChar* udst = ruMalloc0((rusize)len, UChar);
    int32_t needed;
    if (isUpper) {
        needed = u_strToUpper(udst, len, usrc, slen, "utf8", &errorCode);
    } else {
        needed = u_strToLower(udst, len, usrc, slen, "utf8", &errorCode);
    }
    len = needed+1; // terminator
    if (errorCode == U_BUFFER_OVERFLOW_ERROR || // too small
        errorCode == U_STRING_NOT_TERMINATED_WARNING) { // terminator missing
        // cache miss?
        ruVerbLogf("size mismatch in u_strTo(Lower|Upper) needed: %ld", needed);
        udst = ruRealloc(udst, (rusize)slen, UChar);
        errorCode = U_ZERO_ERROR;
        if (isUpper) {
            needed = u_strToUpper(udst, len, usrc, slen, "utf8", &errorCode);
        } else {
            needed = u_strToLower(udst, len, usrc, slen, "utf8", &errorCode);
        }
    }
    if(U_FAILURE(errorCode) || udst[needed] != 0) {
        ruSetError("error in u_strTo(Lower|Upper)=%ld error=%s", needed,
                   u_errorName(errorCode));
        ruFree(udst);
        return NULL;
    }
    return udst;
}

alloc_chars utf8SwitchCase(trans_chars instr, bool isUpper) {
    ruClearError();
    if (!instr) return NULL;

    UErrorCode errorCode = U_ZERO_ERROR;
    UCaseMap* ucm = ucasemap_open("utf8", 0, &errorCode);
    int32_t inlen = (int32_t)strlen(instr);
    int32_t dstlen = inlen + 1; //terminator
    char *dst = ruMalloc0((rusize)dstlen, char);
    int32_t needed;
    if (isUpper) {
        needed = ucasemap_utf8ToUpper(ucm, dst, dstlen, instr, inlen,
                                      &errorCode);
    } else {
        needed = ucasemap_utf8ToLower(ucm, dst, dstlen, instr, inlen,
                                      &errorCode);
    }
    dstlen = needed+1; // terminator
    if (errorCode == U_BUFFER_OVERFLOW_ERROR) {
        // cache miss?
        ruVerbLogf("size mismatch in ucasemap_utf8To(Upper|Lower) needed: %ld",
                needed);
        dst = ruRealloc(dst, (rusize)dstlen, char); // terminator
        errorCode = U_ZERO_ERROR;
        if (isUpper) {
            needed = ucasemap_utf8ToUpper(ucm, dst, dstlen, instr, inlen,
                                          &errorCode);
        } else {
            needed = ucasemap_utf8ToLower(ucm, dst, dstlen, instr, inlen,
                                          &errorCode);
        }
    }
    ucasemap_close(ucm);
    if(U_FAILURE(errorCode)) {
        ruSetError("error in ucasemap_utf8To(Upper|Lower) error=%s\n",
                u_errorName(errorCode));
        ruFree(dst);
        return NULL;
    }
    return dst;
}

RUAPI alloc_chars ruStrFromUtf16(trans_uni unistr) {
    return uniToChar((UChar*)unistr);
}

RUAPI alloc_chars ruStrFromNUtf16(trans_uni unistr, int32_t bytelen) {
    UConverter *conv = getConverter();
    alloc_chars upat = convToStr(conv, (UChar*)unistr, bytelen);
    ucnv_close(conv);
    return upat;
}

RUAPI alloc_uni ruStrToUtf16(trans_chars str) {
    return (alloc_uni)charToUni(str);
}

RUAPI alloc_uni ruStrNToUtf16(trans_chars str, int32_t bytelen) {
    UConverter *conv = getConverter();
    alloc_uni upat = (alloc_uni)convToUni(conv, str, bytelen);
    ucnv_close(conv);
    return upat;
}

RUAPI alloc_chars ruUtf8CaseNormalize(trans_chars  instr, int32_t normMode, int32_t caseMode) {
    ruClearError();
    if (!instr) return NULL;

    UChar *usrc = NULL, *utmp = NULL;
    char* out = NULL;
    UErrorCode errorCode = U_ZERO_ERROR;
    int32_t needed, dstlen;
    // get a converter
    UConverter *conv = getConverter();
    do {
        // make unicode
        usrc = convToUni(conv, instr, -1);
        if (!usrc) break;
        // do the transform
        const UNormalizer2 *un = NULL;
        if (normMode) {
            switch(normMode) {
                case ruUtf8Nfc:
                    un = unorm2_getNFCInstance(&errorCode);
                    break;
                case ruUtf8Nfd:
                    un = unorm2_getNFDInstance(&errorCode);
                    break;
//                case ruUtf8Nfkc:
//                    un = unorm2_getNFKCInstance(&errorCode);
//                    break;
//                case ruUtf8Nfkd:
//                    un = unorm2_getNFKDInstance(&errorCode);
//                    break;
            }
            if (!un) {
                if(U_FAILURE(errorCode)) {
                    ruSetError("error in unorm2_getInstance error=%s\n",
                             u_errorName(errorCode));
                    break;
                } else {
                    ruSetError("invalid normMode");
                    break;
                }
            }
            // get needed transform space
            needed = unorm2_normalize(un, usrc, -1, NULL, 0, &errorCode);
            dstlen = needed+1; // terminator
            if (errorCode != U_BUFFER_OVERFLOW_ERROR) {
                ruSetError("error in unorm2_normalize error=%s\n",
                         u_errorName(errorCode));
                break;
            }
            // do the transform
            utmp = ruMalloc0((rusize)dstlen, UChar); // terminator
            errorCode = U_ZERO_ERROR;
            unorm2_normalize(un, usrc, -1, utmp, dstlen, &errorCode);
            if(U_FAILURE(errorCode)) {
                ruSetError("error in unorm2_normalize error=%s\n",
                         u_errorName(errorCode));
                break;
            }
            ruFree(usrc);
            usrc = utmp;
            utmp = NULL;
        }
        if(caseMode) {
            switch(caseMode) {
                case ruUtf8Lower:
                    utmp = uniSwitchCase(usrc, false);
                    break;
                case ruUtf8Upper:
                    utmp = uniSwitchCase(usrc, true);
                    break;
            }
            if (!utmp) {
                ruSetError("casemode was invalid or failed");
                break;
            }
            ruFree(usrc);
            usrc = utmp;
            utmp = NULL;
        }
        out = convToStr(conv, usrc, -1);
    } while(0);
    // clean up
    ucnv_close(conv);
    ruFree(usrc);
    ruFree(utmp);
    return out;
}
