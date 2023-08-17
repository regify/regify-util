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
// Linux file access monitoring functions
#include "../lib.h"
#include <poll.h>
#include <sys/inotify.h>

// internal debug logging
#if 01
// no debug
#define fam_dbg(fmt, ...)
#define fam_dbg_inotify_event(i, ev)

#else
// debug logging
#define fam_dbg(fmt, ...) ruLog_(RU_LOG_DBUG, fmt, __VA_ARGS__)
#define fam_dbg_inotify_event(i, ev) ruMacStart { \
    if(ruDoesLog(RU_LOG_VERB)) { \
        fam_log_inotify_event(__FILE__, __func__, __LINE__, i, ev); \
    } \
} ruMacEnd

static void fam_log_inotify_event(const char *file, const char *func, int32_t line,
                                  int i, struct inotify_event* ev) {
    if (ev->len) {
        ruDoLog(RU_LOG_DBUG, file, func, line,
                "i[%03d] ev wd[%d] mask[0x%04x %04x] cookie[%d] len[%d] name[%s]",
                i, ev->wd, ev->mask >> 16, ev->mask & 0xffff,
                ev->cookie, ev->len, ev->name);
    } else {
        ruDoLog(RU_LOG_DBUG, file, func, line,
                "i[%03d] ev wd[%d] mask[0x%04x %04x] cookie[%d]",
                i, ev->wd, ev->mask>>16, ev->mask&0xffff, ev->cookie);
    }
}

#endif

// Linux
#define IN_MY_EVENTS	(IN_ACCESS | IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE | \
                        IN_CLOSE_NOWRITE | IN_OPEN | IN_MOVED_FROM | \
                        IN_MOVED_TO | IN_DELETE | IN_CREATE | IN_DELETE_SELF | \
                        IN_MOVE_SELF)

#define EVENT_SIZE   sizeof(struct inotify_event)
#define BUF_LEN      (4192 * (EVENT_SIZE + 16))

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
    ruFamHandler eventCb; // the given user event callback function
    perm_ptr ctx;       // the users void data pointer
    ruThread tid;       // thread id of fam thread
    // fam thread
    void* buf;          // buffer to store inotify_events in
    ruMap wdPath;       // map of k: watch_descriptor v: fullpath to dir (no /)
    ruMap pathWd;       // map of k: fullpath to dir (no /) v: watch_descriptor
    ruMap cookie;       // map of k: int cookie v: ck objects for moves
    struct pollfd pfd;  // poll descriptor for inotify_events
    // common
    bool hasInit;        // set #True when run thread has initialized all watchdirs finnished
    bool quit;          // set #True to break loop
};

static ptr freeCookie(ptr o) {
    ck* c = (ck*)o;
    ruFree(c->sourcePath);
    return ruClear(c);
}

static ck* newCookie(const char* path) {
    ck* c = ruMalloc0(1, ck);
    c->sourcePath = path;
    c->mtime = ruTimeMs();
    return c;
}

static void trimEnd(char* str, const char* end) {
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
    int32_t wd = inotify_add_watch(fctx->id, fullPath, IN_MY_EVENTS);
    if (wd < 0) {
        ruCritLogf("Failed adding watch on '%s' errno: %d - %s",
                   fullPath, errno, strerror(errno));
        return RUE_CANT_OPEN_FILE;
    }
    alloc_chars pathCopy = copyNoSlash(fullPath);
    ruMapPut(fctx->wdPath, &wd, pathCopy);
    ruMapPut(fctx->pathWd, pathCopy, &wd);
    ruDbgLogf("Added %s with handle %d", fullPath, wd);
    ruFree(pathCopy);
    return RUE_OK;
}

static int32_t fam_watchDir(famCtx *fctx, char* filePath) {
    fam_dbg("Watch: %s", filePath);
    return ruFolderWalk(filePath, RU_WALK_FOLDER_FIRST, watcher, fctx);
}

static int32_t fam_unwatchDir(famCtx* fctx, char* filePath) {
    fam_dbg("Unwatch: %s", filePath);
    // yank list to avoid concurrent modifications
    ruList yankies = ruListNew(NULL);
    int32_t ret;
    int32_t wd;
    perm_chars path = NULL;
    for (ret = ruMapFirst(fctx->wdPath, &wd, &path); ret == RUE_OK;
         ret = ruMapNext(fctx->wdPath, &wd, &path)) {

        if (ruStrStartsWith(path, filePath, NULL)) {
            ruDbgLogf("Removing %s with handle %d", path, wd);
            inotify_rm_watch(fctx->id, wd);
            ruListAppend(yankies, (intptr_t)wd);
            ruMapRemove(fctx->pathWd, path, NULL);
        }
    }


    ruIterator li = ruListIter(yankies);
    for (ru_int wd2 = ruIterNext(li, ru_int);
         li; wd2 = ruIterNext(li, ru_int)) {
        wd = (int32_t)wd2;
        ruMapRemove(fctx->wdPath, &wd2, NULL);
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
    ruMap newWdPaths = ruMapNew(ruTypeInt32(), ruTypeStrFree());
    rusize len = strlen(srcPath);

    int32_t wd;
    perm_chars path = NULL;
    int32_t ret;
    for (ret = ruMapFirst(fctx->wdPath, &wd, &path); ret == RUE_OK;
         ret = ruMapNext(fctx->wdPath, &wd, &path)) {

        if (ruStrStartsWith(path, srcPath, NULL)) {
            alloc_chars newPath = ruDupPrintf("%s%s", destPath, (srcPath + len));
            trimEnd(newPath, "/");
            fam_dbg("Renaming pathWd handle %ld from '%s' to '%s'", wd, path, newPath);
            // store for updating later
            ruMapPut(newWdPaths, &wd, newPath);
            // change the value in the other map now
            ruMapRemove(fctx->pathWd, path, NULL);
            ruMapPut(fctx->pathWd, newPath, &wd);
        }
    }

    for (ret = ruMapFirst(newWdPaths, &wd, &path); ret == RUE_OK;
         ret = ruMapNext(newWdPaths, &wd, &path)) {
        fam_dbg("Updating wdPath map '%ld' with '%s'", wd, path);
        ruMapPut(fctx->wdPath, &wd, path);
    }
    ruMapFree(newWdPaths);
    return RUE_OK;
}

static void fam_handle_moveTo(famCtx* fctx, uint32_t cookie, char* fullPath,
                              struct inotify_event* ev) {
    ruFamEvent* fe = ruFamEventNew(RU_FAM_MOVED, NULL, fullPath);
    // did this file come from somewhere we watch?
    if (!ruMapHas(fctx->cookie, &cookie, NULL)) {
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
    ruMapGet(fctx->cookie, &cookie, &c);
    fe->srcPath = ruStrDup(c->sourcePath);
    fam_dbg("File move from '%s' with cookie [%d] ended up at '%s'",
            c->sourcePath, cookie, fullPath);
    fctx->eventCb(fe, fctx->ctx);
    if (ruMapHas(fctx->pathWd, c->sourcePath, NULL)) {
        if (ruStrEquals(fullPath, "")) {
            // if this was a dir we watched remove all watch handles
            fam_unwatchDir(fctx, (char*)c->sourcePath);
        } else { ;
            // then rename it in our lookup maps
            fam_renameDir(fctx, (char*)c->sourcePath, fullPath);
        }
    }
}

static void fam_processEv(famCtx* fctx, struct inotify_event* ev, int* pollTimeout) {
    // get the start address of the name and interpret it as UTF8
    perm_chars name_ = ev->name;
    perm_chars path_ = NULL;
    int32_t ret = ruMapGet(fctx->wdPath, &ev->wd, &path_);
    if (ret != RUE_OK || !path_) {
        ruVerbLogf("no wdPath entry for handle %d ignoring event path: '%s' ret: %d",
                   ev->wd, path_, ret);
        return;
    }
    char* fullPath = ruPathJoin(path_, name_);
    do {
        if (ev->mask & IN_DELETE_SELF) {
            ret = ruMapGet(fctx->wdPath, &ev->wd, &path_);
            ruDbgLogf("Removing watch from [%s]", path_);
            ruMapRemove(fctx->wdPath, &ev->wd, NULL);
            ruMapRemove(fctx->pathWd, path_, NULL);
            inotify_rm_watch(fctx->id, ev->wd);
            break;
        }

        if (ev->mask & IN_MOVED_FROM) {
            ck* c = newCookie(fullPath);
            ruMapPut(fctx->cookie, &ev->cookie, c);
            *pollTimeout = RU_FAM_QUEUE_TIMEOUT;
            fam_dbg("Initiating file move from %s with cookie [%d] time [%ul]",
                    fullPath, ev->cookie, c->mtime);
            fullPath = NULL;
            break;
        }

        if (ev->mask & IN_MOVED_TO) {
            fam_handle_moveTo(fctx, ev->cookie, fullPath, ev);
            // yank cookie
            ruMapRemove(fctx->cookie, &ev->cookie, NULL);
            break;
        }

        if (ev->mask & IN_CREATE) {
            ruFamEvent* fe = ruFamEventNew(RU_FAM_CREATED,
                                           fullPath, NULL);
            fctx->eventCb(fe, fctx->ctx);
            if (ev->mask & IN_ISDIR) {
                fam_dbg("The directory %s was created.", name_);
                if (RUE_OK != fam_watchDir(fctx, fullPath)) {
                    ruCritLog("aborting fam thread.");
                    fctx->quit = true;
                }
            } else {
                fam_dbg("The file %s was created.", name_);
            }
            break;
        }

        if (ev->mask & IN_ATTRIB) {
            ruFamEvent* fe = ruFamEventNew(RU_FAM_ATTRIB,
                                           fullPath, NULL);
            fctx->eventCb(fe, fctx->ctx);
            if (ev->mask & IN_ISDIR) {
                fam_dbg("The directory %s had a meta data change.", name_);
                if (RUE_OK != fam_watchDir(fctx, fullPath)) {
                    ruCritLog("aborting fam thread.");
                    fctx->quit = true;
                }
            } else {
                fam_dbg("The file %s had a meta data change.", name_);
            }
            break;
        }

        if (ev->mask & IN_MODIFY) {
            if (ev->mask & IN_ISDIR) {
                // we ignore modified dirs
                fam_dbg("The directory %s was modified.", name_);
            } else {
                ruFamEvent* fe = ruFamEventNew(RU_FAM_MODIFIED,
                                               fullPath, NULL);
                fctx->eventCb(fe, fctx->ctx);
                fam_dbg("The file %s was modified.", name_);
            }
            break;
        }

        if (ev->mask & IN_DELETE) {
            ruFamEvent* fe = ruFamEventNew(RU_FAM_DELETED,
                                           fullPath, NULL);
            fctx->eventCb(fe, fctx->ctx);
            if (ev->mask & IN_ISDIR) {
                fam_dbg("The directory %s was deleted.", name_);
            } else {
                fam_dbg("The file %s was deleted.", name_);
            }
            break;
        }

    } while(false);
    ruFree(fullPath);
}

static int32_t fam_runLoop(famCtx* fctx) {
    int pollTimeout = RU_FAM_POLL_TIMEOUT;

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
        uint64_t mtime = ruTimeMs() - RU_FAM_QUEUE_TIMEOUT;
        uint32_t cookie;
        ck* c;
        for (ret = ruMapFirst(fctx->cookie, &cookie, &c); ret == RUE_OK;
             ret = ruMapNext(fctx->cookie, &cookie, &c)) {
            // work with key and/or val
            if (c->mtime < mtime) {
                fam_dbg("Removing event %ld path:%s", cookie, c->sourcePath);
                fam_handle_moveTo(fctx, cookie, "", 0);
                ruListAppend(dels, (intptr_t)cookie);
            }
        }
        ruIterator li = ruListIter(dels);
        for(void* key = ruIterNext(li, char*); li;
                key = ruIterNext(li, char*)) {
            cookie = (uint32_t)(intptr_t)key;
            ruMapRemove(fctx->cookie, &cookie, NULL);
        }
        dels = ruListFree(dels);

        if (ruMapSize(fctx->cookie, NULL) == 0) {
            pollTimeout = RU_FAM_POLL_TIMEOUT;
        } else {
            pollTimeout = RU_FAM_QUEUE_TIMEOUT;
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
            if (ev->mask) {
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
    fctx->wdPath = ruMapNew(ruTypeInt32(), ruTypeStrDup());
    fctx->pathWd = ruMapNew(ruTypeStrDup(), ruTypeInt32());
    fctx->cookie = ruMapNew(ruTypeInt32(), ruTypePtr(freeCookie));

    ruInfoLogf("Opened inotify handle in %s", fctx->topDir);
    int32_t ret = fam_watchDir(fctx, fctx->topDir);
    if (ret == RUE_OK) {
        fam_dbg("Loaded existing directory structure under id: %d.", fctx->id);
        fctx->pfd.fd = fctx->id;
        fctx->pfd.events = POLLIN | POLLPRI;
        fctx->hasInit = true;
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
ruFamCtx ruFamMonitorFilePath(trans_chars filePath, trans_chars threadName,
                              ruFamHandler eventCallBack, perm_ptr ctx) {
    ruVerbLogf("Getting ready to monitor: %s", filePath);
    famCtx* fctx = ruMalloc0(1, famCtx);
    fctx->name = ruStrDup(threadName);
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
    while (!fctx->hasInit && !fctx->quit) {
        ruSleepMs(RU_FAM_QUEUE_TIMEOUT);
    }
    if (fctx->quit) {
        ruFamKillMonitor(fctx);
        return NULL;
    }
    return (ruFamCtx)fctx;
}

ruFamCtx ruFamKillMonitor(ruFamCtx o) {
    famCtx* fctx = (famCtx*)o;
    ruInfoLogf("Request to kill fam thread %x", fctx->tid);
    if (fctx->tid) {
        fctx->quit = true;
        bool res = ruThreadWait(fctx->tid, RU_FAM_KILL_TIMEOUT, NULL);
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

bool ruFamQuit(ruFamCtx o) {
    famCtx* fctx = (famCtx*)o;
    if (!fctx) return true;
    return fctx->quit;
}
