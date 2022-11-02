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

ruMakeTypeGetter(Mux, MuxMagic)
ruMakeTypeGetter(Thr, MagicThr)

RU_THREAD_LOCAL char* logPidEnd = NULL;

static void* threadRunner(void* context) {
    Thr* tc = (Thr*) context;
    if (tc->start) {
        tc->exitRes = tc->start(tc->user);
        tc->finished = true;
        pthread_exit(tc->exitRes);
    }
    return tc->exitRes;
}

RUAPI long int ruThreadGetId(void) {
#ifdef __EMSCRIPTEN__
    return 0;
#else
    return syscall(SYS_gettid);
#endif
}

RUAPI void ruThreadSetName(const char* name) {
    ruFree(logPidEnd);
    if (name) {
        logPidEnd = ruDupPrintf(".%ld]:[%s]", ruThreadGetId(), name);
    }
}

RUAPI ruThread ruThreadCreate(ruStartFunc start, void* context) {
    ruClearError();
    Thr* tc = ruMalloc0(1, Thr);
    tc->start = start;
    tc->user = context;
    int32_t ret = pthread_create(&tc->tid, NULL,
                                 threadRunner, tc);
    if (ret) {
        ruSetError("thread creation failed ec: %d", ret);
        ruFree(tc);
        return NULL;
    }
    return tc;
}

RUAPI ruThread ruThreadFree(ruThread tid) {
    Thr* tc = ThrGet(tid, NULL);
    if (!tc) return NULL;
    if (!tc->finished) {
#ifndef __EMSCRIPTEN__
        ruThreadKill(tc);
#endif
    }
    ruFree(tc);
    return NULL;
}

RUAPI bool ruThreadFinished(ruThread tid, int32_t* code) {
    Thr* tc = ThrGet(tid, code);
    if (tc) return tc->finished;
    return true;
}

RUAPI int32_t ruThreadKill(ruThread tid) {
    int32_t code;
    Thr* tc = ThrGet(tid, &code);
    if (tc) {
#ifndef __EMSCRIPTEN__
        pthread_kill(tc->tid, SIGKILL);
#endif
    }
    return code;
}

RUAPI int ruThreadJoin(ruThread tid, void** exitVal ) {
    int32_t code;
    Thr* tc = ThrGet(tid, &code);
    if (!tc) return code;
    code = pthread_join(tc->tid, &tc->exitRes);
    if (code) {
        ruSetError("thread join failed ec: %d", code);
        return RUE_GENERAL;
    }
    if (exitVal) {
        *exitVal = tc->exitRes;
    }
    return RUE_OK;
}

RUAPI ruMutex ruMutexInit(void) {
    ruClearError();
    Mux *mx = ruMalloc0(1, Mux);
    mx->type = MuxMagic;
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
    return (ruMutex)mx;
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

RUAPI void ruMutexLock(ruMutex m) {
    ruClearError();
    int32_t ret;
    Mux *mux = MuxGet(m, &ret);
    if (!mux) {
        ruCritLogf("failed getting mutex %d", ret);
        ruAbort();
    }
#ifdef RUMS
    if (WAIT_OBJECT_0 != WaitForSingleObject(mux->mux, INFINITE)) {
        ruCritLog("failed unlocking mutex");
        ruAbort();
    }
#else
    pthread_mutex_lock(&mux->mux);
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
#ifdef RUMS
    ReleaseMutex(mux->mux);
#else
    pthread_mutex_unlock(&mux->mux);
#endif
}

