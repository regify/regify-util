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

#ifndef REGIFY_UTIL_THREAD_H
#define REGIFY_UTIL_THREAD_H

#ifdef RUMS
#define RU_THREAD_LOCAL __declspec(thread)
#else
#include <pthread.h>
#ifdef DO_IOS
#define RU_THREAD_LOCAL thread_local
#else
#define RU_THREAD_LOCAL __thread
#endif
#endif

/**
 * \defgroup threading Threading Related
 * \brief These functions abstract threading related aspects.
 * @{
 */

/**
 * Thread Id type
 */
typedef unsigned long ru_tid;

/**
 * Native thread identifier
 */
#ifdef _WIN32
typedef HANDLE ruThreadId;
#else
typedef pthread_t ruThreadId;
#endif

/**
 * Thread Identifier
 */
typedef void* ruThread;

/**
 * Thread Safe Counter
 */
typedef void* ruCount;

/**
 * Signature of a thread starting function.
 */
typedef void* (*ruStartFunc) (void* context);

/**
 * Returns the id of the running thread
 * @return thread id
 */
RUAPI ru_tid ruThreadGetId(void);

/**
 * Sets or clear name of the running thread
 * @param name New name to set or NULL to clear/free the exisiting name.
 */
RUAPI void ruThreadSetName(trans_chars name);

/**
 * Creates a new thread
 * @param start The start function
 * @param context The start function's argument.
 * @return The thread identifier or NULL on failure in which case call \ref ruLastError for details.
 */
RUAPI ruThread ruThreadCreate(ruStartFunc start, void* context);

/**
 * \brief Creates a new thread with background priority
 * @param start The start function
 * @param context The start function's argument.
 * @return The thread identifier or NULL on failure in which case call \ref ruLastError for details.
 */
RUAPI ruThread ruThreadCreateBg(ruStartFunc start, void* context);

/**
 * \brief Whether thraed has finished and is joinable
 * @param tid Thread id of thread to join.
 * @param code Optional where error code will be stored
 * @return true if joinable
 */
RUAPI bool ruThreadFinished(ruThread tid, int32_t* code);

RUAPI ruThreadId ruThreadNativeId(ruThread tid, int32_t* code);

/**
 * Waits for given thread to terminate for tosecs seconds and kills it after that
 *
 * NOTE: Killing is currently not supported on Android.
 * @param tid Thread id of thread to join.
 * @param tosecs Number of seconds to wait before killing the thread.
 * @param exitVal Where the threads return value will be store if not NULL and
 *                thread wasn't killed.
 * @return false if thread timed out
 */
RUAPI bool ruThreadWait(ruThread tid, sec_t tosecs, void** exitVal);

/**
 * Waits for given thread to terminate
 * @param tid Thread id of thread to join.
 * @param exitVal Where the threads return value will be store if not NULL.
 * @return 0 on success
 */
RUAPI int ruThreadJoin(ruThread tid, void** exitVal );

/**
 * Sends a kill signal to the specified thread.
 *
 * NOTE: Currently not supported on Android
 * @param tid Thread id to send kill signal to
 * @return \ref RUE_OK if the given thread id was valid
 */
RUAPI int32_t ruThreadKill(ruThread tid);

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
 * \brief Tries to aquire a lock for the given \ref ruMutex without blocking.
 * @param m The mutex to lock.
 * @return false if the mutex was locked. If true the acquired lock must be unlocked after use.
 */
RUAPI bool ruMutexTryLock(ruMutex m);

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
 * @return NULL
 */
RUAPI ruMutex ruMutexFree(ruMutex m);

/**
 * \brief Returns a new thread safe counter initialized to given value.
 * @param initialCount Value to set at start
 * @return New \ref ruCount free with \ref ruCountFree
 */
RUAPI ruCount ruCounterNew(int64_t initialCount);

/**
 * \brief Increments the counter by given value and returns the new value
 * @param counter Counter in question
 * @param value value to increment counter by
 * @param code Optional where error code will be stored
 * @return The new counter value
 */
RUAPI int64_t ruCounterIncValue(ruCount counter, int64_t value, int32_t* code);

/**
 * Calls \ref ruCounterIncValue without the code parameter
 */
#define ruCounterInc(counter, value) ruCounterIncValue(counter, value, NULL)

/**
 * Calls \ref ruCounterIncValue with 0 to simply return the current value
 */
#define ruCounterRead(counter) ruCounterIncValue(counter, 0, NULL)

/**
 * \brief Sets the counter to given value and returns the previous value
 * @param counter Counter in question
 * @param value new value to set counter to
 * @param code Optional where error code will be stored
 * @return The previous counter value
 */
RUAPI int64_t ruCountSetValue(ruCount counter, int64_t value, int32_t* code);

/**
 * Calls \ref ruCountSetValue without the code parameter
 */
#define ruCountSet(counter, value) ruCountSetValue(counter, value, NULL)

/**
 * \brief Frees given counter
 * @param counter Counter to free
 * @return NULL
 */
RUAPI ruCount ruCountFree(ruCount counter);

/**
 * @}
 */

#endif //REGIFY_UTIL_THREAD_H
