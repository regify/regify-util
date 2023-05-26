// Windows file access monitoring functions
// Copyright 2014 regify S.A.
// works with EnableExplicit
#include "../lib.h"

//#define fam_dbg
#define fam_dbg(fmt, ...) ruLog_(RU_LOG_DBUG, fmt, __VA_ARGS__)

// eventbuffer
#define _FAM_BUF_SIZE  16380 * sizeof(int)

perm_chars Added = "Added";
perm_chars Deleted = "Deleted";
perm_chars Modified = "Modified";
perm_chars RenamedFrom = "Renamed From";
perm_chars RenamedTo = "Renamed To";
perm_chars BAD_DATA = "BAD DATA";

typedef struct famCtx_ famCtx;
typedef struct worker_ worker;
typedef struct fileMon_ fileMon;

struct famCtx_ {
    alloc_chars name;       // the name of the thread
    // internal stuff
    worker* wctx;           // worker thread context
    ruThread wrkThread;     // worker thread id
    ruThreadId wrkTid;      // worker thread native id
    ruThread cbThread;      // callback thread id if user supplied *eventCb
    ruList events;          // event queue
    bool quit;              // flag indicating termination
    bool cbInit;          // true when callback thread is initialized

    // user data
    perm_ptr ctx;           // the users void data pointer
    famHandler eventCb;     // the given user event callback function
};

struct worker_ {
    ruList fileMons;
    bool quit;
    famCtx* mon;
    long int reqCount;
};

struct fileMon_ {
    // Parameters from the caller For ReadDirectoryChangesW().
    int32_t notifyFlags;
    bool recursive;
    alloc_chars dirName;
    worker* worker;

    char* lastName;  // renamed from filename for renames

    // Result of calling CreateFile().
    HANDLE dirHandle;

    // Required parameter For ReadDirectoryChangesW().
    OVERLAPPED* overlapped;

    // Data buffer For the request.
    // Since the memory is allocated by malloc, it will always
    // be aligned As required by ReadDirectoryChangesW().
    ptr buf;   // vector<BYTE>

    // Double buffer strategy so that we can issue a new Read
    // request before we process the current buffer.
    ptr buBuf;  // vector<BYTE>
    DWORD bufSize; // buffer size
};

static void CALLBACK monCompNotify(DWORD evCode, DWORD transferLen,
                                   OVERLAPPED* overlapped);

//<editor-fold desc="callback thread">
static ptr cbRun(ptr ctx) {
    famCtx *mon = (famCtx*) ctx;
    setThreadNameRef(ruDupPrintf("%sCb", mon->name));
    ruInfoLog("Starting");
    msec_t pollTimeout = FAM_POLL_TIMEOUT;
    mon->cbInit = true;

    do {
        famEvent *fe = ruListTryPop(mon->events, pollTimeout, NULL);
        if (!fe) {
            pollTimeout = FAM_POLL_TIMEOUT;
        } else {
            pollTimeout = FAM_QUEUE_TIMEOUT;
            famEventLog(RU_LOG_DBUG, fe, "got event: ");
            if (mon->eventCb) {
                mon->eventCb(fe, mon->ctx);
            }
            fe = famEventFree(fe);
        }
    } while (!mon->quit);

    ruInfoLog("Stopping");
    return NULL;
}

static perm_chars actionToString(int32_t action) {
    switch (action) {
        case FILE_ACTION_ADDED:
            return Added;
        case FILE_ACTION_REMOVED:
            return Deleted;
        case FILE_ACTION_MODIFIED:
            return Modified;
        case FILE_ACTION_RENAMED_OLD_NAME:
            return RenamedFrom;
        case FILE_ACTION_RENAMED_NEW_NAME:
            return RenamedTo;
        default:
            return BAD_DATA;
    }
}
//</editor-fold>

//<editor-fold desc="fileMon">
static fileMon* monNew(worker* wt, trans_chars directory) {
    fileMon* fm = ruMalloc0(1, fileMon);
    fm->worker = wt;
    fm->notifyFlags = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
                      FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE;
    fm->recursive = true;
    // make unix slashes and trim the trailing one
    directory = ruStrTrim(directory, "/", ruTrimEnd, &fm->dirName);
    ruReplace(fm->dirName, ruStrReplace(directory, "\\", "/"));

    fm->overlapped = ruMalloc0(1, OVERLAPPED);
    fm->buf = ruMalloc0(_FAM_BUF_SIZE, char);
    fm->buBuf = ruMalloc0(_FAM_BUF_SIZE, char);
    fm->bufSize = _FAM_BUF_SIZE;

    // The hEvent member is not used when there is a completion
    // function, so it's ok to use it to point to the object.
    fm->overlapped->hEvent = fm;
    return fm;
}

static void monFree(fileMon* fm) {
    // RequestTermination() must have been called successfully.
    if (fm->dirHandle) {
        fam_dbg("dirHandle = %d", fm->dirHandle);
    }
    ruFree(fm->dirName);
    ruFree(fm->buf);
    ruFree(fm->buBuf);
    ruFree(fm->overlapped);
    ruFree(fm);
}

static bool monOpenDir(fileMon* fm) {
    fam_dbg("start dir:%d", fm->dirName);
    // Allow this routine To be called redundantly.
    if (fm->dirHandle) return true;

    fm->dirHandle = CreateFile(fm->dirName,
                               FILE_LIST_DIRECTORY,
                               FILE_SHARE_READ |
                               FILE_SHARE_WRITE |
                               FILE_SHARE_DELETE,
                               NULL,
                               OPEN_EXISTING,
                               FILE_FLAG_BACKUP_SEMANTICS |
                               FILE_FLAG_OVERLAPPED,
                               NULL);

    if (fm->dirHandle == INVALID_HANDLE_VALUE) return false;
    return true;
}

static void monProcNotify(fileMon *fm) {

    byte* pBase = fm->buBuf;

    do {
        FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*)pBase;

        alloc_chars fileName = ruStrFromNUtf16(
                fni->FileName,(int32_t)fni->FileNameLength);
        fam_dbg("fullPath: '%s' FileNameLength: %d action: %x",
                fileName, fni->FileNameLength, fni->Action);
        // flip the slashes in the direction of the base dir
        ruStrByteReplace(fileName, '\\', '/');
        // Handle a trailing backslash, such as for a root directory.
        alloc_chars fullPath = ruPathJoin(fm->dirName, fileName);

        // If it could be a short filename, expand it.
        rusize flen = ruStrLen(fileName);
        // The maximum length of an 8.3 filename is twelve, including the dot.
        if (flen <= 12 && ruStrStr(fileName, "~")) {
            // Convert To the long filename form. Unfortunately, this
            // does Not work For deletions, so it's an imperfect fix.
            alloc_uni wpath = ruStrToUtf16(fullPath);
            DWORD len = GetLongPathNameW(wpath,
                                         NULL, 0);
            if (len) {
                alloc_uni wlongPath = (alloc_uni)ruMalloc0(len, TCHAR);
                len = GetLongPathNameW(wpath,
                                       wlongPath, len);
                if (len) {
                    ruReplace(fullPath, ruStrFromUtf16(wlongPath));
                }
                ruFree(wlongPath);
            }
            ruFree(wpath);
        }
        rusize fsz = ruFileSize(fullPath, NULL);
        fam_dbg("got event: %d %s on file '%s' size: %ld",
                fni->Action, actionToString(fni->Action), fullPath, fsz);
        int famev = 0;
        switch (fni->Action) {
            case FILE_ACTION_ADDED:
            famev = fam_created;
            break;
            case FILE_ACTION_MODIFIED:
            famev = fam_modified;
            break;
            case FILE_ACTION_REMOVED:
            famev = fam_deleted;
            break;
            case FILE_ACTION_RENAMED_OLD_NAME:
            fm->lastName = fullPath;
            break;
            case FILE_ACTION_RENAMED_NEW_NAME:
            famev = fam_moved;
        }
        if (fni->Action == FILE_ACTION_MODIFIED && ruIsDir(fullPath)) {
            famev = fam_attrib;
        }

        if (fni->Action != FILE_ACTION_RENAMED_OLD_NAME) {
            famEvent *ev = ruMalloc0(1, famEvent);
            ev->eventType = famev;
            if (famev == fam_moved) {
                ev->srcPath = fm->lastName;
                ev->dstPath = fullPath;
            } else {;
                ev->srcPath = fullPath;
            }
            famEventLog(RU_LOG_DBUG, ev, "added event: ");
            ruListPush(fm->worker->mon->events, ev);
        }

        if (!fni->NextEntryOffset) break;
        pBase += fni->NextEntryOffset;
    } while (true);
}

static void monBeginRead(fileMon* fm) {
    // This call needs To be reissued after every APC.
    ReadDirectoryChangesW(fm->dirHandle,
                          fm->buf,
                          fm->bufSize,
                          fm->recursive,
                          fm->notifyFlags,
                          NULL,
                          fm->overlapped,
                          monCompNotify);
}

static void CALLBACK monCompNotify(DWORD evCode, DWORD transferLen,
                                   OVERLAPPED* overlapped) {
    fileMon *fm = overlapped->hEvent;
    if (evCode == ERROR_OPERATION_ABORTED) {
        fam_dbg("Shutting down monitor for [%d]", fm->dirName);
        fm->worker->reqCount--;
        monFree(fm);
        return;
    }

    // This might mean overflow? Not sure.
    if (!transferLen) return;

    // We could just Swap back And forth between the two
    // buffers, but this code is easier To understand And Debug.
    if (transferLen > fm->bufSize) transferLen = fm->bufSize;
    memcpy_s(fm->buBuf, transferLen, fm->buf,
             transferLen);

    // Get the new Read issued As fast As possible. The documentation
    // says that the original OVERLAPPED Structure will Not be used
    // again once the completion routine is called.
    monBeginRead(fm);
    monProcNotify(fm);
}

static void monReqTerm(fileMon *fm) {
    CancelIo(fm->dirHandle);
    CloseHandle(fm->dirHandle);
    fm->dirHandle = 0;
}
//</editor-fold>

//<editor-fold desc="worker">
// All functions in worker run in the context of the worker thread.
// One instance of this object is allocated for each instance of famCtx.
// This class is responsible for thread startup, orderly thread shutdown.
static worker* wrkNew(famCtx *pParent) {
    fam_dbg("%s", "start");
    worker* wt = ruMalloc0(1, worker);
    wt->mon = pParent;
    fam_dbg("%s", "end");
    return wt;
}

static worker* wrkFree(worker* wt) {
    ruFree(wt);
    return NULL;
}

static ptr wrkRun(ptr o) {
    worker *wt = (worker*)o;
    setThreadNameRef(ruDupPrintf("%sWrk", wt->mon->name));
    ruInfoLog("starting");
    while (wt->reqCount || !wt->quit) {
        SleepEx(INFINITE, true);
    }
    ruInfoLog("ending");
    return NULL;
}

// Called by QueueUserAPC
static void CALLBACK wrkAddDir(ULONG_PTR o) {
    fileMon* fm = (fileMon*)o;
    if (fm) {
        fam_dbg("start dir:%d", fm->dirName);
    } else {;
        fam_dbg("%s", "start null");
    }

    if (monOpenDir(fm)) {
        fm->worker->reqCount++;
        ruListAppend(fm->worker->fileMons, fm);
        monBeginRead(fm);
    } else {
        monFree(fm);
    }
}

// Called by QueueUserAPC
static void CALLBACK wrkReqTermination(ULONG_PTR o) {
    worker* wt = (worker*)o;
    wt->quit = true;
    ruIterator li = ruListIter(wt->fileMons);
    for (fileMon* mon = ruIterNext(li, fileMon*);
         li; mon = ruIterNext(li, fileMon*)) {
        // Each Request object will delete itself.
        monReqTerm(mon);
    }
    ruListClear(wt->fileMons);
}
//</editor-fold>

//<editor-fold desc="monitor">
static famCtx* famNew(void) {
    fam_dbg("%s", "start");
    famCtx* fctx = ruMalloc0(1, famCtx);
    fctx->events = ruListNew(famEventFreeV);
    fctx->wctx = wrkNew(fctx);
    fam_dbg("returning 0x%x", fctx);
    return fctx;
}

static void famTerm(famCtx* fctx) {
    if (fctx->wrkThread) {
        QueueUserAPC(wrkReqTermination,
                     fctx->wrkTid, (ULONG_PTR) fctx->wctx);
        ruThreadJoin(fctx->wrkThread, NULL);
        fctx->wrkThread = NULL;
    }
}

static famCtx* famFree(famCtx* fctx) {
    fctx->quit = true;
    famTerm(fctx);
    fctx->wctx = wrkFree(fctx->wctx);
    fctx->events = ruListFree(fctx->events);
    ruFree(fctx->name);
    ruFree(fctx);
    return NULL;
}

static void famInit(famCtx* fctx) {
    // Kick off the wctx thread, which will be managed by wctx.
    fctx->wrkThread = ruThreadCreate(wrkRun, fctx->wctx);
    fctx->wrkTid = ruThreadNativeId(fctx->wrkThread, NULL);
    if (fctx->eventCb) {
        fctx->cbThread = ruThreadCreate(cbRun, fctx);
        while (!fctx->cbInit) {
            ruSleepMs(FAM_QUEUE_TIMEOUT);
        }
    }
}

static void famAddDir(famCtx* fctx, trans_chars directory) {
    fam_dbg("start dir: %s", directory);
    if (!fctx->wrkThread) famInit(fctx);

    fileMon* fm = monNew(fctx->wctx, directory);
    QueueUserAPC(wrkAddDir, fctx->wrkThread,
                 (ULONG_PTR) fm);
    fam_dbg("%s", "end");
}
//</editor-fold>

//<editor-fold desc="public">
famObj fam_monitorFilePath(trans_chars filePath, trans_chars name,
                           famHandler eventCallBack, perm_ptr ctx) {
    ruVerbLogf("Getting ready to monitor: %s", filePath);
    famCtx *fctx = famNew();
    fctx->name = ruStrDup(name);
    fctx->eventCb = eventCallBack;
    fctx->ctx = ctx;
    famAddDir(fctx, filePath);
    return fctx;
}

famObj fam_killMonitor(famObj o) {
    if (!o) return NULL;
    famCtx* mon = (famCtx*)o;
    return famFree(mon);
}

bool fam_quit(famObj o) {
    famCtx* fctx = (famCtx*)o;
    if (!fctx) return true;
    return fctx->quit;
}
//</editor-fold>
