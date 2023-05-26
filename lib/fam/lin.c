// Linux file access monitoring functions
// Copyright regify
#include "../lib.h"
#include <poll.h>
#include <sys/inotify.h>

//#define fam_dbg
#define fam_dbg(fmt, ...) ruLog_(RU_LOG_DBUG, fmt, __VA_ARGS__)

// Linux
#define IN_MY_EVENTS	IN_ACCESS | IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE | \
                        IN_CLOSE_NOWRITE | IN_OPEN | IN_MOVED_FROM | \
                        IN_MOVED_TO | IN_DELETE | IN_CREATE | IN_DELETE_SELF | \
                        IN_MOVE_SELF

#define EVENT_SIZE   sizeof(struct inotify_event)
#define BUF_LEN      4192 * (EVENT_SIZE + 16)

typedef struct ck_ ck;
struct ck_ {
    const char *sourcePath; // path where the move started from
    uint64_t mtime;    // time in milliseconds the move was started
};

// what we use internally to stash all our stuff
typedef struct famCtx_ famCtx;
struct famCtx_ {
    // calling thread
    char *name;         // the name of the thread
    char *topDir;       // path to the top level directory
    int id;             // the inotify file handle
    famHandler eventCb; // the given user event callback function
    perm_ptr ctx;       // the users void data pointer
    ruThread tid;       // thread id of fam thread
    // fam thread
    void* buf;          // buffer to store inotify_events in
    ruMap wdPath;       // map of k: watch_descriptor v: fullpath to dir (no /)
    ruMap pathWd;       // map of k: fullpath to dir (no /) v: watch_descriptor
    ruMap cookie;       // map of k: int cookie v: ck objects for moves
    struct pollfd pfd;  // poll descriptor for inotify_events
    // common
    bool isInit;        // set #True when run thread has initialized all watchdirs finnished
    bool quit;          // set #True to break loop
};

static void freeCookie(void* o) {
    ck* c = (ck*)o;
    ruFree(c->sourcePath);
    ruFree(c);
}

static ck* newCookie(const char* path) {
    ck* c = ruMalloc0(1, ck);
    c->sourcePath = path;
    c->mtime = ruTimeMs();
    return c;
}

void trimEnd(char* str, const char* end) {
    if (ruStrEndsWith(str, "/", NULL)) {
        *(str+ strlen(str-strlen(end))) = '\0';
    }
}

static alloc_chars copyNoSlash(trans_chars base) {
    alloc_chars out = NULL;
    if (ruStrEndsWith(base, "/", NULL)) {
        out = ruStrNDup(base, ruStrLen(base)-1);
    } else {
        out = ruStrDup(base);
    }
    return out;
}

static int watcher(trans_chars fullPath, bool isDir, void *ctx) {
    famCtx* fctx = (famCtx*)ctx;
    if (!isDir) return RUE_OK;
    ru_int wd = inotify_add_watch(fctx->id, fullPath, IN_MY_EVENTS);
    if ( wd < 0) {
        ruCritLogf("Failed adding watch on '%s' errno: %d - %s",
                   fullPath, errno, strerror(errno));
        return RUE_CANT_OPEN_FILE;
    }
    alloc_chars pathCopy = copyNoSlash(fullPath);
    ruMapPut(fctx->wdPath, (ptr)wd, pathCopy);
    // TODO: memleak on failure?
    ruMapPut(fctx->pathWd, ruStrDup(pathCopy), wd);
    fam_dbg("Added %s with handle %d", pathCopy, wd);
    return RUE_OK;
}

static int32_t fam_watchDir(famCtx *fctx, char* filePath) {
    return ruFolderWalk(filePath, RU_WALK_FOLDER_FIRST, watcher, fctx);
}

static int32_t fam_unwatchDir(famCtx* fctx, char* filePath) {
    fam_dbg("Unwatch: %s", filePath);
    // yank list to avoid concurrent modifications
    ruList yankies = ruListNew(NULL);
    int32_t ret;
    void* key, * val;
    for (ret = ruMapFirst(fctx->wdPath, &key, &val); ret == RUE_OK;
         ret = ruMapNext(fctx->wdPath, &key, &val)) {
        ru_int wd = (ru_int) key;
        char* path = (char*) val;

        if (ruStrStartsWith(path, filePath, NULL)) {
            fam_dbg("Removing %s with handle %d", path, wd);
            inotify_rm_watch(fctx->id, (int)wd);
            ruListAppend(yankies, wd);
            ruMapRemove(fctx->pathWd, path, NULL);
        }
    }

    ruIterator li = ruListIter(yankies);
    for (ru_int wd = ruIterNext(li, ru_int);
         li; wd = ruIterNext(li, ru_int)) {
        ruMapRemove(fctx->wdPath, (void*)wd, NULL);
    }
    yankies = ruListFree(yankies);
    return RUE_OK;
}

static int32_t fam_renameDir(famCtx* fctx, char* srcPath, char* destPath) {
    fam_dbg("Rename:%s to:%s", srcPath, destPath);
    // iterate over wdPath map
    // put wd:newPath entries in newWdPaths
    // rename pathWd path:wd -> newPath:wd
    //
    // iterate over newWdPaths
    // put path map wd:newPath

    // values are transferred to fctx->wdPath, so we do not free it in this map
    ruMap newWdPaths = ruMapNew(ruIntHash, ruIntMatch,
                                NULL, NULL, 64);
    rusize len = strlen(srcPath);

    void* key = NULL, * val = NULL;
    int32_t ret;
    for (ret = ruMapFirst(fctx->wdPath, &key, &val); ret == RUE_OK;
         ret = ruMapNext(fctx->wdPath, &key, &val)) {
        ru_int wd = (ru_int) key;
        char* path = (char*) val;

        if (ruStrStartsWith(path, srcPath, NULL)) {
            char* newPath = ruDupPrintf("%s%s", destPath, (srcPath + len));
            trimEnd(newPath, "/");
            fam_dbg("Renaming pathWd handle %ld from '%s' to '%s'", wd, path, newPath);
            // store for updating later
            // TODO: mem leak on failure?
            ruMapPut(newWdPaths, (void*) wd, (void*) ruStrDup(newPath));
            // change the value in the other map now
            ruMapRemove(fctx->pathWd, path, NULL);
            ruMapPut(fctx->pathWd, newPath, (void*)wd);
        }
    }

    for (ret = ruMapFirst(newWdPaths, &key, &val); ret == RUE_OK;
         ret = ruMapNext(newWdPaths, &key, &val)) {
        fam_dbg("Updating wdPath map '%ld' with '%s'", (ru_int)key, (char*)val);
        ruMapPut(fctx->wdPath, key, val);
    }
    newWdPaths = ruMapFree(newWdPaths);
    return RUE_OK;
}

static void fam_log_inotify_event(const char *file, const char *func, int32_t line,
                                  int i, struct inotify_event* ev) {
    if (ev->len) {
        ruDoLog(RU_LOG_DBUG, file, func, line,
                "i[%03d] ev wd[%d] mask[%04x %04x] cookie[%d] len[%d] name[%s]",
                i, ev->wd, ev->mask >> 16, ev->mask & 0xffff,
                ev->cookie, ev->len, ev->name);
    } else {
        ruDoLog(RU_LOG_DBUG, file, func, line,
                "i[%03d] ev wd[%d] mask[%04x %04x] cookie[%d]",
                i, ev->wd, ev->mask>>16, ev->mask&0xffff, ev->cookie);
    }
}

#define fam_dbg_inotify_event(i, ev) ruMacStart { \
    if(ruDoesLog(RU_LOG_VERB)) { \
        fam_log_inotify_event(__FILE__, __func__, __LINE__, i, ev); \
    } \
} ruMacEnd

static void fam_handle_moveTo(famCtx* fctx, uint32_t cookie, char* fullPath,
                              struct inotify_event* ev) {
    famEvent* fe = famEventNew(fam_moved, NULL, fullPath);
    // did this file come from somewhere we watch?
    if (!ruMapHas(fctx->cookie, (ru_int)cookie, NULL)) {
        fam_dbg("File move to is missing cookie [%d]", cookie);
        fctx->eventCb(fe, fctx->ctx);
        if (ev != 0 && ev->mask & IN_ISDIR) {
            fam_dbg("The directory %s was moved in.", ev->name);
            if (RUE_OK != fam_watchDir(fctx, fullPath)) {
                ruCritLog("aborting fam thread.");
                fctx->quit = true;
            }
        } else { ;
            fam_dbg("The file %s was moved in.", ev->name);
        }
        return;
    }

    // is this a directory we watch
    ck* c = NULL;
    ruMapGet(fctx->cookie, (ru_int)cookie, &c);
    fe->srcPath = ruStrDup(c->sourcePath);
    fam_dbg("File move from %s with cookie [%d] ended up at %s",
            c->sourcePath, cookie, fullPath);
    fctx->eventCb(fe, fctx->ctx);
    if (ruMapHas(fctx->pathWd, (void*)c->sourcePath, NULL)) {
        if (ruStrCmp(fullPath, "") == 0) {
            // if this was a dir we watched remove all watch handles
            fam_unwatchDir(fctx, (char*)c->sourcePath);
        } else { ;
            // then rename it in our lookup maps
            fam_renameDir(fctx, (char*)c->sourcePath, fullPath);
        }
    }
}

static void fam_processEv(famCtx* fctx, struct inotify_event* ev, int* pollTimeout) {
    // get the start address of the name and interprete it as UTF8
    perm_chars name_ = ev->name;
    perm_chars path_ = NULL;
    int32_t ret = ruMapGet(fctx->wdPath, (ru_int)ev->wd, &path_);
    if (ret != RUE_OK || !path_) {
        ruVerbLogf("no wdPath entry for handle %d ignoring event", ev->wd);
        return;
    }
    char* fullPath = ruPathJoin(path_, name_);
    do {
        if (ev->mask & IN_DELETE_SELF) {
            ret = ruMapGet(fctx->wdPath, (ru_int)ev->wd, &path_);
            fam_dbg("Removing watch from [%s]", path_);
            ruMapRemove(fctx->wdPath, (ru_int)ev->wd, NULL);
            ruMapRemove(fctx->pathWd, path_, NULL);
            inotify_rm_watch(fctx->id, ev->wd);
            break;
        }

        if (ev->mask & IN_MOVED_FROM) {
            ck* c = newCookie(fullPath);
            ruMapPut(fctx->cookie, (ru_int)ev->cookie, c);
            *pollTimeout = FAM_QUEUE_TIMEOUT;
            fam_dbg("Initiating file move from %s with cookie [%d] time [%ul]",
                    fullPath, ev->cookie, c->mtime);
            fullPath = NULL;
            break;
        }

        if (ev->mask & IN_MOVED_TO) {
            fam_handle_moveTo(fctx, ev->cookie, fullPath, ev);
            // yank cookie
            ruMapRemove(fctx->cookie, (ru_int)ev->cookie, NULL);
            break;
        }

        if (ev->mask & IN_CREATE) {
            famEvent* fe = famEventNew(fam_created,
                                       fullPath, NULL);
            fctx->eventCb(fe, fctx->ctx);
            if (ev->mask & IN_ISDIR) {
                fam_dbg("The directory %s was created.", name_);
                if (RUE_OK != fam_watchDir(fctx, fullPath)) {
                    ruCritLog("aborting fam thread.");
                    fctx->quit = true;
                }
            } else { ;
                fam_dbg("The file %s was created.", name_);
            }
            break;
        }

        if (ev->mask & IN_ATTRIB) {
            famEvent* fe = famEventNew(fam_attrib,
                                       fullPath, NULL);
            fctx->eventCb(fe, fctx->ctx);
            if (ev->mask & IN_ISDIR) {
                fam_dbg("The directory %s had a meta data change.", name_);
                if (RUE_OK != fam_watchDir(fctx, fullPath)) {
                    ruCritLog("aborting fam thread.");
                    fctx->quit = true;
                }
            } else { ;
                fam_dbg("The file %s had a meta data change.", name_);
            }
            break;
        }

        if (ev->mask & IN_MODIFY) {
            if (ev->mask & IN_ISDIR) {
                // we ignore modified dirs
                fam_dbg("The directory %s was modified.", name_);
            } else { ;
                famEvent* fe = famEventNew(fam_modified,
                                           fullPath, NULL);
                fctx->eventCb(fe, fctx->ctx);
                fam_dbg("The file %s was modified.", name_);
            }
            break;
        }

        if (ev->mask & IN_DELETE) {
            famEvent* fe = famEventNew(fam_deleted,
                                       fullPath, NULL);
            fctx->eventCb(fe, fctx->ctx);
            if (ev->mask & IN_ISDIR) {
                fam_dbg("The directory %s was deleted.", name_);
            } else { ;
                fam_dbg("The file %s was deleted.", name_);
            }
            break;
        }

    } while(false);
    ruFree(fullPath);
}

static int32_t fam_runLoop(famCtx* fctx) {
    int pollTimeout = FAM_POLL_TIMEOUT;

    while (!fctx->quit) {
        msec_t pollEnd = ruTimeMs() + pollTimeout;
        int32_t numReady;
        do {
            numReady = poll(&fctx->pfd, 1, 1);
            if (numReady == -1) {
                ruCritLog("Poll failed");
                return RUE_GENERAL;
            }
            if (fctx->quit) {
                ruVerbLog("quitting");
                return RUE_OK;
            }
        } while (!ruTimeMsEllapsed(pollEnd));

        // clear cookies older than 1 second
        ruList dels = ruListNew(NULL);
        int32_t ret;
        uint64_t mtime = ruTimeMs() - FAM_QUEUE_TIMEOUT;
        void* key = NULL, * val = NULL;
        for (ret = ruMapFirst(fctx->cookie, &key, &val); ret == RUE_OK;
             ret = ruMapNext(fctx->cookie, &key, &val)) {
            // work with key and/or val
            ck* c = (ck*) val;
            if (c->mtime < mtime) {
                ru_int cookie = (ru_int) key;
                fam_dbg("Removing event %ld path:%s", cookie, c->sourcePath);
                fam_handle_moveTo(fctx, cookie, "", 0);
                ruListAppendPtr(dels, key);
            }
        }
        ruIterator li = ruListIter(dels);
        for(void* key = ruIterNext(li, char*); li;
                key = ruIterNext(li, char*)) {
            ruMapRemove(fctx->cookie, key, NULL);
        }
        dels = ruListFree(dels);

        if (ruMapSize(fctx->cookie, NULL) == 0) {
            pollTimeout = FAM_POLL_TIMEOUT;
        } else {
            pollTimeout = FAM_QUEUE_TIMEOUT;
        }
        if (numReady != 1) continue;

        //fam_dbg("Reading from inotify handle")
        rusize_s len = read(fctx->id, fctx->buf, BUF_LEN);
        if (len < 0) {
            ruCritLog("Failed to read inotify file handle");
        } else { ;
//            fam_dbg("Read %d bytes", len);
        }
        rusize i = 0;
        while (i < len) {
            struct inotify_event* ev = (struct inotify_event*)((long)fctx->buf + i);
            //fam_dbg("Mem[" + Hex(*ev) + "] [" + MemoryToHex(*ev, #BUF_LEN - i + 1, " ") + "]")
            fam_dbg_inotify_event(i, ev);
            if (ev->mask != 0) {
                fam_processEv(fctx, ev, &pollTimeout);
            }
            i += EVENT_SIZE + ev->len;
//            fam_dbg("buffer increment %d", EVENT_SIZE + ev->len);
        }
    }
    return RUE_OK;
}


static void* fam_runThread(void* ctx) {
    famCtx* fctx = (famCtx*) ctx;
    ruThreadSetName(fctx->name);
    fctx->buf = ruMallocSize(BUF_LEN, 1);
    fctx->wdPath = ruMapNew(ruIntHash, ruIntMatch,
                            NULL, free, 64);
    fctx->pathWd = ruMapNewString(free, NULL);
    fctx->cookie = ruMapNew(ruIntHash, ruIntMatch,
                            NULL, freeCookie, 64);

    ruInfoLogf("Opened inotify handle in %s", fctx->topDir);
    int32_t ret = fam_watchDir(fctx, fctx->topDir);
    if (ret == RUE_OK) {
        fam_dbg("Loaded existing directory structure %d.", fctx->id);
        fctx->pfd.fd = fctx->id;
        fctx->pfd.events = POLLIN | POLLPRI;
        fctx->isInit = true;
        // main loop
        ret = fam_runLoop(fctx);
        // done
    } else {
        ruCritLog("aborting fam thread.");
        fctx->quit = true;
    }
    // clean up
    ruFree(fctx->buf);
    fctx->wdPath = ruMapFree(fctx->wdPath);
    fctx->pathWd = ruMapFree(fctx->pathWd);
    fctx->cookie = ruMapFree(fctx->cookie);
    ruInfoLog("Fam thread quitting.");
    ruThreadSetName(NULL);
    return (void*)(ru_int)ret;
}

static famCtx* fam_freeCtx(famCtx* fctx) {
    if (!fctx) return NULL;
    close(fctx->id);
    ruFree(fctx->name);
    ruFree(fctx->topDir);
    ruFree(fctx);
    return NULL;
}

// public functions
famObj fam_monitorFilePath(trans_chars filePath, trans_chars name,
                            famHandler eventCallBack, perm_ptr ctx) {
    ruVerbLogf("Getting ready to monitor: %s", filePath);
    famCtx* fctx = ruMalloc0(1, famCtx);
    fctx->name = ruStrDup(name);
    fctx->topDir = ruStrDup(filePath);
    fctx->id = inotify_init();
    fctx->eventCb = eventCallBack;
    fctx->ctx = ctx;
    fctx->tid = ruThreadCreate(fam_runThread, (void*) fctx);
    if (!fctx->tid) {
        ruCritLogf("Failed to spawn fam thread ec: %s", ruLastError());
        return fam_freeCtx(fctx);
    }
    // wait for thread to initialize
    while (!fctx->isInit && !fctx->quit) {
        ruSleepMs(FAM_QUEUE_TIMEOUT);
    }
    if (fctx->quit) {
        fam_killMonitor(fctx);
        return NULL;
    }
    return (famObj)fctx;
}

famObj fam_killMonitor(famObj ctx) {
    famCtx* fctx = (famCtx*)ctx;
    ruInfoLogf("Request to kill fam thread %x", fctx->tid);
    if (fctx->tid) {
        fctx->quit = true;
        bool res = ruThreadWait(fctx->tid, FAM_KILL_TIMEOUT, NULL);
        if (res) {
            ruInfoLogf("Fam thread %s has been shut down", fctx->name);
        } else {
            ruWarnLogf("Fam thread %s timed out and was killed", fctx->name);
        }
    } else {
        ruWarnLogf("Fam thread %s is invalid", fctx->name);
    }
    fam_freeCtx(fctx);
    return NULL;
}

bool fam_quit(famObj ctx) {
    famCtx* fctx = (famCtx*)ctx;
    if (!fctx) return true;
    return fctx->quit;
}
