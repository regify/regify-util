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

ruMakeTypeGetter(Map, MagicMap)
ruMakeTypeGetter(keySpec, MagicKeySpec)
ruMakeTypeGetter(valSpec, MagicValSpec)

typedef struct {
    char *key;
    ru_uint hash;
    void *value;
    Map *mp;
} kv;

static void kvFree(void *o) {
    kv *item = (kv*)o;
    if (item->mp) {
        if (item->mp->keyFree) item->mp->keyFree(item->key);
        if (item->mp->valFree) item->mp->valFree(item->value);
    }
    memset(item, 0, sizeof(kv));
    ruFree(item);
}

RUAPI ru_uint ruInt64Hash(trans_ptr key) {
    return (ru_uint)*(int64_t*)key;
}

RUAPI bool ruInt64Match(trans_ptr testKey, trans_ptr existingKey) {
    return *(int64_t*)testKey == *(int64_t*)existingKey;
}

RUAPI ru_uint ruLongHash(trans_ptr key) {
    return (ru_uint)*(long*)key;
}

RUAPI bool ruLongMatch(trans_ptr testKey, trans_ptr existingKey) {
    return *(long*)testKey == (long)(intptr_t)existingKey;
}

RUAPI ru_uint ruInt32Hash(trans_ptr key) {
    return (ru_uint)*(int32_t*)key;
}

RUAPI bool ruInt32Match(trans_ptr testKey, trans_ptr existingKey) {
    return *(int32_t*)testKey == (int32_t)(intptr_t)existingKey;
}

RUAPI ru_uint ruInt16Hash(trans_ptr key) {
    return (ru_uint)*(int16_t*)key;
}

RUAPI bool ruInt16Match(trans_ptr testKey, trans_ptr existingKey) {
    return *(int16_t*)testKey == (int16_t)(intptr_t)existingKey;
}

RUAPI ru_uint ruInt8Hash(trans_ptr key) {
    return (ru_uint)*(int8_t*)key;
}

RUAPI bool ruInt8Match(trans_ptr testKey, trans_ptr existingKey) {
    return *(int8_t*)testKey == (int8_t)(intptr_t)existingKey;
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

keySpec int64KeySpec = {MagicKeySpec, ruInt64Hash, ruInt64Match,free,
                        (ruCloneFunc)ruInt64, ruRefPtrInt64};
keySpec longKeySpec = {MagicKeySpec, ruLongHash, ruLongMatch,NULL,
                       (ruCloneFunc)ruLongRefPtr, ruRefPtrLong};
keySpec int32KeySpec = {MagicKeySpec, ruInt32Hash, ruInt32Match,NULL,
                        (ruCloneFunc)ruInt32RefPtr, ruRefPtrInt32};
keySpec int16KeySpec = {MagicKeySpec, ruInt16Hash, ruInt16Match,NULL,
                        (ruCloneFunc)ruInt16RefPtr, ruRefPtrInt16};
keySpec int8KeySpec = {MagicKeySpec, ruInt8Hash, ruInt8Match,NULL,
                       (ruCloneFunc)ruInt8RefPtr, ruRefPtrInt8};
keySpec strRefKeySpec = {MagicKeySpec, ruStrHash, ruStrMatch,NULL,
                         NULL, NULL};
keySpec strDupKeySpec = {MagicKeySpec, ruStrHash, ruStrMatch,free,
                         (ruCloneFunc)ruStrDup, NULL};
keySpec strFreeKeySpec = {MagicKeySpec, ruStrHash, ruStrMatch,free,
                         NULL, NULL};

valSpec int64ValSpec = {MagicValSpec, free, (ruCloneFunc)ruInt64,
                        ruRefPtrInt64};
valSpec longValSpec = {MagicValSpec, NULL, (ruCloneFunc)ruLongRefPtr,
                       ruRefPtrLong};
valSpec int32ValSpec = {MagicValSpec, NULL, (ruCloneFunc)ruInt32RefPtr,
                        ruRefPtrInt32};
valSpec int16ValSpec = {MagicValSpec, NULL, (ruCloneFunc)ruInt16RefPtr,
                        ruRefPtrInt16};
valSpec int8ValSpec = {MagicValSpec, NULL, (ruCloneFunc)ruInt8RefPtr,
                       ruRefPtrInt8};
valSpec boolValSpec = {MagicValSpec, NULL, (ruCloneFunc)ruBoolRefPtr,
                       ruRefPtrBool};
valSpec strRefValSpec = {MagicValSpec, NULL, NULL, NULL};
valSpec strDupValSpec = {MagicValSpec, free, (ruCloneFunc)ruStrDup, NULL};
valSpec strFreeValSpec = {MagicValSpec, free, NULL, NULL};

RUAPI ruKeySpec ruKeySpecInt64(void) {
    return &int64KeySpec;
}

RUAPI ruKeySpec ruKeySpecLong(void) {
    return &longKeySpec;
}

RUAPI ruKeySpec ruKeySpecInt32(void) {
    return &int32KeySpec;
}

RUAPI ruKeySpec ruKeySpecInt16(void) {
    return &int16KeySpec;
}

RUAPI ruKeySpec ruKeySpecInt8(void) {
    return &int8KeySpec;
}

RUAPI ruKeySpec ruKeySpecStrRef(void) {
    return &strRefKeySpec;
}

RUAPI ruKeySpec ruKeySpecStrDup(void) {
    return &strDupKeySpec;
}

RUAPI ruKeySpec ruKeySpecStrFree(void) {
    return &strFreeKeySpec;
}

RUAPI ruValSpec ruValSpecInt64(void) {
    return &int64ValSpec;
}

RUAPI ruValSpec ruValSpecLong(void) {
    return &longValSpec;
}

RUAPI ruValSpec ruValSpecInt32(void) {
    return &int32ValSpec;
}

RUAPI ruValSpec ruValSpecInt16(void) {
    return &int16ValSpec;
}

RUAPI ruValSpec ruValSpecInt8(void) {
    return &int8ValSpec;
}

RUAPI ruValSpec ruValSpecBool(void) {
    return &boolValSpec;
}

RUAPI ruValSpec ruValSpecStrRef(void) {
    return &strRefValSpec;
}

RUAPI ruValSpec ruValSpecStrDup(void) {
    return &strDupValSpec;
}

RUAPI ruValSpec ruValSpecStrFree(void) {
    return &strFreeValSpec;
}

RUAPI ruKeySpec ruKeySpecNew(ruHashFunc hash, ruMatchFunc match, ruFreeFunc keyFree,
                             ruCloneFunc in, ruPtr2TypeFunc out) {
    if (!hash || !match) return NULL;
    keySpec* s = ruMalloc0(1, keySpec);
    s->type = MagicKeySpec;
    s->hash = hash;
    s->match = match;
    s->keyFree = keyFree;
    s->in = in;
    s->out = out;
    return s;
}

RUAPI ruValSpec ruValSpecNew(ruFreeFunc valFree,
                             ruCloneFunc clone, ruPtr2TypeFunc out) {
    valSpec* s = ruMalloc0(1, valSpec);
    s->type = MagicValSpec;
    s->valFree = valFree;
    s->in = clone;
    s->out = out;
    return s;
}

RUAPI ruMap ruMapNewSpec(ruKeySpec KS, ruValSpec VS) {
    ruClearError();
    keySpec* ks = keySpecGet(KS, NULL);
    valSpec* vs = valSpecGet(VS, NULL);
    if (!ks || !ks->hash || !ks->match) {
        ruCritLog("Failed map creation due an invalid keySpec parameter");
        return NULL;
    }
    if (VS && !vs) {
        ruCritLog("Failed map creation due an invalid valSpec parameter");
        ruAbort();
    }

    Map *mp = ruMalloc0(1, Map);
    mp->type = MagicMap;

    // default start value?
    uint32_t expectedSize = 16;

    /* Allocate space for the hash table. */
    mp->table = ruMalloc0(expectedSize, List*);

    /* Initialize the buckets. */
    mp->buckets = expectedSize;
    for (uint32_t i = 0; i < mp->buckets; i++) {
        mp->table[i] = ListNew(kvFree);
    }

    /* Encapsulate the functions. */
    mp->hash = ks->hash;
    mp->match = ks->match;
    mp->keyFree = ks->keyFree;
    mp->keyIn = ks->in;
    mp->keyOut = ks->out;
    if (vs) {
        mp->valFree = vs->valFree;
        mp->valIn = vs->in;
        mp->valOut = vs->out;
    }
    /* Initialize the number of elements in the table. */
    mp->size = 0;
    mp->mux = ruMutexInit();
    return (ruMap)mp;
}

RUAPI ruMap ruMapNewString(ruFreeFunc keyFree, ruFreeFunc valFree) {
    keySpec ks = {MagicKeySpec, ruStrHash, ruStrMatch, keyFree, NULL};
    valSpec vs = {MagicValSpec, valFree, NULL};
    return ruMapNewSpec(&ks, &vs);
}

RUAPI ruMap ruMapNew(ruHashFunc hash, ruMatchFunc match,
                     ruFreeFunc keyFree, ruFreeFunc valFree, uint32_t expectedSize) {
    keySpec ks = {MagicKeySpec, hash, match, keyFree, NULL};
    valSpec vs = {MagicValSpec, valFree, NULL};
    return ruMapNewSpec(&ks, &vs);
}

RUAPI ruMap ruMapFree(ruMap rm) {
    ruClearError();
    uint32_t i;
    Map *mp = MapGet(rm, NULL);
    if(!mp) {
        return NULL;
    }
    mp->doQuit = true;
    ruMutexLock(mp->mux);
    ruMutexUnlock(mp->mux);

    /* Destroy each bucket. */
    for (i = 0; i < mp->buckets; i++) {
        ListFree(mp->table[i]);
    }
    /* Free the storage allocated for the hash table. */
    ruFree(mp->table);
    if (mp->mux) mp->mux = ruMutexFree(mp->mux);
    /* No operations are allowed now, but clear the structure as a precaution. */
    memset(mp, 0, sizeof(Map));
    ruFree(mp);
    return NULL;
}

static void remap(Map *mp) {
    uint32_t oldSize = mp->buckets;
    List** oldTable = mp->table;

    // new buckets
    uint32_t newSize = mp->buckets * 2;
    mp->table = ruMalloc0(newSize, List*);
    for (uint32_t i = 0; i < newSize; i++) {
        mp->table[i] = ListNew(kvFree);
    }

    // migrate old entries to new buckets
    for (uint32_t i = 0; i < oldSize; i++) {
        List *list = oldTable[i];
        while (list->size > 0) {
            kv* item = ListRemove(list, list->head->next, NULL);
            ru_uint bucket = item->hash % newSize;
            ListInsertAfter(mp->table[bucket], NULL, item);
        }
    }
    mp->buckets = newSize;

    // clean up old buckets
    for (uint32_t i = 0; i < oldSize; i++) {
        ListFree(oldTable[i]);
    }
    ruFree(oldTable);
}

static inline void runKeyIn(Map* mp, kv* item, ptr key, ru_uint hash) {
    if (mp->keyIn) {
        item->key = mp->keyIn(key);
    } else {
        item->key = key;
    }
    item->hash = hash;
}

static inline void runValIn(Map* mp, kv* item, ptr val) {
    if (mp->valIn) {
        item->value = mp->valIn(val);
    } else {
        item->value = val;
    }
}

static int32_t runValOut(Map *mp, ptr data, ptr* target) {
    if (mp->valOut) {
        return mp->valOut(data, target);
    } else {
        *target = data;
    }
    return RUE_OK;

}

static int32_t runKeyOut(Map *mp, ptr data, ptr* target) {
    if (mp->keyOut) {
        return mp->keyOut(data, target);
    } else {
        *target = data;
    }
    return RUE_OK;

}

static int32_t MapPut(Map* mp, ptr key, ptr val, ptr* existingVal) {
    /* Hash the key. */
    mp->iterActive = false;
    ru_uint hash = mp->hash(key);
    ru_uint bucket = hash % mp->buckets;
    for (ListElmt *le = mp->table[bucket]->head->next; le != NULL; le = le->next) {
        kv* item = le->data;
        if (mp->match(key, item->key)) {
            if (existingVal) {
                *existingVal = item->value;
                return RUE_FILE_EXISTS;
            }
            // update the exisiting item
            if (mp->keyFree) mp->keyFree(item->key);
            if (mp->valFree) mp->valFree(item->value);
            runKeyIn(mp, item, key, hash);
            runValIn(mp, item, val);
            return RUE_OK;
        }
    }
    kv* item = ruMalloc0(1, kv);
    runKeyIn(mp, item, key, hash);
    runValIn(mp, item, val);
    item->mp = mp;
    /* Insert the data into the bucket. */
    int32_t ret = ListInsertAfter(mp->table[bucket], NULL, item);
    if (ret == RUE_OK) {
        mp->size++;
        uint32_t loadFactor = mp->size / mp->buckets;
        if (loadFactor > 4) remap(mp);
    } else {
        kvFree(item);
    }
    return ret;
}

RUAPI int32_t ruMapPutData(ruMap rm, ptr key, ptr val, ptr* exisitingVal) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) {
        return ret;
    }
    if (!key) return RUE_PARAMETER_NOT_SET;

    ret = RUE_USER_ABORT;
    if (mp->doQuit) return ret;
    ruMutexLock(mp->mux);
    if (!mp->doQuit) {
        ret = MapPut(mp, key, val,exisitingVal);
    }
    ruMutexUnlock(mp->mux);
    return ret;
}

static int32_t MapRemove(Map *mp, ptr key, void **val) {
    /* Hash the key. */
    int32_t bucket = mp->hash(key) % mp->buckets;
    mp->iterActive = false;
    /* Search for the data in the bucket. */
    for (ListElmt *le = mp->table[bucket]->head->next; le != NULL; le = le->next) {
        kv* item = le->data;
        if (mp->match(key, item->key)) {
            /* Remove the data from the bucket.*/
            int32_t ret;
            if (val) {
                ret = runValOut(mp, item->value, val);
                item->value = NULL;
            }
            ListRemove(mp->table[bucket], le, &ret);
            if (ret == RUE_OK) {
                mp->size--;
                kvFree(item);
            }
            return ret;
        }
    }
    /* Return that the data was not found. */
    return RUE_GENERAL;
}

RUAPI int32_t ruMapRemoveData(ruMap rm, ptr key, void **val) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) {
        return ret;
    }
    if (!key) return RUE_PARAMETER_NOT_SET;

    // thread safe version
    ret = RUE_USER_ABORT;
    if (!mp->doQuit) {
        ruMutexLock(mp->mux);
        if (!mp->doQuit) {
            ret = MapRemove(mp, key, val);
        }
        ruMutexUnlock(mp->mux);
    }
    return ret;
}

static int32_t MapGetData(Map *mp, ptr key, void **value) {
    /* Hash the key. */
    int32_t bucket = mp->hash(key) % mp->buckets;
    /* Search for the data in the bucket. */
    for (ListElmt *le = mp->table[bucket]->head->next; le != NULL; le = le->next) {
        kv* item = le->data;
        if (mp->match(key, item->key)) {
            /* Pass back the data from the table. */
            return runValOut(mp, item->value, value);
        }
    }
    /* Return that the data was not found. */
    return RUE_GENERAL;
}

RUAPI bool ruMapHasKey(ruMap rm, ptr key, int32_t *code) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) ruRetWithCode(code, ret, false);
    if (!key) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, false);

    ret = RUE_USER_ABORT;
    if (!mp->doQuit) {
        ruMutexLock(mp->mux);
        if (!mp->doQuit) {
            void *val;
            ret = MapGetData(mp, key, &val);
        }
        ruMutexUnlock(mp->mux);
    }

    if (ret == RUE_OK) {
        ruRetWithCode(code, RUE_OK, true);
    }
    ruRetWithCode(code, RUE_OK, false);
}

RUAPI int32_t ruMapGetValue(ruMap rm, ptr key, ptr* value) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) {
        return ret;
    }
    if (!key || !value) return RUE_PARAMETER_NOT_SET;

    ret = RUE_USER_ABORT;
    if (mp->doQuit) return ret;
    ruMutexLock(mp->mux);
    if (!mp->doQuit) {
        ret = MapGetData(mp, key, value);
    }
    ruMutexUnlock(mp->mux);
    return ret;
}

static int32_t MapNextSet(Map* mp, void** key, void** value) {
    if (!mp->iterActive) return RUE_INVALID_STATE;
    if (!mp->iterElmt) {
        do {
            if (mp->iterBucket >= mp->buckets) break;
            mp->iterElmt = mp->table[mp->iterBucket]->head->next;
            mp->iterBucket++;
        } while(!mp->iterElmt);
    }
    if (!mp->iterElmt) {
        if (key) {
            runKeyOut(mp, NULL, key);
        }
        if (value) {
            runValOut(mp, NULL, value);
        }
        return RUE_FILE_NOT_FOUND;
    }
    kv* item = mp->iterElmt->data;
    if (key) {
        runKeyOut(mp, item->key, key);
    }
    if (value) {
        runValOut(mp, item->value, value);
    }
    mp->iterElmt = mp->iterElmt->next;
    return RUE_OK;
}

RUAPI int32_t ruMapFirstSet(ruMap rm, void** key, void** value) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) {
        return ret;
    }
    if (!key && !value) return RUE_PARAMETER_NOT_SET;

    if (mp->doQuit) return RUE_USER_ABORT;
    ruMutexLock(mp->mux);
    if (mp->doQuit) {
        ruMutexUnlock(mp->mux);
        return RUE_USER_ABORT;
    }
    mp->iterBucket = 0;
    mp->iterElmt = NULL;
    mp->iterActive = true;
    ret = MapNextSet(mp, key, value);
    ruMutexUnlock(mp->mux);
    return ret;
}

RUAPI int32_t ruMapNextSet(ruMap rm, void** key, void** value) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) {
        return ret;
    }
    if (!key && !value) return RUE_PARAMETER_NOT_SET;
    ret = RUE_USER_ABORT;
    if (mp->doQuit) return ret;
    ruMutexLock(mp->mux);
    if (!mp->doQuit) {
        ret = MapNextSet(mp, key, value);
    }
    ruMutexUnlock(mp->mux);
    return ret;
}

RUAPI int32_t ruMapKeyList(ruMap rm, ruList* keys,
                           ruCloneFunc copy, ruFreeFunc listFree) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) {
        return ret;
    }
    if (!keys) return RUE_PARAMETER_NOT_SET;

    ret = RUE_USER_ABORT;
    if (mp->doQuit) return ret;
    ruList set = NULL;
    ruMutexLock(mp->mux);
    if (!mp->doQuit) {

        if (!listFree) listFree = mp->keyFree;
        set = ruListNew(listFree);
        mp->iterBucket = 0;
        mp->iterElmt = NULL;
        mp->iterActive = true;
        void* key = NULL;
        for (ret = MapNextSet(rm, &key, NULL); ret == RUE_OK;
             ret = MapNextSet(rm, &key, NULL)) {
            if (copy) {
                ret = ruListAppend(set, copy(key));
            } else {
                ret = ruListAppend(set, key);
            }
        }

    }
    ruMutexUnlock(mp->mux);

    if (ret == RUE_FILE_NOT_FOUND) ret = RUE_OK;
    if (ret == RUE_OK) {
        *keys = set;
    } else {
        set = ruListFree(set);
    }
    return ret;
}

RUAPI int32_t ruMapRemoveAll(ruMap rm) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) return ret;

    if (mp->doQuit) return RUE_USER_ABORT;
    ruMutexLock(mp->mux);
    if (mp->doQuit) {
        ruMutexUnlock(mp->mux);
        return RUE_USER_ABORT;
    }

    for (uint32_t i = 0; i < mp->buckets; i++) {
        while (ruListSize(mp->table[i], NULL) > 0) {
            kv* item = ListRemove(mp->table[i], mp->table[i]->head->next, &ret);
            if (ret == RUE_OK) {
                mp->size--;
                kvFree(item);
            }
        }
    }
    ruMutexUnlock(mp->mux);
    return RUE_OK;
}

RUAPI uint32_t ruMapSize(ruMap rm, int32_t *code) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) {
        ruRetWithCode(code, ret, 0);
    }

    uint32_t sz = 0;
    ret = RUE_USER_ABORT;
    if (!mp->doQuit) {
        ruMutexLock(mp->mux);
        if (!mp->doQuit) {
            sz = mp->size;
            ret = RUE_OK;
        }
        ruMutexUnlock(mp->mux);
    }
    ruRetWithCode(code, ret, sz);
}

// Set API
RUAPI ruMap ruSetNewSpec(ruKeySpec ks) {
    return ruMapNewSpec(ks, NULL);
}

RUAPI ruSet ruSetNewString(ruFreeFunc itemFree) {
    keySpec ks = {MagicKeySpec, ruStrHash, ruStrMatch, itemFree, NULL};
    return ruSetNewSpec(&ks);
}

RUAPI ruSet ruSetNew(ruHashFunc hash, ruMatchFunc match,
                     ruFreeFunc itemFree, uint32_t expectedSize) {
    keySpec ks = {MagicKeySpec, hash, match, itemFree, NULL};
    return ruSetNewSpec(&ks);
}

RUAPI ruSet ruSetFree(ruSet rs) {
    return ruMapFree(rs);
}

RUAPI int32_t ruSetPutItem(ruSet rs, ptr item) {
    return ruMapPutData(rs, item, NULL, NULL);
}

RUAPI int32_t ruSetRemoveItem(ruMap rs, void *item) {
    return ruMapRemoveData(rs, item, NULL);
}

RUAPI bool ruSetHasItem(ruSet rs, void *item, int32_t *code) {
    return ruMapHasKey(rs, item, code);
}

RUAPI int32_t ruSetFirstSet(ruSet rs, void** item) {
    return ruMapFirstSet(rs, item, NULL);
}

RUAPI int32_t ruSetNextSet(ruSet rs, void** item) {
    return ruMapNextSet(rs, item, NULL);
}

RUAPI int32_t ruSetItemList(ruSet rs, ruList* items,
                           ruCloneFunc copy, ruFreeFunc listFree) {
    return ruMapKeyList(rs, items, copy, listFree);
}

RUAPI int32_t ruSetRemoveAll(ruSet rs) {
    return ruMapRemoveAll(rs);
}

RUAPI uint32_t ruSetSize(ruSet rs, int32_t *code) {
    return ruMapSize(rs, code);
}

