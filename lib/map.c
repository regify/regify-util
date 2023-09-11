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

typedef struct {
    char *key;
    ru_uint hash;
    ptr value;
    Map *mp;
} kv;

static ptr kvFree(ptr o) {
    kv *item = (kv*)o;
    if (item->mp) {
        if (item->mp->keyFree && item->key) item->mp->keyFree(item->key);
        if (item->mp->valFree && item->value) item->mp->valFree(item->value);
    }
    memset(item, 0, sizeof(kv));
    return ruClear(item);
}

RUAPI ruMap ruMapNew(ruType keyType, ruType valueType) {
    typeSpec* ks = typeSpecGet(keyType, NULL);
    typeSpec* vs = typeSpecGet(valueType, NULL);
    if (!ks || !ks->hash || !ks->match) {
        ruCritLog("Failed map creation due an invalid keySpec parameter");
        ks = ruTypeFree(ks);
        vs = ruTypeFree(vs);
        return NULL;
    }
    if (valueType && !vs) {
        ruAbortm("Failed map creation due an invalid valSpec parameter");
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
        mp->table[i] = ListNewType(ruTypePtr(kvFree));
    }

    /* Set the type specs. */
    mp->keySpec = ks;
    mp->valSpec = vs;
    mp->keyFree = ks->free;
    mp->keyIn = ks->in;
    mp->keyOut = ks->out;
    if (vs) {
        mp->valFree = vs->free;
        mp->valIn = vs->in;
        mp->valOut = vs->out;
    }
    /* Initialize the number of elements in the table. */
    mp->size = 0;
    mp->mux = ruMutexInit();
    return (ruMap)mp;
}

RUAPI ruMap ruMapFree(ruMap rm) {
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
    ruTypeFree(mp->keySpec);
    ruTypeFree(mp->valSpec);
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
        mp->table[i] = ListNewType(ruTypePtr(kvFree));
    }

    // migrate old entries to new buckets
    for (uint32_t i = 0; i < oldSize; i++) {
        List *list = oldTable[i];
        while (list->size > 0) {
            kv* item = NULL;
            ListRemoveTo(list, list->head->next, (ptr*)&item);
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

static int32_t runValOut(Map *mp, kv* item, ptr* target, bool moving) {
    ptr data = NULL;
    if (item) data = item->value;
    if (mp->valOut) {
        return mp->valOut(data, target);
    } else {
        *target = data;
        if (moving && item) item->value = NULL;
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
    ru_uint hash = mp->keySpec->hash(key);
    ru_uint bucket = hash % mp->buckets;
    for (ListElmt *le = mp->table[bucket]->head->next; le != NULL; le = le->next) {
        kv* item = le->data;
        if (mp->keySpec->match(key, item->key)) {
            if (existingVal) {
                *existingVal = item->value;
                return RUE_FILE_EXISTS;
            }
            // update the exisiting item
            if (mp->keyFree && item->key) mp->keyFree(item->key);
            if (mp->valFree && item->value) mp->valFree(item->value);
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

static int32_t MapRemove(Map *mp, trans_ptr key, ptr* val) {
    /* Hash the key. */
    int32_t bucket = mp->keySpec->hash(key) % mp->buckets;
    mp->iterActive = false;
    /* Search for the data in the bucket. */
    for (ListElmt *le = mp->table[bucket]->head->next; le != NULL; le = le->next) {
        kv* item = le->data;
        if (mp->keySpec->match(key, item->key)) {
            /* Remove the data from the bucket.*/
            int32_t ret = RUE_OK;
            if (val) {
                ret = runValOut(mp, item, val, true);
            }
            if (RUE_OK == ListRemoveTo(mp->table[bucket], le, NULL)) {
                mp->size--;
            }
            return ret;
        }
    }
    /* Return that the data was not found. */
    return RUE_GENERAL;
}

RUAPI int32_t ruMapRemoveData(ruMap rm, trans_ptr key, ptr* val) {
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

static int32_t MapGetData(Map *mp, trans_ptr key, ptr* value) {
    /* Hash the key. */
    int32_t bucket = mp->keySpec->hash(key) % mp->buckets;
    /* Search for the data in the bucket. */
    for (ListElmt *le = mp->table[bucket]->head->next; le != NULL; le = le->next) {
        kv* item = le->data;
        if (mp->keySpec->match(key, item->key)) {
            /* Pass back the data from the table. */
            return runValOut(mp, item, value, false);
        }
    }
    /* Return that the data was not found. */
    return RUE_GENERAL;
}

RUAPI bool ruMapHasKey(ruMap rm, trans_ptr key, int32_t *code) {
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) ruRetWithCode(code, ret, false);
    if (!key) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, false);

    ret = RUE_USER_ABORT;
    if (!mp->doQuit) {
        ruMutexLock(mp->mux);
        if (!mp->doQuit) {
            ptr val;
            ret = MapGetData(mp, key, &val);
        }
        ruMutexUnlock(mp->mux);
    }

    if (ret == RUE_OK) {
        ruRetWithCode(code, RUE_OK, true);
    }
    ruRetWithCode(code, RUE_OK, false);
}

RUAPI int32_t ruMapGetValue(ruMap rm, trans_ptr key, ptr* value) {
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

static int32_t MapNextSet(Map* mp, ptr* key, ptr* value) {
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
            runValOut(mp, NULL, value, false);
        }
        return RUE_FILE_NOT_FOUND;
    }
    kv* item = mp->iterElmt->data;
    if (key) {
        runKeyOut(mp, item->key, key);
    }
    if (value) {
        runValOut(mp, item, value, false);
    }
    mp->iterElmt = mp->iterElmt->next;
    return RUE_OK;
}

RUAPI int32_t ruMapFirstSet(ruMap rm, ptr* key, ptr* value) {
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

RUAPI int32_t ruMapNextSet(ruMap rm, ptr* key, ptr* value) {
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

RUAPI int32_t ruMapKeyList(ruMap rm, ruList* keys) {
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

        set = ruListNew(ruTypeClone(mp->keySpec));
        mp->iterBucket = 0;
        mp->iterElmt = NULL;
        mp->iterActive = true;
        ptr key = NULL;
        for (ret = MapNextSet(rm, &key, NULL); ret == RUE_OK;
             ret = MapNextSet(rm, &key, NULL)) {
            ruListAppend(set, key);
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
            ret = ListRemoveTo(mp->table[i],
                               mp->table[i]->head->next, NULL);
            if (ret == RUE_OK) {
                mp->size--;
            }
        }
    }
    ruMutexUnlock(mp->mux);
    return RUE_OK;
}

RUAPI uint32_t ruMapSize(ruMap rm, int32_t *code) {
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
RUAPI ruMap ruSetNew(ruType keyType) {
    return ruMapNew(keyType, NULL);
}

RUAPI ruSet ruSetFree(ruSet rs) {
    return ruMapFree(rs);
}

RUAPI int32_t ruSetPutItem(ruSet rs, ptr item) {
    return ruMapPutData(rs, item, NULL, NULL);
}

RUAPI int32_t ruSetRemoveItem(ruMap rs, ptr item) {
    return ruMapRemoveData(rs, item, NULL);
}

RUAPI bool ruSetHasItem(ruSet rs, trans_ptr item, int32_t *code) {
    return ruMapHasKey(rs, item, code);
}

RUAPI int32_t ruSetFirstSet(ruSet rs, ptr* item) {
    return ruMapFirstSet(rs, item, NULL);
}

RUAPI int32_t ruSetNextSet(ruSet rs, ptr* item) {
    return ruMapNextSet(rs, item, NULL);
}

RUAPI int32_t ruSetItemList(ruSet rs, ruList* items) {
    return ruMapKeyList(rs, items);
}

RUAPI int32_t ruSetRemoveAll(ruSet rs) {
    return ruMapRemoveAll(rs);
}

RUAPI uint32_t ruSetSize(ruSet rs, int32_t *code) {
    return ruMapSize(rs, code);
}

