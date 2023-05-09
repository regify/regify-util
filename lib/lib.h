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
    #include <utime.h>
#else
    #include <io.h>
    #include <sys/utime.h>
#endif
#ifdef _WIN32
    #include <windows.h>
    #include <sys/types.h>
#else
    #include <sys/param.h>
    #include <sys/syscall.h>
#if defined(ITS_OSX) || defined(ITS_IOS)
    #include <sys/statvfs.h>
#endif
#ifdef __linux__
    #include <sys/vfs.h>
#endif
#endif
#include <ctype.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>
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
#define strncasecmp _strnicmp
#define timegm _mkgmtime
#define timelocal mktime
#define localtime_r localtime_s
#define utimbuf _utimbuf
#define utime _utime
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
extern RU_THREAD_LOCAL char* logPidEnd;

void ruSetError(const char *format, ...);
void ruClearError(void);

// limits
//#define RU_MIN_INT64	(int64_t)-0x8000000000000000L
#define RU_MAX_INT64	0x7fffffffffffffffL
#define RU_MAX_UINT64	0xffffffffffffffffL

// Type checking is done with the first element being uint64_t magic must be < uint16_t
#define MagicMux            2301
#define MagicString         2302
#define MagicFileKvStore    2303
#define MagicListElmt       2304
#define MagicList           2305
#define MagicMap            2306
#define MagicThr            2307
#define MagicIni            2308
#define KvStoreMagic        2309
#define MagicTsc            2310
// cleaner.c #define MagicCleaner 2410

/*
 *  Mutex
 */
#ifdef _WIN32
#include <windows.h>
typedef HANDLE ruMutex_t;
#else
#include <pthread.h>
#include <sched.h>
typedef pthread_mutex_t ruMutex_t;
#endif

typedef struct mux_ {
    ru_uint type;
    ruMutex_t mux;
} Mux;

typedef struct thr_ {
    ru_uint type;
    ruThreadId tid;
    ruStartFunc start;
    void* user;
    void* exitRes;
    bool finished;
} Thr;

Mux* ruMuxInit(void);
void ruMuxLock(Mux *mux);
void ruMuxUnlock(Mux *mux);

// thread safe counter
typedef struct tsc_ {
    ru_uint type;
    ruMutex mutex;  // counter mutex
    int64_t count;  // counter
} tsc;

/*
 *  Strings
 */
typedef struct String_ {
    ru_uint type;
    char *start;
    rusize idx;
    rusize len;
} String;
/**
 * Replaces non standard file slashes with the proper ones
 * @param inPath path to convert
 * @return converted path to be freed by the caller
 */
alloc_chars fixPath(const char *inPath);

/*
 *  Lists
 */
typedef struct ListElmt_ {
    ru_uint type;
    struct ListElmt_* prev;
    ptr data;
    struct ListElmt_* next;
} ListElmt;

typedef struct List_ {
    ru_uint type;
    uint32_t size;
    void (*destroy)(void *data);
    ListElmt* head;
    ListElmt* tail;
    // optional thread safety
    ruMutex mux;
    bool doQuit;    // flag to initiate map shutdown
} List;

List* ListNew(void (*destructor)(void *data));
void ListFree(List *list);
ptr ListRemove(List* list, ListElmt* old_element, int32_t* code);
int32_t ListInsertAfter(List *list, ruListElmt rle, ptr data);

/*
 *  Maps
 */
typedef struct Map_ {
    ru_uint type;
    uint32_t   buckets;
    ruHashFunc hash;
    ruMatchFunc match;
    ruFreeFunc keyFree;
    ruFreeFunc valFree;
    uint32_t   size;
    List   **table;
    // optional thread safety
    ruMutex mux;
    bool doQuit;    // flag to initiate map shutdown
    // iterator
    bool iterActive;
    uint32_t iterBucket;
    ListElmt *iterElmt;
} Map;


/*
 * Ini Files
 */
typedef struct Ini_ {
    ru_uint type;     // magic
    ruMap keys;         // char*: char*
    ruMap sections;     // char*: ruMap(char*: char*)
} Ini;

/**
 * Callback that will be called with ini file content
 *
 * @param user The user context that was passed to \ref ruIniRead
 * @param section The current section
 *                NULL if there is no section
 * @param name The field name, maybe be repeated if value spans multiple lines
 *             NULL on new section
 * @param value The value of the field
 *              NULL on new section
 *              NULL if the entry was just name
 *              blank if the entry was name =
 * @param lineno the line number where this callback occurred
 * @return true to continue of false to exit
 */
typedef bool (*ruIniCallback)(void* user, const char* section,
                             const char* name, const char* value,
                             int lineno);

typedef int (*writer)(void* stream, const char* format, ...);

/**
 * \brief Parse given zero-terminated IONI file data string.
 * May have [section]s, name=value pairs (whitespace stripped), and comments
 * starting with ';' (semicolon). Section is "" if name=value pair parsed before
 * any section heading. name:value pairs are also supported as a concession to
 * Python's configparser.
 *
 * For each name=value pair parsed, call handler function with given user
 * pointer as well as section, name, and value (data only valid for duration
 * of handler call). Handler should return nonzero on success, zero on error.
 *
 * @param string
 * @param handler The user specified callback
 * @param user The context to be passed to the handler
 * @return \ref RUE_OK on success else an error code
 */
int32_t iniParseString(const char* string, ruIniCallback handler, void* user);


/*
 *  Misc - Lib
 */
void ruAbort(void);
void U_CALLCONV traceData( const void *context, int32_t fnNumber, int32_t level,
                           const char *fmt, va_list args);
bool ruIsunreserved(unsigned char in);
sec_t timeParse(trans_chars dateformat, trans_chars datestr, bool utc);

// ICU stuff
UConverter* getConverter(void);
UChar* convToUni(UConverter *conv, trans_chars instr, int32_t inlen);
alloc_chars convToStr(UConverter *conv, UChar *usrc, int32_t uclen);
UChar* charToUni(const char *instr);
alloc_chars uniToChar(UChar *usrc);
UChar* uniSwitchCase(UChar* usrc, bool isUpper);
alloc_chars utf8SwitchCase(trans_chars instr, bool isUpper);

// io stuff
int32_t errno2rfec(int err);
rusize_s ruWrite(int oh, const void* contents, rusize length);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_LIB_H
