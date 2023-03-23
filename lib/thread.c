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

ruMakeTypeGetter(Mux, MagicMux)
ruMakeTypeGetter(Thr, MagicThr)
ruMakeTypeGetter(tsc, MagicTsc)

RU_THREAD_LOCAL char* logPidEnd = NULL;

#ifdef RUMS
static DWORD WINAPI threadRunner(LPVOID context) {
    Thr *tc = (Thr *) context;
    DWORD res = 0;
    if (tc->start) {
        tc->exitRes = tc->start(tc->user);
        res |= (intptr_t) tc->exitRes;
        tc->finished = true;
        CloseHandle(tc->tid);
    }
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
    return tc->exitRes;
}
#endif

static Thr* threadFree(Thr* tc) {
    if (!tc) return NULL;
    memset(tc, 0, sizeof(Thr));
    ruFree(tc);
    return NULL;
}

static int32_t threadKill(Thr* tc) {
    int32_t ret = RUE_PARAMETER_NOT_SET;
    if (tc) {
        ret = RUE_OK;
#ifdef RUMS
        TerminateThread(tc->tid, 0);
#else
#ifndef __EMSCRIPTEN__
#ifndef SIGKILL
#define SIGKILL 9
#endif
        pthread_kill(tc->tid, SIGKILL);
#endif
#endif
        tc->finished = true;
    }
    return ret;
}

static int threadJoin(Thr* tc, void** exitVal) {
    if (!tc) return RUE_PARAMETER_NOT_SET;
#ifdef RUMS
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

RUAPI ru_uint ruThreadGetId(void) {
#ifdef __linux__
    return syscall(SYS_gettid);
#else
    return GetCurrentThreadId();
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
#ifdef RUMS
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
#ifdef RUMS
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

Mux* ruMuxInit(void) {
    Mux *mx = ruMalloc0(1, Mux);
    mx->type = MagicMux;
#ifdef RUMS
    mx->mux = CreateMutex( NULL, FALSE, NULL);
    if (mx->mux == NULL) {
        ruSetError("mutex init failed: %d\n", GetLastError());
		ruFree(mx);
		return NULL;
    }
#else
    int ret;
    if ((ret = pthread_mutex_init(&mx->mux, NULL))) {
        ruSetError("mutex init failed ec: %d", ret);
        ruFree(mx);
        return NULL;
    }
#endif
    return mx;
}

RUAPI ruMutex ruMutexInit(void) {
    ruClearError();
    return (ruMutex)ruMuxInit();
}

RUAPI ruMutex ruMutexFree(ruMutex m) {
    ruClearError();
    int32_t ret;
    Mux *mux = MuxGet(m, &ret);
    if (!mux) return NULL;
#ifdef RUMS
    CloseHandle(mux->mux);
#else
    pthread_mutex_destroy(&mux->mux);
#endif
    memset(mux, 0, sizeof(Mux));
    ruFree(mux);
    return NULL;
}

RUAPI bool ruMutexTryLock(ruMutex m) {
    ruClearError();
    int32_t ret;
    Mux *mux = MuxGet(m, &ret);
    if (!mux) {
        ruCritLogf("failed getting mutex %d", ret);
        ruAbort();
    }
#ifdef RUMS
    DWORD res = WaitForSingleObject(mux->mux, 0);
    if (res != WAIT_OBJECT_0 && res != WAIT_TIMEOUT) {
        ruCritLogf("failed locking mutex ec:%d", GetLastError());
        ruAbort();
    }
    return res == WAIT_OBJECT_0;
#else
    return 0 == pthread_mutex_trylock(&mux->mux);
#endif
}

void ruMuxLock(Mux *mux) {
#ifdef RUMS
    if (WAIT_OBJECT_0 != WaitForSingleObject(mux->mux, INFINITE)) {
        ruCritLogf("failed locking mutex ec:%d", GetLastError());
        ruAbort();
    }
#else
    pthread_mutex_lock(&mux->mux);
#endif
}

RUAPI void ruMutexLock(ruMutex m) {
    ruClearError();
    int32_t ret;
    Mux *mux = MuxGet(m, &ret);
    if (!mux) {
        ruCritLogf("failed getting mutex %d", ret);
        ruAbort();
    }
    ruMuxLock(mux);
}

void ruMuxUnlock(Mux *mux) {
#ifdef RUMS
    ReleaseMutex(mux->mux);
#else
    pthread_mutex_unlock(&mux->mux);
#endif
}

RUAPI void ruMutexUnlock(ruMutex m) {
    ruClearError();
    int32_t ret;
    Mux *mux = MuxGet(m, &ret);
    if (!mux) {
        ruCritLogf("failed getting mutex %d", ret);
        ruAbort();
    }
    ruMuxUnlock(mux);
}

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
