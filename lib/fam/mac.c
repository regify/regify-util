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
// Darwin file access monitoring functions
#include "../lib.h"
#include <CoreServices/CoreServices.h>

//#define fam_dbg
#define fam_dbg(fmt, ...) ruLog_(RU_LOG_DBUG, fmt, __VA_ARGS__)

// File renames and deletes due to moves out of our watched area must be handled
// by the CFRunloop and a cleaner thread. We use an array of maps to store file
// moves in. These are accessed in a ring fashion to facilitate event timeouts.
// The cleaner thread always works on now+2 while the CFRunloop operates on now
// and now-1.
// The main fam runloop populates (k:inode v:old filename) the current (now) out
// of 4 maps with the moveout event (kFSEv..Renamed no stat). It also removes
// entries from the current and the last map when it receives the movein event
// (kFSEv..Renamed and stat) and fires RU_FAM_MOVED events.
// The cleaner thread periodically iterates over the current+2 map and cleans
// it out firing RU_FAM_DELETED events for all the entries still left.
// We need 4 buckets because events may be in 2 of them if we added 1 just before
// the end of the last bucket and the next in this bucket. We also want a
// bucketMillies delay before renames become deletes.
// bucketMillies/1000 should be >= FSEventLatencySecs
#define moveRingBuckets  4
#define bucketMillies  1000
#define FSEventLatencySecs  1.0
// only used internally
#define fam_ignore  0x100

typedef struct {
    alloc_chars name;       // the name of the thread
    alloc_chars topDir;     // path to the top level directory
    ruThread tid;           // thread id of fam thread
    ruThread ctid;          // thread id of cleaner thread

    ruFamHandler eventCb;   // the given user event callback function
    perm_ptr ctx;           // the users void data pointer

    // source path of a create|rename event that we didn't get the inode of
    alloc_chars lastSrc;
    ruMap inodePath;        // map k: inode to v: path for renames and such
    ruMap pathInode;        // map k: path to v: inode for deletes and such
    // ring of moved files k: inode v: filePath
    // the ring nature is not for thread safety, but for events timing out
    ruMap mrMap[moveRingBuckets];
    // keys of each map so we can retain order
    ruList mrLst[moveRingBuckets];

    // set true when run thread has initialized folder have been scanned
    bool hasInit;
    // set when it's time to quit cleaner
    bool quit;

    // FSEventStream and collateral pointers
    FSEventStreamRef stream;
    FSEventStreamContext fsc;
    CFRunLoopRef runLoop;
    CFStringRef watchPath;
    CFArrayRef pathsToWatch;
} famCtx;

#define ap(str) if (!out) { \
        out = ruStringNew(str); \
    } else { \
        ruStringAppendf("|%s", str); \
    }

static alloc_chars flagsToString(int32_t flags) {
    ruString out = NULL;
    if (flags & kFSEventStreamEventFlagMustScanSubDirs) {
        ap("must scan subdirs");
    }
    if (flags & kFSEventStreamEventFlagUserDropped) {
        ap("user dropped events")
    }
    if (flags & kFSEventStreamEventFlagKernelDropped) {
        ap("kernel dropped events")
    }
    if (flags & kFSEventStreamEventFlagEventIdsWrapped) {
        ap("event ids wrapped")
    }
    if (flags & kFSEventStreamEventFlagHistoryDone) {
        ap("history playback done")
    }
    if (flags & kFSEventStreamEventFlagRootChanged) {
        ap("root changed")
    }
    if (flags & kFSEventStreamEventFlagMount) {
        ap("mounted")
    }
    if (flags & kFSEventStreamEventFlagUnmount) {
        ap("unmounted")
    }
    if (flags & kFSEventStreamEventFlagItemCreated) {
        ap("created")
    }
    if (flags & kFSEventStreamEventFlagItemRemoved) {
        ap("removed")
    }
    if (flags & kFSEventStreamEventFlagItemInodeMetaMod) {
        ap("inodeMetaMod")
    }
    if (flags & kFSEventStreamEventFlagItemRenamed) {
        ap("renamed")
    }
    if (flags & kFSEventStreamEventFlagItemModified) {
        ap("modified")
    }
    if (flags & kFSEventStreamEventFlagItemFinderInfoMod) {
        ap("finderInfoMod")
    }
    if (flags & kFSEventStreamEventFlagItemChangeOwner) {
        ap("ownerChange")
    }
    if (flags & kFSEventStreamEventFlagItemXattrMod) {
        ap("attrMod")
    }
    if (flags & kFSEventStreamEventFlagItemIsFile) {
        ap("isFile")
    }
    if (flags & kFSEventStreamEventFlagItemIsDir) {
        ap("isDir")
    }
    if (flags & kFSEventStreamEventFlagItemIsSymlink) {
        ap("isSymlink")
    }
    if (flags & kFSEventStreamEventFlagOwnEvent) {
        ap("ownEvent")
    }
    alloc_chars res = ruStringGetCString(out);
    ruStringFree(out, true);
    return res;
}

static int32_t getBucketNum(int32_t offset) {
    return (int32_t)((ruTimeMs() / bucketMillies + offset) % moveRingBuckets);
}

static int32_t handleFile(famCtx *fctx, trans_chars path,
                          FSEventStreamEventFlags op, alloc_chars* srcFilePath) {
    // Sets the source file name if file was moved
    // Returns RU_FAM_MOVED, RU_FAM_MODIFIED, RU_FAM_CREATED or fam_ignore
    // We need to track file renames using inodes. We maintain pathInode and
    // inodePath, so we can look one up via the other.
    // Truth table 0 means path != mPath or inode != mInode
    // 1 means they're equal 0 also includes missing entries and defaults to ""
    // Also see git:includes/C/util/doc/fam/mac.drn
    // # | mPath mInode
    // 0 |   0      0
    // 1 |   0      1
    // 2 |   1      0
    // 3 |   1      1
    ruZeroedStruct(struct stat, fst);
    uint64_t inode = 0;
    alloc_chars trimedPath = NULL;
    // Keep this reference because ruMapRemove frees the value we hold in mPath,
    // but we still need it.
    // By storing it here it'll not be freed, but then we need to free it.
    alloc_chars mapPath = NULL;
    // reference to used up ->lastSrc  string si it is freed after use
    alloc_chars lastSrcRef = NULL;
    path = ruStrTrim(path, "/", ruTrimEnd, &trimedPath);

    if (stat(path, &fst)) {
        fam_dbg("file: '%s' is gone", path);
    } else {
        fam_dbg("path: '%s' ino: %ld size: %ld", path, fst.st_ino, fst.st_size);
        inode = fst.st_ino;
    }

    int32_t ret = fam_ignore;
    uint64_t mInode = 0;
    ruMapGet(fctx->pathInode, path, &mInode);

    if (inode) { // file there;
        perm_chars mPath = NULL;
        if (op & kFSEventStreamEventFlagItemRenamed) {
            // This is the second event of a file move. The inode can only
            // be in the moveRing. We need to retrieve it and remove the inode
            // from the current or last moveRing to prevent a deleted
            // event form being triggered by the cleanup thread
            int32_t bucket = getBucketNum(0);
            fam_dbg("looking for inode: %ld in bucket: %d", inode, bucket);
            ruMapGet(fctx->mrMap[bucket], &inode, &mPath);
            if (mPath) {
                fam_dbg("removing inode: %ld from bucket: %d", inode, bucket);
                ruMapRemove(fctx->mrMap[bucket], &inode, &mapPath);
                mPath = mapPath;
                fam_dbg("found %s", mPath);
            } else {
                // bucket - 1
                bucket = (bucket+(moveRingBuckets - 1)) % moveRingBuckets;
                fam_dbg("looking for inode: %ld in bucket: %d", inode, bucket);
                ruMapGet(fctx->mrMap[bucket], &inode, &mPath);
                if (mPath) {
                    fam_dbg("removing inode: %ld from bucket: %d", inode, bucket);
                    ruMapRemove(fctx->mrMap[bucket], &inode, &mapPath);
                    mPath = mapPath;
                    fam_dbg("found %s", mPath);
                } else {
                    // see if there is a last event
                    if (fctx->lastSrc) {
                        fam_dbg("found '%s' in lastSrc", fctx->lastSrc);
                        mPath = lastSrcRef = fctx->lastSrc;
                        fctx->lastSrc = NULL;
                    }
                }
            }
            if (mPath) {
                mInode = inode ; // so rename triggers
            }
        }

        if (!mPath) {
            // all others check our maps
            ruMapGet(fctx->inodePath, &inode, &mPath);
            if (!mPath && mInode) {
                mPath = path ; // so modified swapped out works
            }
        }

        if (ruStrEquals(path, mPath)) { // 2,3;
            // nothing changed for us unless inodes changed
            if (inode != mInode) { // 2 modify;
                // Modify because editors save to temp files and move to the
                // initial file. That changes the inode, so we need to update it
                fam_dbg("path: '%s' has a new inode: %ld", path, inode);
                ruMapPut(fctx->pathInode, path, inode);
                if (mInode) {
                    ruMapRemove(fctx->inodePath, &mInode, NULL);
                }
                ruMapPut(fctx->inodePath, &inode, path);
                ret = RU_FAM_MODIFIED;

            } else {
                fam_dbg("path: '%s' is unchanged", path);
            }

        } else { // 0, 1
            // the paths are different
            if (inode == mInode) { // 1 rename
                // Since move part 1 cleans out the maps,
                // we simply add the new mappings.
                ret = RU_FAM_MOVED;
                // returning source file signifies file move
                if (srcFilePath) *srcFilePath = ruStrDup(mPath);

            } else { // 0 new file
                // this looks like a new file and just needs to be added to the maps
                // the cleanup is pure paranoia
                fam_dbg("path: '%s' looks new", path);
                if (mPath) {
                    ruMapRemove(fctx->pathInode, mPath, NULL);
                }
                if (mInode) {
                    ruMapRemove(fctx->inodePath, &mInode, NULL);
                }
                ret = RU_FAM_CREATED;
            }

            fam_dbg("add mappings: '%s' inode: %ld", path, inode);
            ruMapPut(fctx->pathInode, path, inode);
            ruMapPut(fctx->inodePath, &inode, path);
        }

    } else { // file gone
        // if we don't know this file, we don't care
        if (mInode) {
            // we add it to the current moveRing so a future
            // event can finish the rename operation or the cleanup thread
            // reaps it in a while and triggers a deleted event
            int32_t b = getBucketNum(0);
            alloc_chars newPath = ruStrDup(path);
            fam_dbg("putting path: '%s' inode: %ld into bucket: %d", newPath, mInode, b);
            ruListAppend(fctx->mrLst[b], mInode);
            ruMapPut(fctx->mrMap[b], &mInode, newPath);
            // The file is removed from our maps regardless of delete or move
            ruMapRemove(fctx->pathInode, path, NULL);
            ruMapRemove(fctx->inodePath, &mInode, NULL);
        } else {
            // unknown events that are gone could have been created and renamed before
            // we got a hold of the inode. Here we remember this event
            FSEventStreamEventFlags mask =
                    kFSEventStreamEventFlagItemRenamed;// |
                    //kFSEventStreamEventFlagItemCreated;
            if (op & mask) {
                fam_dbg("remembering path: '%s' in lastSrc for potential rename later", path);
                ruReplace(fctx->lastSrc, ruStrDup(path));
            } else {
                fam_dbg("unknown path: '%s' is gone", path);
            }
        }
    }
    ruFree(trimedPath);
    ruFree(mapPath);
    ruFree(lastSrcRef);
    return ret;
}

static int32_t scanLst(trans_chars fullPath, bool isFolder, ptr o) {
    famCtx *fctx = (famCtx*)o;
    handleFile(fctx, fullPath, kFSEventStreamEventFlagNone, NULL);
    return RUE_OK;
}

static void fseCb(ConstFSEventStreamRef stream, void* ctx,
                  size_t numEvents, void* eventPaths,
                  const FSEventStreamEventFlags* eventFlags_l,
                  const FSEventStreamEventId* eventIds_q) {

    famCtx* fctx = (famCtx*)ctx;
    perm_chars* paths = (perm_chars*) eventPaths;
    fam_dbg("event count: %d", numEvents);
    for(size_t i = 0; i < numEvents; i++) {
        // get the start address
        perm_chars dcPath = paths[i];
        // find the length
        alloc_chars filePath = ruStrFromNfd(dcPath);
        if (!filePath) {
            ruCritLogf("Failed to convert '%s' from NFD", dcPath);
        }
        FSEventStreamEventFlags flags = eventFlags_l[i];
        FSEventStreamEventId evId = eventIds_q[i];
        if (ruDoesLog(RU_LOG_DBUG)) {
            // Dump out the arguments.
            if (flags == kFSEventStreamEventFlagNone) {
                fam_dbg("path[%d]: '%s' id: %ld flags: 0x%x something happened",
                        i, filePath, evId, flags);
            } else {
                alloc_chars ev = flagsToString(flags);
                fam_dbg("path[%d]: '%s' id: %ld flags: 0x%x [%s]",
                        i, filePath, evId, flags, ev);
                ruFree(ev);
            }
        }

        alloc_chars srcFile = NULL;
        int32_t status = handleFile(fctx, filePath, flags, &srcFile);

        if (status == fam_ignore) {
            fam_dbg("Ignoring transient path %s event", srcFile);
        } else if (status == RU_FAM_MOVED && srcFile) {
            ruFamEvent* fe = ruFamEventNew(RU_FAM_MOVED,
                                           srcFile, filePath);
            fam_dbg("The path %s was moved to %s", srcFile, filePath);
            fctx->eventCb(fe, fctx->ctx);

        } else if (flags & kFSEventStreamEventFlagItemCreated ||
                   status == RU_FAM_CREATED) {
            if (flags & kFSEventStreamEventFlagItemIsDir) {
                fam_dbg("The directory %s was created.", filePath);
                // scan the new folder to get the inode mappings
                ruFolderWalk(filePath, RU_WALK_FOLDER_FIRST,
                             scanLst, fctx);
            } else {
                fam_dbg("The path %s was created.", filePath);
            }
            ruFamEvent* fe = ruFamEventNew(RU_FAM_CREATED,
                                           filePath, NULL);
            fctx->eventCb(fe, fctx->ctx);
            if (!(flags & kFSEventStreamEventFlagItemIsDir)) {
                if (!(flags & kFSEventStreamEventFlagItemCreated) ||
                    flags & kFSEventStreamEventFlagItemModified) {
                    // add a modified event since it may not come from a move
                    fe = ruFamEventNew(RU_FAM_MODIFIED,
                                       filePath, NULL);
                    fam_dbg("Adding the path %s was modified.", filePath);
                    fctx->eventCb(fe, fctx->ctx);
                }
            }

        } else if (flags & kFSEventStreamEventFlagItemRemoved) {
            ruFamEvent* fe = ruFamEventNew(RU_FAM_DELETED,
                                           filePath, NULL);
            fam_dbg("The path %s was deleted.", filePath);
            fctx->eventCb(fe, fctx->ctx);

        } else if (flags & kFSEventStreamEventFlagItemModified ||
                   status == RU_FAM_MODIFIED) {
            if (flags & kFSEventStreamEventFlagItemIsDir) {
                // we ignore modified dirs
                fam_dbg("The directory %s was modified.", filePath);
            } else {
                ruFamEvent* fe = ruFamEventNew(RU_FAM_MODIFIED,
                                               filePath, NULL);
                fam_dbg("The file %s was modified.", filePath);
                fctx->eventCb(fe, fctx->ctx);
            }
        }
        ruFree(srcFile);
        ruFree(filePath);
    }
}

static ptr runThread(ptr o) {
    famCtx *fctx = (famCtx*)o;
    ruThreadSetName(fctx->name);
    ruInfoLog("starting");
    fctx->runLoop = CFRunLoopGetCurrent();
    FSEventStreamScheduleWithRunLoop(fctx->stream,
                                     fctx->runLoop,
                                     kCFRunLoopDefaultMode);
    if (!FSEventStreamStart(fctx->stream)) {
        ruCritLog("could not start FSEvent stream");
        return NULL;
    }
    // scan dirs for inodes after starting the stream to not miss any,
    // but before we work off the events, so we have all inodes mapped
    ruFolderWalk(fctx->topDir, RU_WALK_FOLDER_FIRST,
                 scanLst, fctx);
    fam_dbg("Loaded existing directory structure '%s'.", fctx->topDir);
    fctx->hasInit = true;
    fam_dbg("%s", "entering CFRunLoop");
    CFRunLoopRun();
    ruInfoLog("stopping");
    ruThreadSetName(NULL);
    return NULL;
}

static ptr cleanerThread(ptr o) {
    famCtx *fctx = (famCtx*)o;
    alloc_chars tname = ruDupPrintf("%sCleaner", fctx->name);
    ruThreadSetName(tname);
    ruFree(tname);
    ruInfoLog("starting");

    do {
        sec_t epoch = (sec_t)(ruTimeMs()/bucketMillies);
        // get the current bucket
        int32_t bucket = getBucketNum(2);
        fam_dbg("Running on bucket: %d at: %ld", bucket, epoch);
        // clean it out
        ruIterator li = ruListIter(fctx->mrLst[bucket]);
        for(int64_t inode = ruIterNext(li, int64_t);
               li; inode = ruIterNext(li, int64_t)) {
            alloc_chars path = NULL;
            int32_t ret = ruMapRemove(fctx->mrMap[bucket], &inode, &path);
            if (ret == RUE_OK) {
                ruFamEvent* fe = ruFamEventNew(RU_FAM_DELETED,
                                               path,NULL);
                fam_dbg("The path:%s from bucket: %d was deleted.", path, bucket);
                fctx->eventCb(fe, fctx->ctx);
            }
            ruFree(path);
        }
        ruListClear(fctx->mrLst[bucket]);

        do {
            // quittable delay til next epoch. Example calculation:
            // We started at 3000 and took 300, so epoch = 3, elasped 3300
            // delay = (3+1 * 1000) - 3300 = 4000 - 3300 = 700
            msec_t delay = (epoch+1) * bucketMillies - ruTimeMs();
            if (delay > 100) delay = 100;
            if (fctx->quit) break;
            //fam_dbg("sleeping for: %ldms", delay);
            if (delay <= 0) break;
            ruSleepMs(delay);
        } while (true);

    } while (!fctx->quit);

    ruInfoLog("stopping");
    ruThreadSetName(NULL);
    return NULL;
}

static void stopStream(famCtx *fctx) {
    if (!fctx->stream) return;
    ruVerbLogf("Stopping stream 0x%x", fctx->stream);
    FSEventStreamStop(fctx->stream);
    FSEventStreamInvalidate(fctx->stream);
    CFRunLoopStop(fctx->runLoop);
    ruVerbLog("RunLoop stopped");
}

// public functions
RUAPI ruFamCtx ruFamMonitorFilePath(trans_chars filePath, trans_chars threadName,
                              ruFamHandler eventCallBack, perm_ptr ctx) {
    ruVerbLogf( "Getting ready to monitor: %s", filePath);
    famCtx* fctx = ruMalloc0(1, famCtx);

    do {
        fctx->name = ruStrDup(threadName);
        fctx->topDir = ruStrDup(filePath);
        fctx->inodePath = ruMapNew(ruTypeInt64(), ruTypeStrDup());
        for (int32_t i = 0; i < moveRingBuckets; i++) {
            fctx->mrLst[i] = ruListNew(NULL);
            fctx->mrMap[i] = ruMapNew(ruTypeInt64(), ruTypeStrDup());
        }
        fctx->pathInode = ruMapNew(ruTypeStrDup(), ruTypeInt64());
        fctx->watchPath = CFStringCreateWithCString(
                NULL, fctx->topDir, kCFStringEncodingUTF8);
        fctx->pathsToWatch = CFArrayCreate(
                NULL, (trans_ptr*)&fctx->watchPath,
                1, NULL);
        fctx->fsc.info = fctx;
        CFAbsoluteTime latency = FSEventLatencySecs;
        fam_dbg("fseCb:%x", fseCb);
        // Create the stream, passing in a callback
        fctx->stream = FSEventStreamCreate(
                NULL, fseCb, &fctx->fsc,
                fctx->pathsToWatch,
                kFSEventStreamEventIdSinceNow, latency,
                kFSEventStreamCreateFlagNone |
                kFSEventStreamCreateFlagFileEvents);
        if (!fctx->stream) {
            ruCritLog("Failed to create fsevent stream");
            break;
        }
        fam_dbg("stream:%x famCtx:%x", fctx->stream, fctx);
        fctx->eventCb = eventCallBack;
        fctx->ctx = ctx;
        fctx->tid = ruThreadCreate(runThread, fctx);
        if (!fctx->tid) {
            ruCritLog("Failed to spawn CFRunLoop thread");
            break;
        }
        ruVerbLogf("Launched CFRunLoop thread: 0x%x",
                   ruThreadNativeId(fctx->tid, NULL));
        // wait for thread to initialize
        fctx->ctid = ruThreadCreate(cleanerThread, fctx);
        if (!fctx->ctid) {
            ruCritLog("Failed to spawn cleaner thread");
            break;
        }
        ruVerbLogf("Launched cleaner thread: 0x%x",
                   ruThreadNativeId(fctx->ctid, NULL));
        while (!fctx->hasInit) ruSleepMs(RU_FAM_QUEUE_TIMEOUT);

        return fctx;

    } while (false);

    // clean up a call gone bad
    ruFamKillMonitor(fctx);
    return NULL;
}

RUAPI ruFamCtx ruFamKillMonitor(ruFamCtx o) {
    famCtx* fctx = (famCtx*)o;
    ruInfoLogf("Request to kill fam thread %x", fctx->tid);
    fctx->quit = true;
    stopStream(fctx);
    if (fctx->tid) {
        bool res = ruThreadWait(fctx->tid, RU_FAM_KILL_TIMEOUT, NULL);
        if (res) {
            ruInfoLogf("Fam thread %s has been shut down", fctx->name);
        } else {
            ruWarnLogf("Fam thread %s timed out and was killed", fctx->name);
        }
    } else {
        ruWarnLogf("Fam thread %s is invalid", fctx->name);
    }
    if (fctx->ctid) {
        bool res = ruThreadWait(fctx->ctid, RU_FAM_KILL_TIMEOUT, NULL);
        if (res) {
            ruInfoLogf("Fam cleaner thread %s has been shut down", fctx->name);
        } else {
            ruWarnLogf("Fam cleaner thread %s timed out and was killed", fctx->name);
        }
    } else {
        ruWarnLogf("Fam cleaner thread %s is invalid", fctx->name);
    }
    FSEventStreamRelease(fctx->stream);
    CFRelease(fctx->pathsToWatch);
    CFRelease(fctx->watchPath);
    ruMapFree(fctx->inodePath);
    ruMapFree(fctx->pathInode);
    for (int i = 0; i < moveRingBuckets; i++) {
        ruMapFree(fctx->mrMap[i]);
        ruListFree(fctx->mrLst[i]);
    }
    ruFree(fctx->lastSrc);
    ruFree(fctx->topDir);
    ruFree(fctx->name);
    ruFree(fctx);
    return NULL;
}

RUAPI bool ruFamQuit(ruFamCtx o) {
    famCtx* fctx = (famCtx*)o;
    if (!fctx) return true;
    return fctx->quit;
}
