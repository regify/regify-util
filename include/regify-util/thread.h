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
 * Returns the id of the running thread
 * @return thread id
 */
RUAPI long int ruThreadGetId(void);

/**
 * Sets clear clear name of the running thread
 * @param name New name to set or NULL to clear/free the exisiting name.
 */
RUAPI void ruThreadSetName(const char* name);

/**
 * Thread Identifier
 */
typedef void* ruThread;

/**
 * Signature of a thread starting function.
 */
typedef void* (*ruStartFunc) (void* context);

/**
 * Creates a new thread
 * @param start The start function
 * @param context The start function's argument.
 * @return The thread id or 0 on failure in which case call \ref ruLastError for details.
 */
RUAPI ruThread ruThreadCreate(ruStartFunc start, void* context);

/**
 *
 * @param tid thread id to free
 * @return NULL
 */
RUAPI ruThread ruThreadFree(ruThread tid);

RUAPI bool ruThreadFinished(ruThread tid, int32_t* code);

/**
 * Waits for given thread to terminate
 * @param tid Thread id of thread to join.
 * @param exitVal Where the threads return value will be store if not NULL.
 * @return 0 on success
 */
RUAPI int ruThreadJoin(ruThread tid, void** exitVal );

/**
 * Sends a kill signal to the specified thread
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
 * @}
 */

#endif //REGIFY_UTIL_THREAD_H
