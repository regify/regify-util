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
#ifndef REGIFY_UTIL_LIB_H
#define REGIFY_UTIL_LIB_H

#ifdef _WIN64
    // needed for our logger
    // https://sourceforge.net/p/mingw-w64/wiki2/gnu%20printf/
    #define __USE_MINGW_ANSI_STDIO 1
#endif
#define RU_BUILDING
#include <regify-util.h>
#ifndef RUMS
    #include <dirent.h>
    #include <sys/time.h>
    #include <unistd.h>
#else
    #include <io.h>
#endif
#ifdef _WIN32
    #include <windows.h>
#endif
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include "unicode/uchar.h"
#include "unicode/ustring.h"
#include "unicode/utypes.h"
#include "unicode/ustdio.h"
#include "unicode/ucasemap.h"
#include "unicode/uregex.h"
#include "unicode/uiter.h"
#include "unicode/unorm2.h"

#ifndef NDEBUG
#include "unicode/utrace.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef TARGET_OS_IOS
#if TARGET_OS_IOS != 0
#define DO_IOS
#endif
#endif

#ifdef RUMS
    // running non posix
#define strcasecmp _stricmp
#endif

// Error reporting
#define RU_ERRBUF_SIZE 2048
#ifdef RUMS

#define RU_SLASH '\\'
#define RU_SLASH_S "\\"

#else

#define RU_SLASH '/'
#define RU_SLASH_S "/"

#endif

extern unsigned int ruIntChunk;

void ruSetError(const char *format, ...);
void ruClearError();

// limits
#define RU_MIN_INT64	(int64_t)-0x8000000000000000L
#define RU_MAX_INT64	0x7fffffffffffffffL
#define RU_MAX_UINT64	0xffffffffffffffffL

/*
 *  Mutex
 */
#ifdef RUMS
#include <windows.h>
typedef HANDLE ruMutex_t;
#else
#include <pthread.h>
typedef pthread_mutex_t ruMutex_t;
#endif
typedef struct mux_ {
    ruMutex_t mux;
    u_int32_t type;
} Mux;
#define MuxMagic 23014201

/*
 *  Strings
 */
typedef struct String_ {
    char *start;
    rusize idx;
    rusize len;
    u_int32_t type;
} String;
#define StringMagic 23014202
char* fixPath(const char *inPath);

/*
 *  Lists
 */
typedef struct ListElmt_ {
    void               *data;
    struct ListElmt_   *next;
    u_int32_t type;
} ListElmt;
#define ListElmtMagic 23014204

typedef struct List_ {
    int32_t                size;
    void               (*destroy)(void *data);
    ListElmt           *head;
    ListElmt           *tail;
    u_int32_t type;
} List;
#define ListMagic 23014205

List* ListNew(void (*destructor)(void *data));
void ListFree(List *list);
void* ListRemoveAfter(List *list, ruListElmt rle, int32_t *code);
ListElmt* ListHead(List *list);
int32_t ListInsertAfter(List *list, ruListElmt rle, const void *data);

/*
 *  Maps
 */
typedef struct Map_ {
    u_int32_t   buckets;
    u_int32_t   (*hash)(const void *key);
    bool   (*match)(const void *key1, const void *key2);
    void   (*keyFree)(void *key);
    void   (*valFree)(void *val);
    u_int32_t   size;
    List   **table;
    // iterator
    bool iterActive;
    u_int32_t iterBucket;
    ListElmt *iterElmt;
    u_int32_t type;
} Map;
#define MapMagic 23014206

int32_t MapGetData(Map *mp, void *key, void **value);

/*
 *  Misc - Lib
 */
void ruAbort();
void U_CALLCONV traceData( const void *context, int32_t fnNumber, int32_t level,
                           const char *fmt, va_list args);
bool ruIsunreserved(unsigned char in);

// ICU stuff
UConverter* getConverter();
UChar* strToUni(UConverter *conv, const char *instr);
UChar* charToUni(const char *instr);
char* uniToStr(UConverter *conv, UChar *udst, int32_t uclen);
char* uniNToChar(UChar *udst, int32_t len);
char* uniToChar(UChar *udst);
UChar* uniSwitchCase(UChar* usrc, bool isUpper);
char* utf8SwitchCase(const char *instr, bool isUpper);

// io stuff
int32_t errno2rfec(int err);
rusize_s ruWrite(int oh, const void* contents, rusize length);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_LIB_H
