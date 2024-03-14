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
 * \mainpage
 * \copyright regify. This project is released under the MIT License.
 *
 * The regify utility package is a collection of general utilities ranging from
 * \ref string, over collections like \ref list or \ref hashmap to \ref logging,
 * \ref regex and abstracted storage such as \ref kvstore_sec. There are also \ref
 * io utilities.
 * It is designed to run on Unix derivatives (Linux, Mac OSX tested), Windows,
 * Android and iOS.
 * All char* input/output is expected to be valid UTF-8.
 *
 * \defgroup misc Miscellaneous Functions
 * \brief Abstracted functions pertaining to other areas.
 *
 * @{
 */
#ifndef UTIL_REGIFY_UTIL_H
#define UTIL_REGIFY_UTIL_H

/* for our error codes only */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

/* Only need to export C interface if used by C++ source code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** \cond noworry */
#if defined(regify_util_EXPORTS)
    #if (defined(WINDOWS) || defined(WIN32) || defined(__BORLANDC__))
        #ifdef RU_BUILDING
            #define RUAPI extern __declspec(dllexport)
        #else
            #define RUAPI extern __declspec(dllimport)
        #endif
    #else
        #if defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR__) >= 303
            #define RUAPI __attribute__ ((visibility ("default")))
        #else
            #define RUAPI
        #endif
    #endif
#else
    #define RUAPI
#endif
/** \endcond */

#if defined(WINDOWS) || defined(WIN32) || defined(__BORLANDC__)
    /** \cond noworry */
    #ifdef _MSC_VER
        // running non posix
        #define RUMS
    #endif

    #include <windows.h>

    #ifndef int32_t
        typedef int int32_t;
    #endif
    #ifndef int64_t
        typedef long long int64_t;
    #endif
    /** \endcond */

    /**
     * \brief Abstracted version of size_t but signed
     * \ingroup misc
     */
    #ifdef _WIN64
        typedef int64_t rusize_s;
    #else
        typedef int rusize_s;
    #endif

    /**
     * \brief Returns string of the requested registry entry.
     * \ingroup misc
     *
     * \param topKey Top level registry hierachy such as \b HKEY_LOCAL_MACHINE
     * \param tree Path to the leaf without the leaf such as \b SYSTEM\\Select
     * \param leaf Name of the leaf key such as \b Default
     * \param value Registry entry. Must be freed via \ref ruFree by the caller.
     * \return \ref RUE_OK on success else an error code.
     */
    RUAPI int32_t ruGetRegistryEntry(HKEY topKey, const char* tree,
                                     const char* leaf, char** value);

    /**
     * \brief Retrieves information about volume at given mount point.
     * \ingroup misc
     *
     * \param mountPoint Mount popint of volumen such as c:\
     * \param serialNo Where the serial number is stored if not NULL.
     * \param maxCompLen Where the maximum component length is stored if not NULL.
     * \param fsFlags Where the file system flags are stored if not NULL.
     * \param volumeName Where the volume name is stored if not NULL.
     *        Free with \ref ruFree
     * \param fsName Where the file system name is stored if not NULL.
     *        Free with \ref ruFree
     * \return \ref RUE_OK on success else an error code.
     */
    RUAPI int32_t ruGetVolumeInfo(const char* mountPoint,
            unsigned long* serialNo, unsigned long* maxCompLen, unsigned long* fsFlags,
            char** volumeName, char** fsName);

#else

#if defined(__APPLE__) && defined(__MACH__) // Apple OSX and iOS (Darwin)
#include <TargetConditionals.h>
    #if TARGET_OS_IPHONE == 1
        #ifndef ITS_IOS
            #define ITS_IOS 1
        #endif
    #elif TARGET_OS_MAC == 1
        #ifndef ITS_OSX
            #define ITS_OSX 1
        #endif
    #endif
#endif

    /* for folderwalk */
    #include <dirent.h>

    /**
     * \brief Abstracted version of size_t but signed
     */
    typedef signed long rusize_s;

#endif

/**
 * \brief Abstracted process id type.
 */
#ifdef _WIN32
typedef DWORD ru_pid;
#else
#include <sys/types.h>
typedef pid_t ru_pid;
#endif


/**
 * \brief A permanent NULL terminated string pointer.
 *
 * \ingroup misc
 * On input it needs to persist through the life of a given context.
 * On output it is guaranteed to live as long as it's context or until it has
 * been explicitly finalized. It must not be freed.
 * Within functions this can also be used for string pointers that must not be freed.
 */
typedef const char* perm_chars;

/**
 * \brief A transient NULL terminated string pointer.
 *
 * \ingroup misc
 * On input this string must only persist for the duration of the function call.
 * On output this string is valid until another call with the given context is
 * performed and must not be freed.
 */
typedef const char* trans_chars;

/**
 * \brief An allocated NULL terminated string pointer.
 *
 * \ingroup misc
 * On input this can represent a place on the stack or heap that will be written
 * to by the called function. Usually accompanied by a length parameter.
 * It can also represent a parameter that will be freed in conjunction with an
 * enclosing object after use.
 * On output this string belongs to the caller and must be freed after use.
 * Within functions this can also be used for string pointers that must be freed.
 */
typedef char* alloc_chars;

/**
 * \brief A permanent UTF16 wchar_t character type.
 *
 * \ingroup misc
 * On input it needs to persist through the life of a given context.
 * On output it is guaranteed to live as long as it's context or until it has
 * been explicitly finalized. It must not be freed.
 * Within functions this can also be used for string pointers that must not be freed.
 */
typedef const uint16_t* perm_uni;

/**
 * \brief A transient UTF16 wchar_t character type.
 *
 * \ingroup misc
 * On input this string must only persist for the duration of the function call.
 * On output this string is valid until another call with the given context is
 * performed and must not be freed.
 */
typedef const uint16_t* trans_uni;

/**
 * \brief An allocated UTF16 wchar_t character type.
 *
 * \ingroup misc
 * On input this can represent a place on the stack or heap that will be written
 * to by the called function. Usually accompanied by a length parameter.
 * It can also represent a parameter that will be freed in conjunction with an
 * enclosing object after use.
 * On output this data belongs to the caller and must be freed after use.
 * Within functions this can also be used for data pointers that must be freed.
 */
typedef uint16_t* alloc_uni;

/**
 * \brief A permanent data bytes pointer.
 *
 * \ingroup misc
 * On input it needs to persist through the life of a given context.
 * On output it is guaranteed to live as long as it's context or until it has
 * been explicitly finalized. It must not be freed.
 * Within functions this can also be used for string pointers that must not be freed.
 */
typedef const unsigned char* perm_bytes;

/**
 * \brief A transient data bytes pointer.
 *
 * \ingroup misc
 * On input this string must only persist for the duration of the function call.
 * On output this string is valid until another call with the given context is
 * performed and must not be freed.
 */
typedef const unsigned char* trans_bytes;

/**
 * \brief An allocated data bytes pointer.
 *
 * \ingroup misc
 * On input this can represent a place on the stack or heap that will be written
 * to by the called function. Usually accompanied by a length parameter.
 * It can also represent a parameter that will be freed in conjunction with an
 * enclosing object after use.
 * On output this data belongs to the caller and must be freed after use.
 * Within functions this can also be used for data pointers that must be freed.
 */
typedef unsigned char* alloc_bytes;

/**
 * \brief A permanent pointer.
 *
 * \ingroup misc
 * On input it needs to persist through the life of a given context.
 * On output it is guaranteed to live as long as it's context or until it has
 * been explicitly finalized. It must not be freed.
 */
typedef const void* perm_ptr;

/**
 * \brief A transient pointer.
 *
 * \ingroup misc
 * On input this string must only persist for the duration of the function call.
 * On output this string is valid until another call with the given context is
 * performed and must not be freed.
 */
typedef const void* trans_ptr;

/**
 * \brief An allocated pointer.
 *
 * \ingroup misc
 * On input this can represent a place on the stack or heap that will be written
 * to by the called function. Usually accompanied by a length parameter.
 * It can also represent a parameter that will be freed in conjunction with an
 * enclosing object after use.
 * On output this object belongs to the caller and must be freed after use.
 */
typedef void* alloc_ptr;

/**
 * \brief A generic pointer.
 */
typedef void* ptr;

/**
 * \brief A signed type for expressing seconds.
 */
#ifdef __EMSCRIPTEN__
typedef long long sec_t;
#else
typedef long sec_t;
#endif

/**
 * \brief A signed type for expressing milli seconds.
 */
typedef int64_t msec_t;

/**
 * \brief A signed type for expressing micro seconds.
 */
typedef int64_t usec_t;

/**
 * \brief A pointer sized integer type for collections like \ref ruMap or \ref ruList.
 */
typedef intptr_t ru_int;

/**
 * \brief A pointer sized unsigned integer type for collections like \ref ruMap or \ref ruList.
 */
typedef uintptr_t ru_uint;

/**
 * \brief Abstracted version of size_t.
 */
typedef size_t rusize;

/**
 * \ingroup specs
 * \brief An opaque type representing a type specification.
 */
typedef void* ruType;

/**
 * \brief Says to determine the size parameter using strlen or an equivalent.
 */
#define RU_SIZE_AUTO (~0LU)

/**
 * \ingroup memory
 * \brief Signature of a generic free function returning NULL.
 * \param o object to free. Function should insure it's not NULL before freeing
 */
typedef ptr (*ruClearFunc)(ptr o);

/**
 * \ingroup memory
 * \brief Signature of a generic free function.
 * \param o object to free. Function should insure it's not NULL before freeing
 */
typedef void (*ruFreeFunc)(ptr o);

/**
 * \brief Abstracted version of the Posix struct timeval.
 */
typedef struct {
    sec_t sec;
    usec_t usec;
} ruTimeVal;


typedef struct {
    msec_t start;
    msec_t finish;
    msec_t retry;
} ruTryLoop;

/**
 * \brief Returns the build version of this package
 * \ingroup misc
 * @return Version of this package. This string is static and must not be freed.
 */
RUAPI const char* ruVersion(void);

/**
 * \brief Returns an English textual representation of the last error this thread
 * generated.
 *
 * Note, not all functions use this, so this function should only be
 * called after a function that does use it is called, else misleading
 * information will appear.
 * \ingroup errors
 * @return Ephemeral error message. Must be copied if it is to persist.
 */
RUAPI const char* ruLastError(void);

/**
 * \brief Returns a lowercase string representation of the current OS.
 *
 * Currently knows about windows, android, linux, osx, ios and unix.
 * @return os string. Caller should copy this for persistence.
 */
RUAPI perm_chars ruGetOs(void);

/**
 * \brief Returns the name of this host.
 * @return hostname. Caller should free this with \ref ruFree.
 */
RUAPI alloc_chars ruGetHostname(void);

/**
 * \brief Returns the value of the requested environment vartiable or NULL if it is not
 * set.
 * @param variable Variable name to retrieve
 * @return A copy of the value. May not be freed by the caller.
 */
RUAPI trans_chars ruGetenv(const char *variable);

/**
 * \brief Returns true if given string contains a valid 64 bit integer
 * @param numstr String to parse
 * @return parse result overflow returns as false
 */
RUAPI bool ruIsInt64(const char* numstr);

/**
 * \brief Return the current process id
 * @return the current process id
 */
RUAPI ru_pid ruProcessId(void);

/**
 * \brief Indicates that an operation is non blocking. Used by \ref ruRunProg
 */
#define RU_NON_BLOCK (-1)

/**
 * \brief Indicates that an operation that has no timeout. Used by \ref ruRunProg
 */
#define RU_NO_TIMEOUT 0

/**
 * \brief Runs given program
 *
 * Example:
 * ~~~~~{.c}
    const char* command[] = {"/bin/sleep", "3", NULL};
    int ret = ruRunProg(command, RU_NO_TIMEOUT);
 * ~~~~~
 *
 * @param argv program with params to run
 * @param timeout Use \ref RU_NON_BLOCK for non blocking, \ref RU_NO_TIMEOUT for
 *                blocking and > 0 to a timeout in seconds.
 * @return \ref RUE_FORK_FAILED for a fork failure,
 *         \ref RUE_RUN_FAILED for a launch failure,
 *         \ref RUE_TIMEOUT for a timeout,
 *         else the return code of the process
 *         or 0 if it was launched non blocking.
 */
RUAPI int32_t ruRunProg(const char **argv, sec_t timeout);

/**
 * \brief Returns the ISO-639-1 2 letter country code pertaining to the running system,
 * @return The country code. Caller must free with \ref ruFree.
 */
RUAPI alloc_chars ruGetLanguage(void);

/**
 * \brief Returns a quasi ramdom number between 0 and max + offset.
 *
 * This function ORs timevals usec with sec, so it is not designed for
 * cryptographic use.
 * @param max The maximum random value.
 * @param offset An off set to add to the result to move the range.
 * @return A number between offset and offset + max.
 */
RUAPI unsigned long ruSemiRandomNumber(unsigned long max, long offset);

/**
 * \brief Compares 2 version strings
 * This function compares version number strings that are made up of real numbers.
 * The advantage is that the delimiter does not matter. It will work with, for
 * example, 141.01.03, 141:1:3, or even 141A1P3. It also handles
 * mismatched tails so that 141.1.3 will come before 141.1.3.1. Finally NULLs
 * are also properly compared.
 *
 * @param ver1 First version to check
 * @param ver2 Second version to check
 * @return -1, 0 or 1 depending on whether ver1 is less equal or greater ver2
 */
RUAPI int ruVersionComp(trans_chars ver1, trans_chars ver2);

/**
 * \brief Returns a \ref ruTimeVal representing the current time.
 * @param result Pointer to the \ref ruTimeVal structure to populate.
 * @return Return code of the operation or \ref RUE_OK on success.
 */
RUAPI int32_t ruGetTimeVal(ruTimeVal *result);

/**
 * \brief Return the current local time in microseconds since Jan. 1 1970
 * @return Micro seconds since epoch
 */
RUAPI usec_t ruTimeUs(void);

/**
 * \brief Return the current local time in milliseconds since Jan. 1 1970
 * @return Milli seconds since epoch
 */
RUAPI msec_t ruTimeMs(void);

/**
 * \brief Return the current local time in seconds since Jan. 1 1970
 * @return Seconds since epoch
 */
RUAPI sec_t ruTimeSec(void);

/**
 * \brief Checks if given stamp has elapsed.
 * @param stamp microsecond stamp to compare to.
 * @return Return true if now is >= stamp
 */
RUAPI bool ruTimeUsEllapsed(usec_t stamp);

/**
 * \brief Checks if given stamp has elapsed.
 * @param stamp millisecond stamp to compare to.
 * @return Return true if now is >= stamp
 */
RUAPI bool ruTimeMsEllapsed(msec_t stamp);

/**
 * \brief Checks if given stamp has elapsed.
 * @param stamp second stamp to compare to.
 * @return Return true if now is >= stamp
 */
RUAPI bool ruTimeEllapsed(sec_t stamp);

/**
 * \brief Sleeps for the given number of micro seconds.
 * @param microseconds
 */
RUAPI void ruSleepUs(usec_t microseconds);

/**
 * \brief Sleeps for the given number of milli seconds.
 * @param milliseconds
 */
RUAPI void ruSleepMs(msec_t milliseconds);

/**
 * \brief Sleeps for the given number of seconds.
 * @param secs time to sleep
 */
#define ruSleep(secs) ruSleepMs(((msec_t)(secs))*1000)

/**
 * Handles the details of looping delayed until a timeout is reached.
 * Example:
 * ~~~~~{.c}
    ruTryLoop tl;
    ruTryLoopInit(&tl, 10, 8000);

    do {
      // do something
      // break out if done early
    while(!ruTryLoopDone(&tl));
 * ~~~~~
 *
 * @param cycle pointer to a \ref ruTryLoop structure
 * @param retryMs how long \ref ruTryLoopDone should block when not done
 * @param timeoutMs how long \ref ruTryLoopDone should return false after blocking
 */
RUAPI void ruTryLoopInit(ruTryLoop* cycle, msec_t retryMs, msec_t timeoutMs);

/**
 * Blocks and returns whether loop is done or not.
 * \see \ref ruTryLoopInit
 * @param cycle pointer to a \ref ruTryLoop structure
 * @return true if cycle is done or false after blocking for retryMs milliseconds
 */
RUAPI bool ruTryLoopDone(ruTryLoop* cycle);

/**
 * \brief Convert a string representation of time to a time stamp
 * @param dateformat a strptime type format string
 * @param datestr the string representation of the given date time
 * @return given date time in seconds since epoch
 */
RUAPI sec_t ruTimeParse(trans_chars dateformat, trans_chars datestr);

/**
 * \brief UTC version of \ref ruTimeParse
 * @param dateformat a strptime type format string
 * @param datestr the string representation of the given date time
 * @return given date time in seconds since epoch
 */
RUAPI sec_t ruUtcParse(trans_chars dateformat, trans_chars datestr);

/**
 * \brief Return the current local time formatted in given buffer
 * @param format format string for strftime
 * @param len length of the given buffer must include room for the null terminator
 * @param timeStr buffer where formatted null terminated string will be placed
 * @param timesecs datestamp in seconds since epoch or 0 to use current time
 * @return microsecond part of the current time or -1 on error;
 */
RUAPI int ruTimeFormat(trans_chars format, rusize len, alloc_chars timeStr, sec_t timesecs);

/**
 * \brief UTC version of \ref ruTimeFormat
 * @param format format string for strftime
 * @param len length of the given buffer must include room for the null terminator
 * @param timeStr buffer where formatted null terminated string will be placed
 * @param timesecs datestamp in seconds since epoch or 0 to use current time
 * @return microsecond part of the current time or -1 on error;
 */
RUAPI int ruUtcFormat(trans_chars format, rusize len, alloc_chars timeStr, sec_t timesecs);

/**
 * \brief Converts given local time stamp to UTC
 * @param stamp seconds since epoch in local time
 * @return UTC seconds since epoch
 */
RUAPI sec_t ruTimeLocalToUtc(sec_t stamp);

/**
 * \brief Converts given UTC time stamp to local time
 * @param stamp UTC seconds since epoch
 * @return seconds since epoch in local time
 */
RUAPI sec_t ruTimeUtcToLocal(sec_t stamp);

/**
 * \brief Copies the given string and returns the pointer
 * @param in String to duplicate
 * @return Pointer to copied string. Caller must free.
 */
RUAPI ptr ruCharPtrDup(char* in);


/** \cond noworry */
#define ruMacStart do
#define ruMacEnd while(0)
/** \endcond */

/**
 * \brief Frees given resource if set and returns NULL
 * \ingroup memory
 * @param o resource to be freed and if not NULL already
 * @return NULL
 */
RUAPI ptr ruClear(ptr o);

/**
 * \brief Frees given resource and sets paramater to NULL
 * \ingroup memory
 * @param p resource to be freed and NULLed if not NULL already
 */
#define ruFree(p) ruMacStart { if(p) { free((void*)p); p = NULL; } } ruMacEnd

/**
 * \brief Frees given resource and set paramater to new value
 * \ingroup memory
 * @param p resource to be freed before new value is assigned
 * @param n new value to be assigned to freed resource
 */
#define ruReplace(p, n) ruMacStart { if(p) { free((void*)p); } p = n; } ruMacEnd

/**
 * \brief Convenience macro for setting a potentially passed in result pointer
 * and returning a result.
 * \ingroup misc
 */
#define ruRetWithCode(ptr, code, res) ruMacStart { \
    if (ptr) { *ptr = code; } return res; \
    } ruMacEnd

/**
 * \ingroup cleaner
 * Opaque pointer to cleaner object. See \ref cleaner
 */
typedef void* ruCleaner;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <regify-util/errors.h>
#include <regify-util/logging.h>
#include <regify-util/types.h>
#include <regify-util/list.h>
#include <regify-util/thread.h>
#include <regify-util/string.h>
#include <regify-util/map.h>
#include <regify-util/cleaner.h>
#include <regify-util/ini.h>
#include <regify-util/io.h>
#include <regify-util/json.h>
#include <regify-util/kvstore.h>
#include <regify-util/regex.h>
#include <regify-util/html.h>
#if !defined(__ANDROID__) && defined(__linux__) || defined(ITS_OSX) || defined(_WIN32)
#include <regify-util/fam.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if !defined(__ANDROID__)
/**
 * \cond (!android)
 * \brief Used by \ref ruIpAddrs to specify filtering for IPv4 numbers.
 * \ingroup misc
 */
#define RU_IP4 0x1
/**
 * \brief Used by \ref ruIpAddrs to specify filtering for IPv6 numbers.
 * \ingroup misc
 */
#define RU_IP6 0x2
/**
 * \brief Used by \ref ruIpAddrs to specify filtering for IP numbers.
 * \ingroup misc
 */
#define RU_IP (RU_IP4 | RU_IP6)

/**
 * \brief Returns the IP numbers bound to the host
 * \ingroup misc
 * @param ipfilter Whether to filter for IPv4(\ref RU_IP4), IPv6(\ref RU_IP6)
 *                 or both (\ref RU_IP).
 * @return \ref ruList of IP strings. Caller to free with \ref ruListFree
 */
RUAPI ruList ruIpAddrs(int32_t ipfilter);
/** \endcond */
#endif

/**
 * \brief Runs getopt short with given parameters.
 * \ingroup misc
 *
 * It does not process non option arguments or permute argv to place these at
 * the end.
 * @param parms Where the string/string map with the parameters will be placed.
 *              Caller must free with \ref ruMapFree
 * @param opts getopt short definition string
 * @param argc length of argv, as given in main
 * @param argv array of string parameters with the first being the comman itself
 *             as given in main
 * @return \ref RUE_OK on success map will be set
 *         \ref RUE_PARAMETER_NOT_SET when called wrong, no map
 *         \ref RUE_FILE_NOT_FOUND if there were no options, no map
 *         \ref RUE_INVALID_PARAMETER if there were invalid parameters, no map
 */
RUAPI int32_t ruGetOptMap(ruMap* parms, trans_chars opts, int argc, char** argv);

#ifdef _WIN32
/**
 * \brief Runs getopt short with given parameters.
 * \ingroup misc
 *
 * It does not process non option arguments or permute argv to place these at
 * the end.
 * @param parms Where the string/string map with the parameters will be placed.
 *              Caller must free with \ref ruMapFree
 * @param opts getopt short definition string
 * @param wargc length of argv, as given in main
 * @param wargv array of wide string parameters with the first being the comman
 *              itself as given in main
 * @return \ref RUE_OK on success map will be set
 *         \ref RUE_PARAMETER_NOT_SET when called wrong, no map
 *         \ref RUE_FILE_NOT_FOUND if there were no options, no map
 *         \ref RUE_INVALID_PARAMETER if there were invalid parameters, no map
 */
RUAPI int32_t ruGetOptMapW(ruMap* parms, trans_chars opts, DWORD wargc, LPWSTR* wargv);
#endif

/**
 * \defgroup memory Memory Management
 * \brief Abstracted functions pertaining to memory managment.
 * @{
 */
/**
 * \brief Allocate and zero requested memory.
 * @param count Number of elements to allocate.
 * @param ofsize The size of each element to allocate.
 * @return Guarateed to return the requested memory block or the process will
 *         terminate.
 */
RUAPI alloc_ptr ruMallocSize(rusize count, rusize ofsize);

/**
 * \brief Allocate and zero requested memory.
 * @param count Number of elements to allocate
 * @param ctype The type of element to allocate. This will be feed to sizeof().
 * @return Guarateed to return the requested memory block casted to (*ctype)
 *         or the process will terminate.
 */
#define ruMalloc0(count, ctype) (ctype*) ruMallocSize((rusize)(count), sizeof(ctype));

/**
 * \brief Reallocate requested memory without zeroing.
 * @param buf The initial buffer to extend or reallocated.
 * @param count Number of elements to allocate.
 * @param ofsize The size of each element to allocate.
 * @return Guarateed to return the requested memory block or the process will
 *         terminate.
 */
RUAPI alloc_ptr ruReallocSize(alloc_ptr buf, rusize count, rusize ofsize);

/**
 * \brief Reallocate requested memory without zeroing.
 * @param buf The initial buffer to extend or reallocated.
 * @param count Number of elements to allocate.
 * @param ctype The type of element to allocate. This will be feed to sizeof().
 * @return Guarateed to return the requested memory block casted to (*ctype)
 *         or the process will terminate.
 */
#define ruRealloc(buf, count, ctype) (ctype*) ruReallocSize(buf, count, sizeof(ctype))

/**
 * \brief Returns a copy of the given memory block.
 * @param buf The memory area to duplicate.
 * @param size number of bytes to duplicate.
 * @return Guarateed to return the requested memory block or the process will
 *         terminate.
 */
RUAPI alloc_ptr ruMemDup(trans_ptr buf, rusize size);

/**
 * @}
 */

/** \cond noworry */
/*
 * Some convenience Macros
 */

// the ptr < (void*)0xffff is feeble attempt at address validation
#define ruMakeTypeGetter(ctype, magic) \
ctype* ctype ## Get(void* ptr, int32_t* code) { \
    ctype* ret = (ctype*) ptr; \
    if (!ptr) { \
        ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL); \
    } \
    if (ptr < (void*)0xffff || magic != ret->type) { \
        ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL); \
    } \
    ruRetWithCode(code, RUE_OK, ret); \
}

#define ruZeroedStruct(type, var) type var; \
    memset(&(var), 0, sizeof(type))

#define ruMakeTypeGetHeader(ctype) \
ctype* ctype ## Get(void* ptr, int32_t* code)

/** \endcond */

#ifdef __cplusplus
}   /* extern "C" */
#endif /* __cplusplus */

#include <regify-util/logging.h>

#endif //UTIL_REGIFY_UTIL_H
