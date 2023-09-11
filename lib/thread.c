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

// Mutex debugging instrumentation
#define muxDbg 0

ruMakeTypeGetter(Mux, MagicMux)
ruMakeTypeGetter(Thr, MagicThr)
ruMakeTypeGetter(tsc, MagicTsc)

RU_THREAD_LOCAL char* logPidEnd = NULL;

//<editor-fold desc="Threading">
#ifdef _WIN32
static DWORD WINAPI threadRunner(LPVOID context) {
    Thr *tc = (Thr *) context;
    DWORD res = 0;
    if (tc->start) {
        ru_tid tid = ruThreadGetId();
        ruDbgLogf("Starting thread 0x%p with id: %ld", tc, tid);
        tc->exitRes = tc->start(tc->user);
        res = (DWORD)(intptr_t) tc->exitRes;
        tc->finished = true;
        ruDbgLogf("Finished thread 0x%p with id: %ld", tc, tid);
    }
    ruThreadSetName(NULL);
    return res;
}
#else
static void* threadRunner(void* context) {
    Thr* tc = (Thr*) context;
    if (tc->start) {
        tc->exitRes = tc->start(tc->user);
        tc->finished = true;
        pthread_exit(tc->exitRes);
    }
    ruThreadSetName(NULL);
    return tc->exitRes;
}
#endif

static Thr* threadFree(Thr* tc) {
    if (!tc) return NULL;
    ruDbgLogf("Freeing thread 0x%p", tc);
#ifdef _WIN32
    if (tc->tid) CloseHandle(tc->tid);
#endif
    memset(tc, 0, sizeof(Thr));
    ruFree(tc);
    return NULL;
}

static int32_t threadKill(Thr* tc) {
    int32_t ret = RUE_PARAMETER_NOT_SET;
    if (tc) {
        ruDbgLogf("Killing thread 0x%p", tc);
        ret = RUE_OK;
#ifdef _WIN32
        TerminateThread(tc->tid, 0);
#else
#ifndef __EMSCRIPTEN__
#if defined(__ANDROID__)
        ret = RUE_FEATURE_NOT_SUPPORTED;
#else
        pthread_cancel(tc->tid);
#endif
#endif
#endif
        tc->finished = true;
    }
    return ret;
}

static int threadJoin(Thr* tc, void** exitVal) {
    if (!tc) return RUE_PARAMETER_NOT_SET;
#ifdef _WIN32
    DWORD code = WaitForSingleObject(tc->tid, INFINITE);
    if (code && !tc->finished) {
        ruSetError("thread join failed ec: %d", GetLastError());
        return RUE_GENERAL;
    }
#else
    int32_t code = pthread_join(tc->tid, &tc->exitRes);
    if (code) {
        ruSetError("thread join failed ec: %d", code);
        return RUE_GENERAL;
    }
#endif
    if (exitVal) {
        *exitVal = tc->exitRes;
    }
    return RUE_OK;
}

static bool threadWait(Thr* tc, long tosecs, void** exitVal) {
    long end = ruTimeSec()+tosecs;
    while (end > ruTimeSec()) {
        if (tc->finished) break;
        ruSleepUs(10);
    }

    if (tc->finished) {
        threadJoin(tc, exitVal);
        return true;
    }
    threadKill(tc);
    if (exitVal) *exitVal = NULL;
    return false;
}

RUAPI ru_tid ruThreadGetId(void) {
#if defined(__EMSCRIPTEN__)
    return 0;
#elif defined(__linux__)
    return syscall(SYS_gettid);
#elif defined(_WIN32)
    return GetCurrentThreadId();
#else
    // darwin
    uint64_t tid;
    pthread_threadid_np(NULL, &tid);
    return tid;
#endif
}

RUAPI void ruThreadSetName(trans_chars name) {
    ruFree(logPidEnd);
    if (name) {
        logPidEnd = ruDupPrintf(".%ld]:[%s]:", ruThreadGetId(), name);
    }
}

RUAPI ruThread ruThreadCreate(ruStartFunc start, void* context) {
    ruClearError();
    if (!start) {
        ruSetError("%s", "ruStartFunc is required but NULL");
        return NULL;
    }
    Thr* tc = ruMalloc0(1, Thr);
    tc->type = MagicThr;
    tc->start = start;
    tc->user = context;
    ruDbgLogf("Created thread 0x%p", tc);
#ifdef _WIN32
    tc->tid = CreateThread(NULL, 0,
                           threadRunner, tc,
                           0, NULL);
    if (!tc->tid) {
        ruSetError("thread creation failed ec: %d", GetLastError());
        ruFree(tc);
        return NULL;
    }
#else
    int32_t ret = pthread_create(&tc->tid, NULL,
                                 threadRunner, tc);
    if (ret) {
        ruSetError("thread creation failed ec: %d", ret);
        ruFree(tc);
        return NULL;
    }
#endif
    return tc;
}

RUAPI ruThread ruThreadCreateBg(ruStartFunc startFunc, void* user) {
    Thr* tc = ruThreadCreate(startFunc, user);
    if(!tc) return tc;
    if (tc->tid) {
#ifdef _WIN32
        if (!SetThreadPriority(tc->tid, THREAD_PRIORITY_BELOW_NORMAL)) {
            ruSetError("Failed setting thread priority. %d", GetLastError());
        }
#endif
#ifdef __linux__
        struct sched_param param;
        param.sched_priority = 0;
        int res = pthread_setschedparam(tc->tid, SCHED_BATCH, &param);
        if ( res != 0) {
            ruSetError("Failed setting thread priority. %d", res);
        }
#endif
#ifdef DARWIN
//        sched_param param;
//        long int res;
//        long int policy;
//        res = pthread_getschedparam(ThreadID(tid), @policy, @param);
//
//         long int minPrio = sched_get_priority_min(policy);
//         long int maxPrio = sched_get_priority_max(policy);
//        ruVerbLogf( "Policy: %d Priority: %d", policy, param->sched_priority);
//        ruVerbLogf( "Priorities Min: %d Max: %d", minPrio, maxPrio);
//
//        //SET the priority and/or policy
//        param->sched_priority = minPrio + ((maxPrio-minPrio) / 4);
//        ruVerbLogf( "Setting policy to %d and prio to %d", policy, param->sched_priority);
//        res = pthread_setschedparam(ThreadID(tid), policy, @param);
//        if ( res != 0) {
//          ruWarnLogf( "Failed setting thread priority. %d", GetThreadParamErrMsg(res));
//        }
#endif
    }
    return tc;
}

RUAPI bool ruThreadFinished(ruThread tid, int32_t* code) {
    Thr* tc = ThrGet(tid, code);
    if (tc) return tc->finished;
    return true;
}

RUAPI ruThreadId ruThreadNativeId(ruThread tid, int32_t* code) {
    Thr* tc = ThrGet(tid, code);
    if (!tc) return 0;
    return tc->tid;
}

RUAPI int32_t ruThreadKill(ruThread tid) {
    int32_t ret;
    Thr* tc = ThrGet(tid, &ret);
    if (!tc) return ret;
    ret = threadKill(tc);
    threadFree(tc);
    return ret;
}

RUAPI bool ruThreadWait(ruThread tid, sec_t tosecs, void** exitVal) {
    Thr* tc = ThrGet(tid, NULL);
    if (!tc) return false;
    bool res = threadWait(tc, tosecs, exitVal);
    threadFree(tc);
    return res;
}

RUAPI int ruThreadJoin(ruThread tid, void** exitVal) {
    int32_t ret;
    Thr* tc = ThrGet(tid, &ret);
    if (!tc) return ret;
    ret = threadJoin(tc, exitVal);
    threadFree(tc);
    return ret;
}
//</editor-fold>

//<editor-fold desc="Mutex">
RUAPI ruMutex ruMutexInit(void) {
    ruClearError();
    Mux *mux = ruMalloc0(1, Mux);
    mux->type = MagicMux;
#ifdef _WIN32
    InitializeSRWLock(&mux->mux);
    if (mux->mux.Ptr) {
        ruSetError("mutex init failed: %d\n", GetLastError());
        ruFree(mux);
        return NULL;
    }
#else
    int ret;
    if ((ret = pthread_mutex_init(&mux->mux, NULL))) {
        ruSetError("mutex init failed ec: %d", ret);
        ruFree(mux);
        return NULL;
    }
#endif
    return (ruMutex)mux;
}

RUAPI ruMutex ruMutexFree(ruMutex m) {
    int32_t ret;
    Mux *mux = MuxGet(m, &ret);
    if (!mux) return NULL;
#ifndef _WIN32
    pthread_mutex_destroy(&mux->mux);
#endif
    ruFree(mux->lastCall);
    memset(mux, 0, sizeof(Mux));
    ruFree(mux);
    return NULL;
}

RUAPI bool ruMutexTryLockLoc(ruMutex m, trans_chars filePath, trans_chars func,
                          int32_t line) {
    int32_t ret;
    Mux *mux = MuxGet(m, &ret);
    if (!mux) {
        ruAbortf("failed getting mutex %d", ret);
    }
#ifdef _WIN32
    bool success = TryAcquireSRWLockExclusive(&mux->mux);
#else
    bool success = 0 == pthread_mutex_trylock(&mux->mux);
#endif
    if (success) {
        mux->tlCnt++;
#if muxDbg
        ruReplace(mux->lastCall, ruDupPrintf(
                "TL: %s(%s:%d) l:%d t:%d u:%d", func, ruBaseName((char*)filePath),
                line, mux->lCnt, mux->tlCnt, mux->ulCnt));
#endif
    }
    return success;
}

RUAPI void ruMutexLockLoc(ruMutex m, trans_chars filePath, trans_chars func,
                          int32_t line) {
    int32_t ret;
    Mux *mux = MuxGet(m, &ret);
    if (!mux) {
        ruAbortf("failed getting mutex %d", ret);
    }
#ifdef _WIN32
#if muxDbg
    bool success = false;
    long end = ruTimeSec() + 10;
    while (end > ruTimeSec()) {
        if (TryAcquireSRWLockExclusive(&mux->mux)) {
            success = true;
            break;
        }
        ruSleepUs(10);
    }
    if (!success) {
        ruAbortf("failed locking mutex lastLoc: %s", mux->lastCall);
    }
#else
    AcquireSRWLockExclusive(&mux->mux);
#endif
#else
    pthread_mutex_lock(&mux->mux);
#endif
    mux->lCnt++;
#if muxDbg
    ruReplace(mux->lastCall, ruDupPrintf(
            "L: %s(%s:%d) l:%d t:%d u:%d", func, ruBaseName((char*)filePath),
            line, mux->lCnt, mux->tlCnt, mux->ulCnt));
#endif
}

RUAPI void ruMutexUnlockLoc(ruMutex m, trans_chars filePath, trans_chars func,
                         int32_t line) {
    int32_t ret;
    Mux *mux = MuxGet(m, &ret);
    if (!mux) {
        ruAbortf("failed getting mutex %d", ret);
    }
    mux->ulCnt++;
#if muxDbg
    if (mux->ulCnt != (mux->lCnt + mux->tlCnt)) {
        ruAbortm("Extra release error");
    }
    ruReplace(mux->lastCall, ruDupPrintf(
            "UL: %s(%s:%d) l:%d t:%d u:%d", func, ruBaseName((char*)filePath),
            line, mux->lCnt, mux->tlCnt, mux->ulCnt));
#endif
#ifdef _WIN32
    ReleaseSRWLockExclusive(&mux->mux);
#else
    pthread_mutex_unlock(&mux->mux);
#endif
}
//</editor-fold>

//<editor-fold desc="Thread Safe Counter">
RUAPI ruCount ruCounterNew(int64_t initialCount) {
    tsc* ac = ruMalloc0(1, tsc);
    ac->type = MagicTsc;
    ac->mutex = ruMutexInit();
    ac->count = initialCount;
    return (ruCount)ac;
}

RUAPI int64_t ruCounterIncValue(ruCount counter, int64_t value, int32_t* code) {
    tsc* ac = tscGet(counter, code);
    int64_t val = 0;
    if (!ac) return val;

    ruMutexLock(ac->mutex);
    ac->count += value;
    val = ac->count;
    ruMutexUnlock(ac->mutex);
    return val;
}

RUAPI int64_t ruCountSetValue(ruCount counter, int64_t value, int32_t* code) {
    tsc* ac = tscGet(counter, code);
    int64_t val = 0;
    if (!ac) return val;

    ruMutexLock(ac->mutex);
    val = ac->count;
    ac->count = value;
    ruMutexUnlock(ac->mutex);
    return val;
}

RUAPI ruCount ruCountFree(ruCount counter) {
    tsc* ac = tscGet(counter, NULL);
    if (!ac) return NULL;
    ac->type = 0;
    ac->mutex = ruMutexFree(ac->mutex);
    ac->count = 0;
    ruFree(ac);
    return NULL;
}
//</editor-fold>
