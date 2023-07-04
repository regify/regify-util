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
 *
 * The code below is based on the code from the book "Mastering Algorithms
 * with C" by Kyle Loudon, published by O'Reilly & Associates.
 */
#include "lib.h"

ruMakeTypeGetter(typeSpec, MagicTypeSpec)

RUAPI ru_uint ruInt64Hash(trans_ptr key) {
    return (ru_uint)*(int64_t*)key;
}

RUAPI bool ruInt64Match(trans_ptr testKey, trans_ptr existingKey) {
    return *(int64_t*)testKey == *(int64_t*)existingKey;
}

RUAPI int32_t ruInt64Comp(trans_ptr testVal, trans_ptr existingVal) {
    int64_t testInt = *(int64_t*)testVal;
    int64_t existingInt = *(int64_t*)existingVal;
    if (testInt == existingInt) return 0;
    return testInt > existingInt? 1 : -1;
}

RUAPI ru_Int64 ruInt64(ptr val) {
    int64_t* i = ruMalloc0(1, int64_t);
    *i = *(int64_t*)val;
    return i;
}

RUAPI int32_t ruRefPtrInt64(ptr src, ptr* dst) {
    if (!dst) return RUE_PARAMETER_NOT_SET;
    int64_t num = 0;
    if (src) num = *(int64_t*)(intptr_t*)src;
    int64_t* var = (int64_t*)(intptr_t*)dst;
    *var = num;
    return RUE_OK;
}


RUAPI ru_uint ruLongHash(trans_ptr key) {
    return (ru_uint)*(long*)key;
}

RUAPI bool ruLongMatch(trans_ptr testKey, trans_ptr existingKey) {
    return *(long*)testKey == (long)(intptr_t)existingKey;
}

RUAPI int32_t ruLongComp(trans_ptr testVal, trans_ptr existingVal) {
    long testInt = (long)(intptr_t)testVal;
    long existingInt = (long)(intptr_t)existingVal;
    if (testInt == existingInt) return 0;
    return testInt > existingInt? 1 : -1;
}

RUAPI ptr ruLongRefPtr(ptr in) {
    intptr_t i = (intptr_t)*(long*)in;
    return (ptr)i;
}

RUAPI int32_t ruRefPtrLong(ptr src, ptr* dst) {
    if (!dst) return RUE_PARAMETER_NOT_SET;
    long num = (long)(intptr_t)src;
    long* var = (long*)(intptr_t*)dst;
    *var = num;
    return RUE_OK;
}


RUAPI ru_uint ruInt32Hash(trans_ptr key) {
    return (ru_uint)*(int32_t*)key;
}

RUAPI bool ruInt32Match(trans_ptr testKey, trans_ptr existingKey) {
    return *(int32_t*)testKey == (int32_t)(intptr_t)existingKey;
}

RUAPI int32_t ruInt32Comp(trans_ptr testVal, trans_ptr existingVal) {
    int32_t testInt = (int32_t)(intptr_t)testVal;
    int32_t existingInt = (int32_t)(intptr_t)existingVal;
    if (testInt == existingInt) return 0;
    return testInt > existingInt? 1 : -1;
}

RUAPI ptr ruInt32RefPtr(ptr in) {
    intptr_t i = (intptr_t)*(int32_t*)in;
    return (ptr)i;
}

RUAPI int32_t ruRefPtrInt32(ptr src, ptr* dst) {
    if (!dst) return RUE_PARAMETER_NOT_SET;
    int32_t num = (int32_t)(intptr_t)src;
    int32_t* var = (int32_t*)(intptr_t*)dst;
    *var = num;
    return RUE_OK;
}


RUAPI ru_uint ruInt16Hash(trans_ptr key) {
    return (ru_uint)*(int16_t*)key;
}

RUAPI bool ruInt16Match(trans_ptr testKey, trans_ptr existingKey) {
    return *(int16_t*)testKey == (int16_t)(intptr_t)existingKey;
}

RUAPI int32_t ruInt16Comp(trans_ptr testVal, trans_ptr existingVal) {
    int16_t testInt = (int16_t)(intptr_t)testVal;
    int16_t existingInt = (int16_t)(intptr_t)existingVal;
    if (testInt == existingInt) return 0;
    return testInt > existingInt? 1 : -1;
}

RUAPI ptr ruInt16RefPtr(ptr in) {
    intptr_t i = (intptr_t)*(int16_t*)in;
    return (ptr)i;
}

RUAPI int32_t ruRefPtrInt16(ptr src, ptr* dst) {
    if (!dst) return RUE_PARAMETER_NOT_SET;
    int16_t num = (int16_t)(intptr_t)src;
    int16_t* var = (int16_t*)(intptr_t*)dst;
    *var = num;
    return RUE_OK;
}


RUAPI ru_uint ruInt8Hash(trans_ptr key) {
    return (ru_uint)*(int8_t*)key;
}

RUAPI bool ruInt8Match(trans_ptr testKey, trans_ptr existingKey) {
    return *(int8_t*)testKey == (int8_t)(intptr_t)existingKey;
}

RUAPI int32_t ruInt8Comp(trans_ptr testVal, trans_ptr existingVal) {
    int8_t testInt = (int8_t)(intptr_t)testVal;
    int8_t existingInt = (int8_t)(intptr_t)existingVal;
    if (testInt == existingInt) return 0;
    return testInt > existingInt? 1 : -1;
}

RUAPI ptr ruInt8RefPtr(ptr in) {
    intptr_t i = (intptr_t)*(int8_t*)in;
    return (ptr)i;
}

RUAPI int32_t ruRefPtrInt8(ptr src, ptr* dst) {
    if (!dst) return RUE_PARAMETER_NOT_SET;
    int8_t num = (int8_t)(intptr_t)src;
    int8_t* var = (int8_t*)(intptr_t*)dst;
    *var = num;
    return RUE_OK;
}


RUAPI ru_uint ruBoolHash(trans_ptr key) {
    return *(bool*)key? 1 : 0;
}

RUAPI bool ruBoolMatch(trans_ptr testKey, trans_ptr existingKey) {
    bool test = *(bool*)testKey != false;
    bool other = (bool)(intptr_t)existingKey != false;
    return test == other;
}

RUAPI int32_t ruBoolComp(trans_ptr testVal, trans_ptr existingVal) {
    bool testBool = (bool)(intptr_t)testVal != 0;
    bool existingBool = (bool)(intptr_t)existingVal != 0;
    if (testBool == existingBool) return 0;
    if (testBool) return 1;
    return -1;
}

RUAPI ptr ruBoolRefPtr(ptr in) {
    intptr_t i = (intptr_t)*(bool*)in;
    return (ptr)i;
}

RUAPI int32_t ruRefPtrBool(ptr src, ptr* dst) {
    if (!dst) return RUE_PARAMETER_NOT_SET;
    bool num = (bool)(intptr_t)src;
    bool* var = (bool*)(intptr_t*)dst;
    *var = num;
    return RUE_OK;
}


RUAPI ru_uint ruStrHash(trans_ptr key) {
    trans_bytes ptr = key;
    uint32_t val = 0;
    /* Hash the key by performing a number of bit operations on it. */
    while (*ptr != '\0') {
        val = (val << 4) + (*ptr);
        uint32_t tmp = (val & 0xf0000000);
        if (tmp) {
            val = val ^ (tmp >> 24);
            val = val ^ tmp;
        }
        ptr++;
    }
    return (ru_uint)val;
}

RUAPI bool ruStrMatch(trans_ptr testKey, trans_ptr existingKey) {
    return ruStrEquals((trans_chars)testKey, (trans_chars)existingKey);
}

RUAPI int32_t ruStrComp(trans_ptr testVal, trans_ptr existingVal) {
    return ruStrCmp((trans_chars)testVal, (trans_chars)existingVal);
}


typeSpec int64Spec = {MagicTypeSpec, false,
                      ruInt64Hash, ruInt64Match, ruInt64Comp,
                      ruClear, ruInt64,ruRefPtrInt64};
typeSpec longSpec = {MagicTypeSpec, false,
                     ruLongHash, ruLongMatch,ruLongComp,
                     NULL, ruLongRefPtr,ruRefPtrLong};
typeSpec int32Spec = {MagicTypeSpec, false,
                      ruInt32Hash, ruInt32Match,ruInt32Comp,
                      NULL, ruInt32RefPtr,ruRefPtrInt32};
typeSpec int16Spec = {MagicTypeSpec, false,
                      ruInt16Hash, ruInt16Match,ruInt16Comp,
                      NULL, ruInt16RefPtr,ruRefPtrInt16};
typeSpec int8Spec = {MagicTypeSpec, false,
                     ruInt8Hash, ruInt8Match,ruInt8Comp,
                     NULL, ruInt8RefPtr,ruRefPtrInt8};
typeSpec boolSpec = {MagicTypeSpec, false,
                     ruBoolHash, ruBoolMatch,ruBoolComp,
                     NULL, ruBoolRefPtr,ruRefPtrBool};

typeSpec strRefSpec = {MagicTypeSpec, false,
                       ruStrHash, ruStrMatch,ruStrComp,
                       NULL, NULL,NULL};
typeSpec strDupSpec = {MagicTypeSpec, false,
                       ruStrHash, ruStrMatch,ruStrComp,
                       ruClear, (ruCloneFunc)ruStrDup,NULL};
typeSpec strFreeSpec = {MagicTypeSpec, false,
                        ruStrHash, ruStrMatch,ruStrComp,
                        ruClear, NULL,NULL};

typeSpec ptrFreeSpec = {MagicTypeSpec, false,
                        NULL, NULL, NULL,
                        ruClear, NULL, NULL};


RUAPI ruType ruTypeInt64(void) {
    return &int64Spec;
}

RUAPI ruType ruTypeLong(void) {
    return &longSpec;
}

RUAPI ruType ruTypeInt32(void) {
    return &int32Spec;
}

RUAPI ruType ruTypeInt16(void) {
    return &int16Spec;
}

RUAPI ruType ruTypeInt8(void) {
    return &int8Spec;
}

RUAPI ruType ruTypeBool(void) {
    return &boolSpec;
}

RUAPI ruType ruTypeStrRef(void) {
    return &strRefSpec;
}

RUAPI ruType ruTypeStrDup(void) {
    return &strDupSpec;
}

RUAPI ruType ruTypeStrFree(void) {
    return &strFreeSpec;
}

RUAPI ruType ruTypePtrFree(void) {
    return &ptrFreeSpec;
}

RUAPI ruType ruTypeNew(ruHashFunc hash, ruMatchFunc match, ruCompFunc comp,
                       ruClearFunc free, ruCloneFunc in, ruPtr2TypeFunc out) {
    typeSpec* s = ruMalloc0(1, typeSpec);
    s->type = MagicTypeSpec;
    s->heaped = true;
    s->hash = hash;
    s->match = match;
    s->comp = comp;
    s->free = free;
    s->in = in;
    s->out = out;
    return s;
}

RUAPI ruType ruTypePtr(ruClearFunc free) {
    return ruTypeNew(NULL, NULL, NULL,
                     free, NULL, NULL);
}

RUAPI ruType ruTypeClone(ruType tp) {
    typeSpec* ts = typeSpecGet(tp, NULL);
    if (!ts) return NULL;
    if (!ts->heaped) return ts;
    return ruTypeNew(ts->hash, ts->match, ts->comp, ts->free, ts->in, ts->out);
}

RUAPI ruType ruTypeFree(ruType tp) {
    typeSpec* ts = typeSpecGet(tp, NULL);
    if (!ts || !ts->heaped) return NULL;
    free(ts);
    return NULL;
}
