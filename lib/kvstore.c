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

//<editor-fold desc="generic store">
ruMakeTypeGetter(KvStore, KvStoreMagic)

RUAPI KvStore* ruNewStore(void) {
    ruClearError();
    KvStore* kvs = ruMalloc0(1, KvStore);
    kvs->type = KvStoreMagic;
    return kvs;
}

RUAPI void ruFreeStore(void* obj) {
    ruClearError();
    KvStore *kvs = KvStoreGet(obj, NULL);
    if (kvs && kvs->ctxFree) kvs->ctxFree(kvs->ctx);
    ruFree(kvs);
}

RUAPI int32_t ruValidStore(void* obj) {
    ruClearError();
    int32_t ret = RUE_OK;
    KvStoreGet(obj, &ret);
    return ret;
}
//</editor-fold>

//<editor-fold desc="file store">
typedef struct FileKvStore_ {
    ru_uint type;
    char *folderPath;
} FileKvStore;
ruMakeTypeGetter(FileKvStore, MagicFileKvStore)

ruFileStore ruKvFileFree(ruFileStore ctx) {
    FileKvStore *fks = FileKvStoreGet(ctx, NULL);
    if (!fks) return NULL;
    ruFree(fks->folderPath);
    ruFree(fks);
    return NULL;
}

RUAPI KvStore* ruNewFileStore(const char *folderPath, int32_t* code) {
    ruClearError();
    // check folder path and writability first
    if (!folderPath) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL);
    if (!ruIsDir(folderPath)) {
        int32_t ret = ruMkdir(folderPath, 0755, true);
        if (ret != RUE_OK) ruRetWithCode(code, ret, NULL);
    }

    KvStore* kvs = ruNewStore();
    FileKvStore* fks = ruMalloc0(1, FileKvStore);
    fks->type = MagicFileKvStore;
    fks->folderPath = ruStrDup(folderPath);
    kvs->ctx = fks;
    kvs->ctxFree = (ruFreeFunc)ruKvFileFree;
    kvs->get = ruFileStoreGet;
    kvs->set = ruFileStoreSet;
    kvs->list = ruFileStoreList;
    ruRetWithCode(code, RUE_OK, kvs);
}

static inline char getNum(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0'; // add number
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}

static char* pathToKey(FileKvStore *fks, const char*path) {
    char *start = (char*)path + strlen(fks->folderPath) + 1; // trailing slash
    rusize klen = strlen(start);
    char *replacement = ruMalloc0(klen+1, char);
    char *t = replacement;
    const char *end = start + klen-1;
    for (const char *p = start; p <= end; p++) {
#ifdef _WIN32
        if (*p == '/' || *p == '\\') {
#else
        if (*p == '/') {
#endif
            *t = ' ';
            t++;
            continue;
        }
        if (*p == '_') {
            uint8_t d = getNum(*(p+1));
            if (d != 0xff) {
                uint8_t s = getNum(*(p+2));
                if (s != 0xff) {
                    *t = (d << 4) + s;
                    t++;
                    p += 2;
                    continue;
                }
            }
        }
        // no decoding
        *t = *p;
        t++;
    }
    return replacement;
}

bool isEscaped(char c, bool ends) {
    if (c >= 'a' && c <= 'z') return true;
    if (c >= 'A' && c <= 'Z') return true;
    if (c >= '0' && c <= '9') return true;
    if (!ends && (c == '.' || c == '-')) return true; // allowed in the middle
    return false;
}

static char* keyToPath(FileKvStore *fks, const char* key) {
    rusize cnt = 0, klen = strlen(key);
    rusize base = strlen(fks->folderPath) + 1; // trailing slash
    const char *end = key+klen-1;
    for(const char *p = key; p <= end; p++) {
        if (isEscaped(*p, p == key || p == end)) cnt ++;
        else cnt += 3;
    }
    char *replacement = ruMalloc0(base+cnt+1, char);
    sprintf(replacement, "%s/", fks->folderPath);
    char *t = replacement + base;
    for (const char *s = key; s <= end; s++) {
        if (*s == '*' && s == end && *(t-1) == '/') {
            // leave the last * as wild card for delete
            *t = *s;
            t++;
        } else if (*s == ' ') {
            // we collapse multiple WS into a single space or /
            if (t == replacement || *(t-1) != '/') {
                *t = '/';
                t++;
            }
        } else if (isEscaped(*s, s == key || s == end || *(t-1) == '/' || *(s+1) == ' ')) {
            *t = *s;
            t++;
        } else {
            snprintf(t, 4, "_%x", *s);
            t += 3;
        }
    }
    return replacement;
}

RUAPI int32_t ruFileStoreSet (KvStore *kvs, const char* key,
                              const char *val, rusize len) {
    ruClearError();
    if (!key) return RUE_PARAMETER_NOT_SET;
    if (!strlen(key)) return RUE_INVALID_PARAMETER;
    int32_t ret = RUE_OK;
    kvs = KvStoreGet(kvs, &ret);
    if (!kvs) return ret;
    FileKvStore *fks = FileKvStoreGet(kvs->ctx, &ret);
    if (!fks) return ret;
    char *filepath = keyToPath(fks, key);
    char *dirpath = ruDirName(filepath);
    do {
        if (ruStrEndsWith(filepath, "*", NULL)) {
            if (val) {
                ruSetError("Trailing wildcard * is only valid with NULL values");
                ret = RUE_INVALID_PARAMETER;
                break;
            }
            // whack the folder
            ret = ruFolderRemove(dirpath);
            break;
        }
        if (!ruIsDir(dirpath)) {
            ret = ruMkdir(dirpath, 0755, true);
            if (ret != RUE_OK) {
                ruVerbLogf("failed creating folder '%s' error: %d", dirpath, ret);
                break;
            }
        }
        if (val) {
            ret = ruFileSetContents(filepath, (const char*)val, len);
            if (ret != RUE_OK) {
                ruVerbLogf("failed writing value into '%s' error: %d", filepath, ret);
                break;
            }
        } else {
            if(ruFileRemove(filepath) != 0) {
                ruVerbLogf("failed deleting '%s' errno: %d - %s",
                           filepath, errno, strerror(errno));
                ret = errno2rfec(errno);
                break;
            }
        }
    } while (0);
    ruFree(dirpath);
    ruFree(filepath);
    return ret;
}

RUAPI int32_t ruFileStoreGet (KvStore *kvs, const char* key, char **val,
                            rusize* len) {
    ruClearError();
    if (!key || !val || !len) return RUE_PARAMETER_NOT_SET;
    if (!strlen(key)) return RUE_INVALID_PARAMETER;
    int32_t ret = RUE_OK;
    kvs = KvStoreGet(kvs, &ret);
    if (!kvs) return ret;
    FileKvStore *fks = FileKvStoreGet(kvs->ctx, &ret);
    if (!fks) return ret;
    char *filepath = keyToPath(fks, key);
    ret = ruFileGetContents(filepath, (char**)val, len);
    ruFree(filepath);
    return ret;
}

struct _storeList {
    FileKvStore *fks;
    ruList lst;
};

static int lister(const char*fullPath, bool isDir, void* o) {
    if (isDir) return RUE_OK;
    struct _storeList *sl = (struct _storeList *)o;
    FileKvStore *fks = sl->fks;
    ruList lst = sl->lst;
    char *key = pathToKey(fks, fullPath);
    return ruListAppend(lst, key);
}

RUAPI int32_t ruFileStoreList (KvStore *kvs, const char* key, ruList* result) {
    ruClearError();
    if (!key || !result) return RUE_PARAMETER_NOT_SET;
    if (!strlen(key)) return RUE_INVALID_PARAMETER;
    int32_t ret = RUE_OK;
    kvs = KvStoreGet(kvs, &ret);
    if (!kvs) return ret;
    FileKvStore *fks = FileKvStoreGet(kvs->ctx, &ret);
    if (!fks) return ret;
    char *path, *dirpath = NULL, *filepath = keyToPath(fks, key);
    if (ruStrEndsWith(filepath, "*", NULL)) {
        path = dirpath = ruDirName(filepath);
    } else {
        path = filepath;
    }
    ruList lst = ruListNewType(ruTypePtrFree());
    struct _storeList sl;
    sl.fks = fks;
    sl.lst = lst;
    ret = ruFolderWalk(path, RU_WALK_FOLDER_LAST,lister, &sl);
    ruFree(dirpath);
    ruFree(filepath);
    if (ret != RUE_OK) {
        lst = ruListFree(lst);
    }
    *result = lst;
    return ret;
}
//</editor-fold>

//<editor-fold desc="ini store">
//</editor-fold>

//<editor-fold desc="null store">
RUAPI KvStore* ruNewNullStore(void) {
    KvStore* kvs = ruNewStore();
    kvs->get = ruNullStoreGet;
    kvs->set = ruNullStoreSet;
    kvs->list = ruNullStoreList;
    return kvs;
}

RUAPI int32_t ruNullStoreSet (KvStore *kvs, const char* key,
                              const char *val, rusize len) {
    ruClearError();
    if (!key) return RUE_PARAMETER_NOT_SET;
    if (!strlen(key)) return RUE_INVALID_PARAMETER;
    return RUE_OK;
}

RUAPI int32_t ruNullStoreGet (KvStore *kvs, const char* key, char **val,
                            rusize* len) {
    ruClearError();
    if (!key || !val || !len) return RUE_PARAMETER_NOT_SET;
    if (!strlen(key)) return RUE_INVALID_PARAMETER;
    *val = NULL;
    *len = 0;
    return RUE_OK;
}

RUAPI int32_t ruNullStoreList (KvStore *kvs, const char* key, ruList* list) {
    ruClearError();
    if (!key || !list) return RUE_PARAMETER_NOT_SET;
    if (!strlen(key)) return RUE_INVALID_PARAMETER;
    *list = NULL;
    return RUE_OK;
}
//</editor-fold>
