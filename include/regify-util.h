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
 * \ref string, over collections like \ref list or \ref map to \ref logging,
 * \ref regex and abstracted storage such as \ref kvstore. There are also \ref
 * io utilities.
 * It is designed to run on Unix derivatives (Linux, Mac OSX tested), Windows,
 * Android and iOS.
 * All char* input/output is expected to be valid UTF-8.
 *
 * \defgroup misc Miscellaneous Functions
 * \brief Abstracted functions pertaining to other areas.
 *
 */
#ifndef UTIL_REGIFY_UTIL_H
#define UTIL_REGIFY_UTIL_H

/* for our error codes only */
#include <stdlib.h>
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
#if defined(WINDOWS) || defined(WIN32) || defined(__BORLANDC__) || defined(__EMSCRIPTEN__)
    #ifndef u_int8_t
        typedef unsigned char u_int8_t;
    #endif
    #ifndef u_int32_t
        typedef unsigned int u_int32_t;
    #endif
#endif
/** \endcond */

#if defined(WINDOWS) || defined(WIN32) || defined(__BORLANDC__)
    /** \cond noworry */
    #ifdef _MSC_VER
        // running non posix
        #define RUMS
    #endif

    #ifdef RUMS
        #pragma comment(lib, "Ws2_32.lib")
    #endif
    #include <windows.h>

    #ifndef int32_t
        typedef int int32_t;
    #endif
    #ifndef int64_t
        typedef long long int64_t;
    #endif
    #ifndef u_int64_t
        typedef unsigned long long u_int64_t;
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
            u_long* serialNo, u_long* maxCompLen, u_long* fsFlags,
            char** volumeName, char** fsName);
#else
    /* for folderwalk */
    #include <dirent.h>

    /**
     * \brief Abstracted version of size_t but signed
     */
    typedef signed long rusize_s;

#endif

#ifdef RUMS
#define RU_THREAD_LOCAL __declspec(thread)
#else
#ifdef DO_IOS
#define RU_THREAD_LOCAL thread_local
#else
#define RU_THREAD_LOCAL __thread
#endif
#endif


/**
 * \brief Abstracted version of size_t.
 * \ingroup misc
 */
typedef size_t rusize;

/**
 * \brief Abstracted version of the Posix struct timeval.
 * \ingroup misc
 */
typedef struct {
    long sec;
    long usec;
} ruTimeVal;

/** \cond noworry */
#define ruMacStart do
#define ruMacEnd while(0)
/** \endcond */

/**
 * \brief Frees given resource and set paramater to NULL
 * \ingroup memory
 * @param p resource to be freed and NULLed if not NULL already
 */
#define ruFree(p) ruMacStart { if(p) { free((void*)p); } p = NULL; } ruMacEnd

/**
 * \brief Convenience macro for setting a potentially passed in result pointer
 * and returning a result.
 * \ingroup misc
 */
#define ruRetWithCode(ptr, code, res) ruMacStart { \
    if (ptr) { *ptr = code; } return res; \
    } ruMacEnd

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include <regify-util/errors.h>
#include <regify-util/list.h>
#include <regify-util/logging.h>
#include <regify-util/string.h>
#include <regify-util/map.h>
#include <regify-util/cleaner.h>
#include <regify-util/io.h>
#include <regify-util/kvstore.h>
#include <regify-util/regex.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef unsigned char uchar;

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
 * \defgroup threading Threading Related
 * \brief These functions abstract threading related aspects.
 * @{
 */
 /**
  * \brief Opaque object abstracting object locking.
  */
typedef void* ruMutex;

/**
 * \brief Initialize a new \ref ruMutex
 * @return The new mutex or NULL in which case call \ref ruLastError for details.
 */
RUAPI ruMutex ruMutexInit(void);

/**
 * \brief Aquire a lock for the given \ref ruMutex blocking until it is given.
 * @param m The mutex to lock.
 */
RUAPI void ruMutexLock(ruMutex m);

/**
 * \brief Return the aquired mutex lock so other threads can use it.
 * @param m The mutex to unlock.
 */
RUAPI void ruMutexUnlock(ruMutex m);

/**
 * \brief Free up given \ref ruMutex object.
 * @param m The mutex to free.
 * @}
 */
RUAPI void ruMutexFree(ruMutex m);


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
RUAPI void* ruMallocSize(rusize count, rusize ofsize);

/**
 * \brief Allocate and zero requested memory.
 * @param count Number of elements to allocate
 * @param ctype The type of element to allocate. This will be feed to sizeof().
 * @return Guarateed to return the requested memory block casted to (*ctype)
 *         or the process will terminate.
 */
#define ruMalloc0(count, ctype) (ctype*) ruMallocSize(count, sizeof(ctype));

/**
 * \brief Reallocate requested memory without zeroing.
 * @param buf The initial buffer to extend or reallocated.
 * @param count Number of elements to allocate.
 * @param ofsize The size of each element to allocate.
 * @return Guarateed to return the requested memory block or the process will
 *         terminate.
 */
RUAPI void* ruReallocSize(void *buf, rusize count, rusize ofsize);

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
 * @}
 */
RUAPI void* ruMemDup(void *buf, rusize size);


/**
 * \ingroup misc
 * @{
 */

/**
 * \brief Returns a lowercase string representation of the current OS.
 *
 *
 * Currently knows about windows, android, linux, osx, ios and unix.
 * @return os string. Caller should copy this for persistence.
 */
RUAPI const char* ruGetOs(void);

/**
 * \brief Returns the name of this host.
 * @return hostname. Caller should free this with \ref ruFree.
 */
RUAPI char* ruGetHostname(void);

/**
 * \brief Returns the value of the requested environment vartiable or NULL if it is not
 * set.
 * @param variable Variable name to retrieve
 * @return A copy of the value. May not be freed by the caller.
 */
RUAPI const char* ruGetenv(const char *variable);

/**
 * \brief Returns a \ref ruTimeVal representing the current time.
 * @param result Pointer to the \ref ruTimeVal structure to populate.
 * @return Return code of the operation or \ref RUE_OK on success.
 */
RUAPI int32_t ruGetTimeVal(ruTimeVal *result);

/**
 * \brief Returns the ISO-639-1 2 letter country code pertaining to the running system,
 * @return The country code. Caller must free with \ref ruFree.
 */
RUAPI char* ruGetLanguage(void);

/**
 * \brief Sleeps for the given number of micro seconds.
 * @param microseconds
 */
RUAPI void ruUsleep(unsigned long microseconds);

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
 * @}
 */

/** \cond noworry */
/*
 * Some convenience Macros
 */

// the ptr < (void*)1000 is feeble attempt at address validation
#define ruMakeTypeGetter(ctype, magic) \
ctype* ctype ## Get(void* ptr, int32_t* code) { \
    ctype* ret = (ctype*) ptr; \
    if (!ptr) { \
        ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL); \
    } \
    if (ptr < (void*)1000 || magic != ret->type) { \
        ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL); \
    } \
    ruRetWithCode(code, RUE_OK, ret); \
}
/** \endcond */

#ifdef __cplusplus
}   /* extern "C" */
#endif /* __cplusplus */

#include <regify-util/logging.h>

#endif //UTIL_REGIFY_UTIL_H
