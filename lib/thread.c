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

ruMakeTypeGetter(Trace, MagicTrace)
ruMakeTypeGetter(Cond, MagicCond)
ruMakeTypeGetter(Mux, MagicMux)
ruMakeTypeGetter(Thr, MagicThr)
ruMakeTypeGetter(tsc, MagicTsc)

static RU_THREAD_LOCAL char pidEnd[64];
RU_THREAD_LOCAL perm_chars logPidEnd = NULL;
static RU_THREAD_LOCAL char threadName[64];
RU_THREAD_LOCAL perm_chars ru_threadName = NULL;
perm_chars staticPidEnd = "]:";
perm_chars procPath = NULL;

//<editor-fold desc="Backtrace">
#if defined(UNWIND) || defined(ILTBacktrace) || defined(STACKWALK)
static Trace* newTrace(trans_chars filePath, int32_t line, trans_chars func,
                       perm_ptr offset, perm_ptr addr) {
    Trace* tr = ruMallocSize(1, sizeof(Trace));
    tr->type = MagicTrace;
    tr->filePath = ruStrDup(filePath);
    tr->file = ruBaseName(tr->filePath);
    tr->line = line;
    tr->func = ruStrDup(func);
    tr->offset = offset;
    tr->addr = addr;
    return tr;
}

static ruTrace freeTrace(ruTrace rt) {
    Trace* tr = TraceGet(rt, NULL);
    if (!tr) return NULL;
    ruFree(tr->filePath);
    ruFree(tr->func);
    ruFree(tr->str);
    tr->file = NULL;
    tr->type = 0;
    tr->offset = 0;
    tr->addr = 0;
    tr->line = 0;
    ruFree(tr);
    return NULL;
}
#endif

#if defined(ILTBacktrace)
#include <backtrace.h>

struct backtrace_state* btState = NULL;

void btErrorCb (void *data, trans_chars msg, int error) {
    if (error == -1) {
        ruWarnLog("If you want backtraces, you have to compile with -g");
    } else {
        ruWarnLogf("Backtrace error %d: %s", error, msg);
    }
}

static int btFullCb (void* data, uintptr_t pc, trans_chars file,
                     int line, trans_chars func) {
    ruList callers = (ruList)data;
    //ruDbgLogf("file: %s line: %d func: %s addr: 0x%p",
    //          file, line, func, pc);
    ruListInsertIdx(callers, 0,
                    newTrace(file, line, func, 0, (perm_ptr)pc));
    return 0;
}

#elif defined(UNWIND)
#define UNW_LOCAL_ONLY
#include <libunwind.h>

static void unwindTo(ruList callers) {
    unw_cursor_t cursor;
    unw_context_t context;

    // Initialize cursor to current frame for local unwinding.
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    // Unwind frames one by one, going up the frame stack.
    while (unw_step(&cursor) > 0) {
        unw_word_t offset = 0, pc = 0;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if (!pc) break;

        char sym[256];
        if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
            //ruDbgLogf("TRACE: %s+0x%lx", sym, offset);
            ruListInsertIdx(callers, 0, newTrace(
                    NULL, 0, sym,
                    (perm_ptr)offset, (perm_ptr)pc));
        } else {
            //ruDbgLogf("TRACE: (null)+0x%lx", offset);
            ruListInsertIdx(callers, 0, newTrace(
                    NULL, 0, NULL,
                    (perm_ptr)offset, (perm_ptr)pc));
        }
    }
}

#elif defined(STACKWALK)
#include <dbghelp.h>

static void stackWalk(ruList callers) {
    static HANDLE process = 0;
    if (!process) {
        process = GetCurrentProcess();
        if (!SymInitialize(process, ruDirName(procPath), true)) {
            ruWarnLogf("SymInitialize failed Ec: %d", GetLastError());
        } else {
            ruDbgLog("SymInitialize succeeded");
        }
        char path[1024];
        char* p = &path[0];
        memset(p, 0, 1024);
        if (SymGetSearchPath(process, p, 1024)) {
            ruDbgLogf("search path: '%s'", p);
        } else {
            ruWarnLogf("SymGetSearchPath failed Ec: %d", GetLastError());
        }
    }

    CONTEXT context;
    RtlCaptureContext(&context);

    ruZeroedStruct(STACKFRAME, stack);
    stack.AddrPC.Mode      = AddrModeFlat;
    stack.AddrStack.Mode   = AddrModeFlat;
    stack.AddrFrame.Mode   = AddrModeFlat;
#ifdef _M_X64
    DWORD machine = IMAGE_FILE_MACHINE_AMD64;
    stack.AddrPC.Offset    = context.Rip;
    stack.AddrStack.Offset = context.Rsp;
    stack.AddrFrame.Offset = context.Rbp;
#elif _M_IX86
    DWORD machine = IMAGE_FILE_MACHINE_I386;
    stack.AddrPC.Offset    = context.Eip;
    stack.AddrStack.Offset = context.Esp;
    stack.AddrFrame.Offset = context.Ebp;
#else
#error "platform not supported!"
#endif

    for(;;) {
        bool result = StackWalk(machine, process, GetCurrentThread(), &stack,
                              &context, NULL, SymFunctionTableAccess,
                              SymGetModuleBase, NULL);
        if(!result) break;

        // get the function name
        perm_chars func = NULL;
        #define max_sym_name (MAX_SYM_NAME * sizeof(char))
        char store[sizeof(SYMBOL_INFO) + max_sym_name];
        PSYMBOL_INFO sym = (PSYMBOL_INFO)store;
        memset(sym, 0, sizeof(store));
        sym->SizeOfStruct = sizeof(SYMBOL_INFO);
        sym->MaxNameLen = MAX_SYM_NAME;
        if (SymFromAddr(process, stack.AddrPC.Offset, NULL, sym)) {
            func = sym->Name;
        }

        // get the line and file name
        ruZeroedStruct(IMAGEHLP_LINE, line_num);
        line_num.SizeOfStruct = sizeof(IMAGEHLP_LINE);
        ruTrace tr = NULL;
        if (SymGetLineFromAddr(process, stack.AddrPC.Offset, NULL, &line_num)) {
            perm_chars file = ruBaseName(line_num.FileName);
            tr = newTrace(file, line_num.LineNumber, func,
                          (perm_ptr)stack.AddrFrame.Offset,
                          (perm_ptr)stack.AddrPC.Offset);
        } else {
            if (sym->Size) {
                tr = newTrace(NULL, 0, func,
                              (perm_ptr)stack.AddrFrame.Offset,
                              (perm_ptr)stack.AddrPC.Offset);
            } else {
                tr = newTrace(NULL, 0, NULL,
                              (perm_ptr)stack.AddrFrame.Offset,
                              (perm_ptr)stack.AddrPC.Offset);
            }
        }
        //ruDbgLogf("addr: %p frame: %p Size: %lu Flags: %x",
        //          stack.AddrPC.Offset, stack.AddrFrame.Offset, sym->Size, sym->Flags);
        ruListInsertIdx(callers, 0, tr);
    }
}
#endif

RUAPI void ruBacktraceInit(perm_chars exePath) {
    procPath = exePath;
#if defined(ILTBacktrace)
    if (!btState) {
        btState = backtrace_create_state(exePath, 1, btErrorCb, NULL);
    }
#endif
}

RUAPI ruList ruBacktrace(int32_t* code) {
    ruList callers =  NULL;
    int32_t ret = RUE_FEATURE_NOT_SUPPORTED;
#if defined(UNWIND) || defined(ILTBacktrace) || defined(STACKWALK)
    callers =  ruListNew(ruTypePtr(freeTrace));
#if defined(ILTBacktrace)
    if (!btState) {
        btState = backtrace_create_state(NULL, 1, btErrorCb, NULL);
    }
    backtrace_full(btState, 0, btFullCb, btErrorCb, callers);
#elif defined(UNWIND)
    unwindTo(callers);
#elif defined(STACKWALK)
    stackWalk(callers);
    ruListRemoveIdxDataTo(callers, -1, NULL); // trace implementation
#endif
    ret = RUE_OK;
#endif
    ruRetWithCode(code, ret, callers);
}

RUAPI void ruTraceLog(trans_chars tag, int32_t skip) {
    ruList callers = ruBacktrace(NULL);
    if (!ruListSize(callers, NULL)) {
        ruVerbLogf("%s ruBacktrace not supported", tag);
        return;
    }
    while(0 < skip--) ruListRemoveIdxDataTo(callers, -1, NULL);
    ruIterator li = ruListIter(callers);
    ruTrace item;
    for(ruIterTo(li, item); li; ruIterTo(li, item)) {
        perm_chars tr = ruTraceStr(item);
        ruVerbLogf("%s %s", tag, tr);
    }
    ruListFree(callers);
}

RUAPI perm_chars ruTraceStr(ruTrace rt) {
    Trace* tr = TraceGet(rt, NULL);
    if (!tr) return NULL;
    if (tr->str) return tr->str;
    if (tr->func) {
        if (tr->file && tr->line) {
            tr->str = ruDupPrintf("%s(%s:%d)",
                                  tr->func, tr->file, tr->line);
        } else {
            tr->str = ruDupPrintf("%s(0x%p)", tr->func, tr->addr);
        }
    } else {
        if (tr->file && tr->line) {
            tr->str = ruDupPrintf("%s:%d", tr->file, tr->line);
        } else {
            tr->str = ruDupPrintf("0x%p", tr->addr);
        }
    }
    return tr->str;
}

RUAPI perm_chars ruTraceFilePath(ruTrace rt) {
    Trace* tr = TraceGet(rt, NULL);
    if (!tr) return NULL;
    return tr->filePath;
}

RUAPI perm_chars ruTraceFileName(ruTrace rt) {
    Trace* tr = TraceGet(rt, NULL);
    if (!tr) return NULL;
    return tr->file;
}

RUAPI perm_chars ruTraceFunc(ruTrace rt) {
    Trace* tr = TraceGet(rt, NULL);
    if (!tr) return NULL;
    return tr->func;
}

RUAPI uint32_t ruTraceLine(ruTrace rt) {
    Trace* tr = TraceGet(rt, NULL);
    if (!tr) return 0;
    return tr->line;
}

RUAPI perm_ptr ruTraceOffset(ruTrace rt) {
    Trace* tr = TraceGet(rt, NULL);
    if (!tr) return 0;
    return tr->offset;
}

RUAPI perm_ptr ruTraceAddr(ruTrace rt) {
    Trace* tr = TraceGet(rt, NULL);
    if (!tr) return 0;
    return tr->addr;
}

//</editor-fold>

//<editor-fold desc="Threading">
#ifdef _WIN32
static DWORD WINAPI threadRunner(LPVOID context) {
    Thr *tc = (Thr *) context;
    DWORD res = 0;
    if (tc->start) {
        if (tc->name) ruThreadSetName(tc->name);
        ruDbgLogf("Starting thread 0x%p", tc);
        tc->exitRes = tc->start(tc->user);
        res = (DWORD)(intptr_t) tc->exitRes;
        tc->finished = true;
        ruDbgLogf("Finished thread 0x%p with 0x%p", tc, tc->exitRes);
    }
    ruFree(tc->name);
    ruThreadSetName(NULL);
    return res;
}
#else
static void* threadRunner(void* context) {
    Thr* tc = (Thr*) context;
    if (tc->start) {
        if (tc->name) ruThreadSetName(tc->name);
        ruDbgLogf("Starting thread 0x%p", tc);
        tc->exitRes = tc->start(tc->user);
        ruDbgLogf("Finished thread 0x%p with 0x%p", tc, tc->exitRes);
        tc->finished = true;
        pthread_exit(tc->exitRes);
    }
    ruFree(tc->name);
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
    ruFree(tc->name);
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

void freePidEnd(void) {
    logPidEnd = NULL;
    pidEnd[0] = '\0';
}

void setPidEnd(void) {
    static int threadcnt = 0;
    freePidEnd();
    if((ru_tid)getpid() != ruThreadGetId()) {
        snprintf(&pidEnd[0], 64, ".%ld]:[thread-%03d]:",
                 ruThreadGetId(), ++threadcnt);
        logPidEnd = &pidEnd[0];
    } else {
        logPidEnd = (char*)staticPidEnd;
    }
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
    freePidEnd();
    ru_threadName = NULL;
    threadName[0] = '\0';
    if (name) {
        snprintf(&pidEnd[0], 64,".%ld]:[%s]:", ruThreadGetId(), name);
        logPidEnd = &pidEnd[0];
        snprintf(&threadName[0], 64, "%s", name);
        ru_threadName = &threadName[0];
    }
}

RUAPI perm_chars ruThreadGetName(void) {
    return ru_threadName;
}

RUAPI ruThread ruThreadCreate(ruStartFunc start, alloc_chars name, void* usrCtx) {
    ruClearError();
    if (!start) {
        ruSetError("%s", "ruStartFunc is required but NULL");
        return NULL;
    }
    Thr* tc = ruMalloc0(1, Thr);
    tc->type = MagicThr;
    tc->start = start;
    tc->user = usrCtx;
    tc->name = name;
    ruDbgLogf("Created thread 0x%p with name: %s", tc, name);
#ifdef _WIN32
    tc->tid = CreateThread(NULL, 0,
                           threadRunner, tc,
                           0, NULL);
    if (!tc->tid) {
        ruSetError("thread creation failed ec: %d", GetLastError());
        threadFree(tc);
        return NULL;
    }
#else
    int32_t ret = pthread_create(&tc->tid, NULL,
                                 threadRunner, tc);
    if (ret) {
        ruSetError("thread creation failed ec: %d", ret);
        threadFree(tc);
        return NULL;
    }
#endif
    return tc;
}

RUAPI ruThread ruThreadCreateBg(ruStartFunc startFunc, alloc_chars name, void* usrCtx) {
    Thr* tc = ruThreadCreate(startFunc, name, usrCtx);
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

//<editor-fold desc="Condition">
RUAPI ruCond ruCondInit(void) {
    ruClearError();
    Cond* cond = ruMalloc0(1, Cond);
    cond->type = MagicCond;
#ifdef _WIN32
    InitializeConditionVariable(&cond->cond);
#else
    int ret;
    if ((ret = pthread_cond_init(&cond->cond, NULL))) {
        ruSetError("mutex init failed ec: %d", ret);
        ruFree(cond);
        return NULL;
    }
#endif
    return (ruCond)cond;
}

RUAPI ruCond ruCondFree(ruCond c) {
    int32_t ret;
    Cond *cond = CondGet(c, &ret);
    if (!cond) return NULL;
    memset(cond, 0, sizeof(Cond));
    ruFree(cond);
    return NULL;
}

RUAPI void ruCondSignal(ruCond c) {
    int32_t ret;
    Cond *cond = CondGet(c, &ret);
    if (!cond) {
        ruAbortf("failed getting condition %d", ret);
        return;
    }
#ifdef _WIN32
    WakeConditionVariable(&cond->cond);
#else
    pthread_cond_signal(&cond->cond);
#endif
}

RUAPI void ruCondWaitTil(ruCond c, ruMutex m, int32_t msTimeout) {
    int32_t ret;
    Mux *mux = MuxGet(m, &ret);
    if (!mux) {
        ruAbortf("failed getting mutex %d", ret);
        return;
    }
    Cond *cond = CondGet(c, &ret);
    if (!cond) {
        ruAbortf("failed getting condition %d", ret);
        return;
    }
#ifdef _WIN32
    if (!msTimeout) msTimeout = INFINITE;
    if (!SleepConditionVariableSRW(&cond->cond, &mux->mux, msTimeout, 0) &&
            GetLastError() != ERROR_TIMEOUT) {
        ruCritLogf("failed SleepConditionVariableSRW error: %d", GetLastError());
    }
#else
    if (msTimeout) {
        ruZeroedStruct(struct timeval, r);
        ruZeroedStruct(struct timespec, request);
        gettimeofday (&r, NULL);
        request.tv_sec = r.tv_sec + (msTimeout / 1000);
        request.tv_nsec = (r.tv_usec * 1000) + (msTimeout * 1000000);
        pthread_cond_timedwait(&cond->cond, &mux->mux, &request);
    } else {
        pthread_cond_wait(&cond->cond, &mux->mux);
    }
#endif
}

RUAPI void ruCondWait(ruCond c, ruMutex m) {
    ruCondWaitTil(c, m, 0);
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
        ruSetError("mutex init failed: %d", GetLastError());
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
