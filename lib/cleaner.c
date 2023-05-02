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
/**
 * \file
 * The file cleaner.c contains the ruClean* implementation for password cleaning
 * logs and such.
 *
 */
#ifdef CLEANER_ONLY
// for including only the libpwcleaner without ICU and other depenedencies
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>

typedef const char* perm_chars;
typedef const char* trans_chars;
typedef char* alloc_chars;
typedef const void* trans_ptr;
typedef const void* perm_ptr;
typedef void* ptr;

#define RUE_OK 0
#define RUE_INVALID_PARAMETER 64
#define RUE_PARAMETER_NOT_SET 77
#define RUE_INVALID_STATE	325
#define ruFree(p) if(p) free((void*)(p)); (p) = NULL;
typedef void (*ruCleanerCb) (perm_ptr user_data, trans_chars key, trans_chars subst);
typedef size_t rusize;
#if defined(WINDOWS) || defined(WIN32) || defined(__BORLANDC__)
    #ifndef uint8_t
        typedef unsigned char uint8_t;
    #endif
    #ifndef int32_t
        typedef int int32_t;
    #endif
    #ifndef uint32_t
        typedef unsigned int uint32_t;
    #endif
    #ifndef int64_t
        typedef long long int64_t;
    #endif
    #ifdef _WIN64
        typedef int64_t rusize_s;
    #else
        typedef int rusize_s;
    #endif
#else
typedef signed long rusize_s;
#endif
void* ruMallocSize(rusize count, rusize ofsize) {
    if (!count || !ofsize) return NULL;
    void *p = calloc(count, ofsize);
    if (!p) {
        exit(1);
    }
    return p;
}
#define ruMacStart do
#define ruMacEnd while(0)
#define ruMalloc0(count, ctype) (ctype*) ruMallocSize(count, sizeof(ctype));
#define ruRetWithCode(ptr, code, res) ruMacStart { \
    if (ptr) *(ptr) = code; return res; \
    } ruMacEnd
#define ruMakeTypeGetter(ctype, magic) \
ctype* ctype ## Get(void* ptr, int32_t* code) { \
    ctype* ret = (ctype*) ptr; \
    if (!ptr) { \
        ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL); \
    } \
    if (ptr < (void*)0xffff || (magic) != ret->type) { \
        ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL); \
    } \
    ruRetWithCode(code, RUE_OK, ret); \
}
char* ruStrDup(const char* str) {
    if (!str) return NULL;
#if _WIN32
    char *ret = _strdup(str);
#else
    char *ret = strdup(str);
#endif
    if (!ret) {
        exit(1);
    }
    return ret;
}

bool ruStrEmpty(trans_chars str) {
    if (!str) return true;
    while (*str) {
        if (!isspace((unsigned char)*str)) return false;
        str++;
    }
    return true;
}

typedef void* ruCleaner;
typedef rusize_s (*rcWriteFn) (perm_ptr ctx, trans_ptr buf, rusize len);
typedef rusize_s (*rcReadFn) (perm_ptr ctx, ptr buf, rusize len);
#else
// standard part of libregify-util
#include "lib.h"
#endif

/*
 * Cleaner
 */
typedef struct Tree_ Tree;

struct Tree_ {
    uint8_t me;
    Tree* kids[256];
    alloc_chars subst;
};

typedef struct {
    uint32_t type;
    Tree *root;
    Tree *leaf;

    bool buffered;
    char *inHeap;
    char *inBuf;
    char *inEnd;
    char *matchStart;
    char *cur;
    char *next;
    uint8_t thisChar;

    char *outBuf;
    char *outEnd;
    char *outCur;

    rusize chunkSize;
    rusize bufLen;
    rusize longestEntry;

    int32_t error;

    rcReadFn read;
    perm_ptr readCtx;
    rcWriteFn write;
    perm_ptr writeCtx;
#ifndef CLEANER_ONLY
    ruMutex mux;
#endif

    rusize memsize;
} Cleaner;
#define MagicCleaner        2410
ruMakeTypeGetter(Cleaner, MagicCleaner)

static Tree* newBranch(Cleaner *c, uint8_t letter) {
    Tree *t = ruMalloc0(1, Tree);
    t->me = letter;
    c->memsize += sizeof(Tree);
    return t;
}

static Tree* freeBranch(Cleaner *c, Tree *t) {
    for (int i = 0; i < 256; i++) {
        if (! t->kids[i]) continue;
        t->kids[i] = freeBranch(c, t->kids[i]);
    }
    ruFree(t->subst);
    c->memsize -= sizeof(Tree);
    ruFree(t);
    return NULL;
}

static bool entryUsed(Cleaner *c, Tree *t) {
    for (int i = 0; i < 256; i++) {
        if (t->kids[i]) return true;
    }
    return false;
}

static void dumpEntry(Cleaner *c, Tree *t, char* instr, char* cur, rusize inlen,
                      ruCleanerCb lf, perm_ptr lctx) {
    if ((rusize)(cur-instr) < inlen) {
        for (int i = 0; i < 256; i++) {
            if (t->kids[i]) {
                *cur = (char)i;
                *(cur+1) = '\0';
                dumpEntry(c, t->kids[i], instr, cur+1, inlen, lf, lctx);
            }
        }
    }
    *cur = '\0';
    lf(lctx, instr, t->subst);
}

static void addEntry(Cleaner *c, Tree *t, trans_chars instr, trans_chars subst) {
    uint8_t b = *instr;
    int i = (int) b;
    if (!t->kids[i]) {
        if (!subst) return; // should happen, but anyway
        t->kids[i] = newBranch(c, b);
    }

    if (*(instr+1)) {
        addEntry(c, t->kids[i], instr + 1, subst);
        if (!subst) {
            if (!entryUsed(c, t->kids[i])) {
                t->kids[i] = freeBranch(c, t->kids[i]);
            }
        }
    } else {
        // free up potential duplicate entry
        ruFree(t->kids[i]->subst);
        if (subst) {
            t->kids[i]->subst = ruStrDup(subst);
        } else {
            if (!entryUsed(c, t->kids[i])) {
                t->kids[i] = freeBranch(c, t->kids[i]);
            }
        }
    }
}

static void matchToStart(Cleaner *c) {
    char *src = c->matchStart;
    char *dst = c->inBuf;
    do {
        *dst = *src;
        dst++;
        src++;
    } while( src < c->inEnd);
    c->cur = dst; // end of match
}

static void bufferedRead(Cleaner *c) {
    c->cur++;
    if (!c->buffered) return; // inc only op
    if (c->cur < c->inEnd) return;

    if (c->matchStart) {
        matchToStart(c);
    } else {
        c->cur = c->inBuf;
    }
    rusize readLen = c->inEnd - c->cur;
    rusize_s ret = c->read(c->readCtx, c->cur, readLen);
    if ((rusize)ret < readLen) {
        if (ret < 0) {
            // error
            *c->cur = '\0';
        } else {
            // eof?
            *(c->cur + ret) = '\0';
        }
    }
}

static void flush(Cleaner *c) {
    char *buf = c->outBuf;
    rusize len = c->outCur - c->outBuf;
    while (true) {
        rusize_s ret = c->write(c->writeCtx, buf, len);
        if (ret < 0) {
            // write error
            c->error = errno;
            return;
        }
        if ((rusize)ret < len) {
            buf += ret;
            len -= ret;
        } else {
            c->outCur = c->outBuf;
            return;
        }
    }
}

static void bufferedWrite(Cleaner *c, trans_chars buf, rusize len) {
    trans_chars end = buf + len;
    while (buf < end) {
        if (c->outCur >= c->outEnd) {
            flush(c);
            if (c->error) return;
            continue;
        }
        *c->outCur = *buf;
        c->outCur++;
        buf++;
    }
}

static void peekNext(Cleaner *c) {
    if (!c->next) {
        bufferedRead(c);
        c->next = c->cur;
    }
}

static void getNext(Cleaner *c) {
    peekNext(c);
    c->cur = c->next;
    c->next = 0;
    if (c->cur < c->inEnd) {
        c->thisChar = *c->cur;
    } else {
        // terminate by length parameter
        c->thisChar = '\0';
    }
}

static void doCharacter(Cleaner *c, trans_chars subst) {
    if (!subst && !c->matchStart) {
        // write character
        bufferedWrite(c, c->cur, 1);
        return;
    }
    if (subst) {
        // replace text
        bufferedWrite(c, subst, strlen(subst));
    } else if(c->matchStart) {
        // just pass the text
        bufferedWrite(c, c->matchStart, c->cur-c->matchStart);
    }
    c->matchStart = NULL;
    c->leaf = c->root;
}

static bool walkText(Cleaner *c) {
    getNext(c);
    if (!c->thisChar) return false;

    Tree *leaf = c->leaf->kids[(int) c->thisChar];
    if (!leaf) {
        // no match
        doCharacter(c, NULL);
        return c->error == 0;
    } else {
        if (!c->matchStart) {
            // start of match
            c->matchStart = c->cur;
        }
        peekNext(c);
        if (!c->next) {
            // eof
            doCharacter(c, NULL);
            return c->error == 0;
        } else {
            if (!leaf->kids[(int)(uint8_t)*c->next]) {
                doCharacter(c, leaf->subst);
            } else {
                c->leaf = leaf;
            }
        }
    }
    return c->error == 0;
}

static int32_t cleanNow(Cleaner *c) {
    if (!c->write || (c->buffered && !c->read)) return RUE_PARAMETER_NOT_SET;

#ifndef CLEANER_ONLY
    ruMutexLock(c->mux);
#endif
    if (!c->inHeap || !c->outBuf) {
        c->bufLen = c->chunkSize;
        if (c->bufLen < c->longestEntry) {
            c->bufLen = c->longestEntry;
        }
    }
    // initialize
    if (c->buffered) {
        if (!c->inHeap) {
            c->inHeap = ruMalloc0(c->bufLen, char);
        }
        c->inBuf = c->inHeap;
        c->inEnd = c->inBuf + c->bufLen;
        c->cur = c->inEnd; // trigger initial read
    } else {
        // set to start -1 because bufferedRead increments first.
        c->cur = c->inBuf - 1;
    }
    if (!c->outBuf) {
        c->outBuf = ruMalloc0(c->bufLen, char);
        c->outEnd = c->outBuf + c->bufLen;
    }
    c->leaf = c->root;
    c->outCur = c->outBuf;

    while(walkText(c));
    if (!c->error) {
        flush(c);
    }
#ifndef CLEANER_ONLY
    ruMutexUnlock(c->mux);
#endif
    return c->error;
}

ruCleaner ruCleanNew(rusize chunkSize) {
    Cleaner *c = ruMalloc0(1, Cleaner);
    c->memsize += sizeof(Cleaner);
    c->root = newBranch(c, '\0');
    c->chunkSize = chunkSize;
    if (!c->chunkSize) c->chunkSize = 1024 * 1024;
    c->type = MagicCleaner;
#ifndef CLEANER_ONLY
    c->mux = ruMutexInit();
#endif
    return (ruCleaner)c;
}

ruCleaner ruCleanFree(ruCleaner cp) {
    Cleaner *c = CleanerGet(cp, NULL);
    if (!c) return NULL;
    if (c->root) {
        c->root = c->leaf = freeBranch(c, c->root);
    }
    ruFree(c->inHeap);
    ruFree(c->outBuf);
#ifndef CLEANER_ONLY
    c->mux = ruMutexFree(c->mux);
#endif
    c->type = 0;
    ruFree(c);
    return NULL;
}

int32_t ruCleanAdd(ruCleaner rc, trans_chars instr, trans_chars substitute) {
    int32_t code;
    Cleaner *c = CleanerGet(rc, &code);
    if (!c) return code;
    if (ruStrEmpty(instr) || !substitute) return RUE_PARAMETER_NOT_SET;

#ifndef CLEANER_ONLY
    ruMutexLock(c->mux);
#endif
    addEntry(c, c->root, instr, substitute);
    rusize len = strlen(instr);
    if (len > c->longestEntry) {
        c->longestEntry = len;
        if (len > c->bufLen) {
            ruFree(c->inHeap);
            ruFree(c->outBuf);
        }
    }
#ifndef CLEANER_ONLY
    ruMutexUnlock(c->mux);
#endif
    return code;
}

int32_t ruCleanRemove(ruCleaner rc, trans_chars instr) {
    int32_t code;
    Cleaner *c = CleanerGet(rc, &code);
    if (!c) return code;
    if (!instr) return RUE_PARAMETER_NOT_SET;

#ifndef CLEANER_ONLY
    ruMutexLock(c->mux);
#endif
    addEntry(c, c->root, instr, NULL);
#ifndef CLEANER_ONLY
    ruMutexUnlock(c->mux);
#endif
    return code;
}

int32_t ruCleanDump(ruCleaner cp, ruCleanerCb lf, perm_ptr user_data) {
    int32_t code;
    Cleaner *c = CleanerGet(cp, &code);
    if (!c) return code;
    if (!lf) return RUE_PARAMETER_NOT_SET;

    char* instr = malloc(c->longestEntry + 1);
    *instr = '\0';
    dumpEntry(c, c->root, instr, instr, c->longestEntry, lf, user_data);
    free(instr);
    return code;
}

int32_t ruCleanIo(ruCleaner rc, rcReadFn reader, perm_ptr readCtx,
                  rcWriteFn writer, perm_ptr writeCtx) {
    int32_t code;
    Cleaner *c = CleanerGet(rc, &code);
    if (!c) return code;
    c->buffered = true;
    c->read = reader;
    c->readCtx = readCtx;
    c->write = writer;
    c->writeCtx = writeCtx;
    return cleanNow(c);
}

int32_t ruCleanToWriter(ruCleaner rc, trans_chars in, rusize len,
                        rcWriteFn writer, perm_ptr writeCtx) {
    int32_t code;
    Cleaner *c = CleanerGet(rc, &code);
    if (!c) return code;
    if (!in) return RUE_PARAMETER_NOT_SET;
    if (!len) len = strlen(in);

    c->buffered = false;
    c->inBuf = (char*) in;
    c->inEnd = c->inBuf + len;
    c->write = writer;
    c->writeCtx = writeCtx;

    return cleanNow(c);
}

#ifndef CLEANER_ONLY
static rusize_s myappend(perm_ptr ctx, trans_ptr buf, rusize len) {
    ruString io = (ruString)ctx;
    if (RUE_OK == ruBufferAppend(io, buf, len)) return len;
    return -1;
}

int32_t ruCleanToString(ruCleaner rc, trans_chars in, rusize len, ruString *out) {
    if (!out || !in) return RUE_PARAMETER_NOT_SET;
    if (!len) len = strlen(in);
    ruBuffer res = ruBufferNew((rusize)(len * 1.1)); // 10% margin
    int ret = ruCleanToWriter(rc, in, len, &myappend, res);
    if (ret == RUE_OK) {
        *out = res;
    } else {
        ruStringFree(res, false);
        *out = NULL;
    }
    return ret;
}
#endif

