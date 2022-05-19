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

ruMakeTypeGetter(Map, MapMagic)

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

RUAPI u_int32_t ruStrHash(const void *key) {
    const char *ptr = key;
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
    return val;
}

RUAPI bool ruStrMatch(const void* s1, const void* s2) {
    return ruStrcmp(s1, s2) == 0;
}

RUAPI ruMap ruMapNewString(void (*keyFree)(void *key), void (*valFree)(void *val)) {
    return ruMapNew(ruStrHash, ruStrMatch, keyFree, valFree, 0);
}

RUAPI ruMap ruMapNew(u_int32_t (*hash)(const void *key),
                   bool (*match)(const void *key1, const void *key2),
                   void (*keyFree)(void *key),
                   void (*valFree)(void *val), u_int32_t expectedSize) {
    ruClearError();
    if (!hash || !match) return NULL;

    Map *mp = ruMalloc0(1, Map);
    mp->type = MapMagic;

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
    return (ruMap)mp;
}

RUAPI void ruMapFree(ruMap rm) {
    ruClearError();
    u_int32_t i;
    Map *mp = MapGet(rm, NULL);
    if(!mp) {
        return;
    }
    /* Destroy each bucket. */
    for (i = 0; i < mp->buckets; i++) {
        ListFree(mp->table[i]);
    }
    /* Free the storage allocated for the hash table. */
    ruFree(mp->table);
    /* No operations are allowed now, but clear the structure as a precaution. */
    memset(mp, 0, sizeof(Map));
    ruFree(mp);
}

RUAPI int32_t ruMapPut(ruMap map, void *key, void *val) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(map, &ret);
    if (!mp) {
        return ret;
    }
    if (!key) return RUE_PARAMETER_NOT_SET;

    /* Hash the key. */
    mp->iterActive = false;
    int32_t bucket = mp->hash(key) % mp->buckets;
    for (ListElmt *le = ListHead(mp->table[bucket]); le != NULL; le = le->next) {
        kv* item = le->data;
        if (mp->match(key, item->key)) {
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
    ret = ListInsertAfter(mp->table[bucket], NULL, item);
    if (ret == RUE_OK) {
        mp->size++;
    } else {
        kvFree(item);
    }
    return ret;
}

RUAPI int32_t ruMapRemove(ruMap rm, void *key, void **val) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) {
        return ret;
    }
    if (!key) return RUE_PARAMETER_NOT_SET;

    ListElmt *prev;
    /* Hash the key. */
    int32_t bucket = mp->hash(key) % mp->buckets;

    mp->iterActive = false;
    /* Search for the data in the bucket. */
    prev = NULL;
    for (ListElmt *le = ListHead(mp->table[bucket]); le != NULL; le = le->next) {
        kv* item = le->data;
        if (mp->match(key, item->key)) {
            /* Remove the data from the bucket.*/
            if (val) {
                *val = item->value;
                item->value = NULL;
            }
            ListRemoveAfter(mp->table[bucket], prev, &ret);
            if (ret == RUE_OK) {
                mp->size--;
                kvFree(item);
            }
            return ret;
        }
        prev = le;
    }
    /* Return that the data was not found. */
    return RUE_GENERAL;
}

int32_t MapGetData(Map *mp, void *key, void **value) {
    /* Hash the key. */
    int32_t bucket = mp->hash(key) % mp->buckets;
    /* Search for the data in the bucket. */
    for (ListElmt *le = ListHead(mp->table[bucket]); le != NULL; le = le->next) {
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

RUAPI bool ruMapHas(ruMap rm, void *key, int32_t *code) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) ruRetWithCode(code, ret, false);
    if (!key) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, false);
    void *val;
    ret = MapGetData(mp, key, &val);
    if (ret == RUE_OK) {
        ruRetWithCode(code, RUE_OK, true);
    }
    ruRetWithCode(code, RUE_OK, false);
}

RUAPI int32_t ruMapGetValue(ruMap rm, void *key, void **value) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) {
        return ret;
    }
    if (!key || !value) return RUE_PARAMETER_NOT_SET;
    return MapGetData(mp, key, value);
}

RUAPI int32_t ruMapIterInit(ruMap rm) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) {
        return ret;
    }
    mp->iterBucket = 0;
    mp->iterElmt = NULL;
    mp->iterActive = true;
    return RUE_OK;
}

RUAPI int32_t ruMapNextSet(ruMap rm, void **key, void **value) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) {
        return ret;
    }
    if (!mp->iterActive) return RUE_INVALID_STATE;
    if (!key && !value) return RUE_PARAMETER_NOT_SET;

    if (!mp->iterElmt) {
        do {
            if (mp->iterBucket >= mp->buckets) break;
            mp->iterElmt = ListHead(mp->table[mp->iterBucket]);
            mp->iterBucket++;
        } while(!mp->iterElmt);
    }
    if (!mp->iterElmt) {
        if (key) *(key) = NULL;
        if (value) *(value) = NULL;
        return RUE_FILE_NOT_FOUND;
    }
    kv* item = mp->iterElmt->data;
    if (key) *(key) = item->key;
    if (value) *(value) = item->value;
    mp->iterElmt = mp->iterElmt->next;
    return RUE_OK;
}

RUAPI int32_t ruMapRemoveAll(ruMap rm) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) return ret;

    for (u_int32_t i = 0; i < mp->buckets; i++) {
        while (ruListSize(mp->table[i], NULL) > 0) {
            kv* item = ListRemoveAfter(mp->table[i], NULL, &ret);
            if (ret == RUE_OK) {
                mp->size--;
                kvFree(item);
            }
        }
    }
    return RUE_OK;
}

RUAPI u_int32_t ruMapSize(ruMap rm, int32_t *code) {
    ruClearError();
    int32_t ret;
    Map *mp = MapGet(rm, &ret);
    if (!mp) {
        ruRetWithCode(code, ret, 0);
    }
    ruRetWithCode(code, ret, mp->size);
}

