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

RUAPI ru_uint ruIntHash(trans_ptr key) {
    return (ru_uint)(intptr_t)key;
}

RUAPI bool ruIntMatch(trans_ptr s1, trans_ptr s2) {
    return s1 == s2;
}

RUAPI ru_uint ruStrHash(trans_ptr key) {
    trans_chars ptr = key;
    u_int32_t val = 0;
    /* Hash the key by performing a number of bit operations on it. */
    while (*ptr != '\0') {
        val = (val << 4) + (*ptr);
        u_int32_t tmp = (val & 0xf0000000);
        if (tmp) {
            val = val ^ (tmp >> 24);
            val = val ^ tmp;
        }
        ptr++;
    }
    return (ru_uint)val;
}

RUAPI bool ruStrMatch(trans_ptr s1, trans_ptr s2) {
    return ruStrEquals((trans_chars)s1, (trans_chars)s2);
}

RUAPI ruMap ruMapNewString(ruFreeFunc keyFree, ruFreeFunc valFree) {
    return ruMapNew(ruStrHash, ruStrMatch, keyFree, valFree, 0);
}

RUAPI ruMap ruMapNew(ruHashFunc hash, ruMatchFunc match,
                     ruFreeFunc keyFree, ruFreeFunc valFree,
                     u_int32_t expectedSize) {
    ruClearError();
    if (!hash || !match) return NULL;

    Map *mp = ruMalloc0(1, Map);
    mp->type = MagicMap;

    // default start value?
    if (!expectedSize) expectedSize = 10;

    /* Allocate space for the hash table. */
    mp->table = ruMalloc0(expectedSize, List*);

    /* Initialize the buckets. */
    mp->buckets = expectedSize;
    for (u_int32_t i = 0; i < mp->buckets; i++) {
        mp->table[i] = ListNew(kvFree);
    }

    /* Encapsulate the functions. */
    mp->hash = hash;
    mp->match = match;
    mp->keyFree = keyFree;
    mp->valFree = valFree;
    /* Initialize the number of elements in the table. */
    mp->size = 0;
    mp->mux = ruMutexInit();
    return (ruMap)mp;
}

RUAPI ruMap ruMapFree(ruMap rm) {
    ruClearError();
    u_int32_t i;
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

static int32_t MapPut(Map* mp, ptr key, ptr val, ptr* existingVal) {
    /* Hash the key. */
    mp->iterActive = false;
    int32_t bucket = mp->hash(key) % mp->buckets;
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
            item->key = key;
            item->value = val;
            return RUE_OK;
        }
    }
    // TODO: check whether to remap
    kv *item = ruMalloc0(1, kv);
    item->key = key;
    item->value = val;
    item->mp = mp;
    /* Insert the data into the bucket. */
    int32_t ret = ListInsertAfter(mp->table[bucket], NULL, item);
    if (ret == RUE_OK) {
        mp->size++;
    } else {
        kvFree(item);
    }
    return ret;
}

RUAPI int32_t ruMapPutData(ruMap map, ptr key, ptr val, ptr* exisitingVal) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(map, &ret);
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

static int32_t MapRemove(Map *mp, void *key, void **val) {
    /* Hash the key. */
    int32_t bucket = mp->hash(key) % mp->buckets;
    mp->iterActive = false;
    /* Search for the data in the bucket. */
    for (ListElmt *le = mp->table[bucket]->head->next; le != NULL; le = le->next) {
        kv* item = le->data;
        if (mp->match(key, item->key)) {
            /* Remove the data from the bucket.*/
            if (val) {
                *val = item->value;
                item->value = NULL;
            }
            int32_t ret;
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

RUAPI int32_t ruMapRemoveData(ruMap rm, void *key, void **val) {
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

static int32_t MapGetData(Map *mp, void *key, void **value) {
    /* Hash the key. */
    int32_t bucket = mp->hash(key) % mp->buckets;
    /* Search for the data in the bucket. */
    for (ListElmt *le = mp->table[bucket]->head->next; le != NULL; le = le->next) {
        kv* item = le->data;
        if (mp->match(key, item->key)) {
            /* Pass back the data from the table. */
            *value = item->value;
            return RUE_OK;
        }
    }
    /* Return that the data was not found. */
    return RUE_GENERAL;
}

RUAPI bool ruMapHasKey(ruMap rm, void *key, int32_t *code) {
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

RUAPI int32_t ruMapGetValue(ruMap rm, void* key, void** value) {
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
            *key = NULL;
        }
        if (value) {
            *value = NULL;
        }
        return RUE_FILE_NOT_FOUND;
    }
    kv* item = mp->iterElmt->data;
    if (key) {
        *key = item->key;
    }
    if (value) {
        *value = item->value;
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

RUAPI int32_t ruMapKeySet(ruMap rm, ruCloneFunc copy, ruList* keys, ruFreeFunc listFree) {
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

    for (u_int32_t i = 0; i < mp->buckets; i++) {
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

RUAPI u_int32_t ruMapSize(ruMap rm, int32_t *code) {
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

