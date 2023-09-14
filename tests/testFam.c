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
#include "tests.h"

#define topDir TMPDIR
#define topD2 topDir "/d2"
#define topfoo topDir "/foo"
#define watchDir topDir "/watch"
#define d1 watchDir "/d1"
#define d2 watchDir "/d2"
#define doe watchDir "/dö"
#define doefoe1 doe "/foö¹"
#define d1foo d1 "/foo"
#define d1bar d1 "/bar"
#define d1d2 d1 "/d2"
#define d1d2foo d1d2 "/foo"
#define d2foo d2 "/foo"

static bool isWatching = false;
// windows doesn't like it when check does longjmps in it's worker threads
// so assertions must be done afterward and this signals exiting due to failure
static bool failed = false;

typedef struct famTest_ famTest;
struct famTest_ {
    ruList events;
#ifdef _WIN32
    char* lastFile;  // last file for multiple mods
#endif
};

static void insureDir(char* dir, bool isEmpty) {
    ruVerbLogf("Creating folder: %s", dir);
    if (ruStrEmpty(dir) || ruStrEquals(dir, "/") ||
            ruStrEquals(dir, "c:\\")) {
        ck_abort_msg("Refusing to delete: %s", dir);
    }
    int ret;
    if (ruFileExists(dir)) {
        // already there
        if (ruIsDir(dir)) {
            if (isEmpty) {
                if ((ret = ruFolderRemove(dir))) {
                    ck_abort_msg("Failed to remove folder: %s %d", dir, ret);
                }
            } else {
                return;
            }
        } else {
            if ((ret = ruFileRemove(dir))) {
                ck_abort_msg("Failed to remove file: %s %d", dir, ret);
            }
        }
    }
    if ((ret = ruMkdir(dir, 0775, true))) {
        ck_abort_msg("Failed to create folder: %s %d", dir, ret);
    }
}

static bool compareFamEvents(ruFamEvent* required, ruFamEvent* actual) {
    if (required->eventType == actual->eventType &&
        (ruStrEquals(required->srcPath, actual->srcPath) ||
         ruStrEmpty(required->srcPath) && ruStrEmpty(actual->srcPath)) &&
        (ruStrEquals(required->dstPath, actual->dstPath) ||
        ruStrEmpty(required->dstPath) && ruStrEmpty(actual->dstPath))) {
        return true;
    }
    ruFamEventLog(RU_LOG_VERB, required, "Required:  ");
    ruFamEventLog(RU_LOG_VERB, actual, "Not Actual:");
    return false;
}

static void myfamHandler(ruFamEvent* fe, perm_ptr ctx) {
    famTest* ft = (famTest*)ctx;

#ifdef _WIN32
    // windows is nondeterministic on the order of these events so quit testing them
    if (failed || fe->eventType == RU_FAM_ATTRIB ||
        fe->eventType == RU_FAM_MODIFIED &&
        ruStrEquals(fe->srcPath, ft->lastFile)) {
        // ignore subsequent mods on M$
        ruVerbLog("Ignored");
        ruFamEventFree(fe);
        return;
    }
#endif
    ruFamEventLog(RU_LOG_INFO, fe, "got");
    ruFamEvent* req = NULL;
    if (!isWatching) {
        req = ruListPop(ft->events, NULL);
        if (req == 0) {
            ruFamEventLog(RU_LOG_CRIT, fe, "got unexpected event");
#ifdef _WIN32
            failed = true;
            return;
#endif
            ck_abort_msg("got unexpected event");
        }
        if (!compareFamEvents(req, fe)) {
#ifdef _WIN32
            ruCritLog("Failed compare");
            failed = true;
            return;
#endif
            ck_abort_msg("Failed compare");
        }
    }
#ifdef _WIN32
    ruFree(ft->lastFile);
    if ( fe->eventType == RU_FAM_CREATED || fe->eventType == RU_FAM_MODIFIED) {
      ft->lastFile = ruStrDup(fe->srcPath);
    } else if ( fe->eventType == RU_FAM_MOVED) {
      ft->lastFile = ruStrDup(fe->dstPath);
    } else {;
      ft->lastFile = NULL;
    }
#endif
    ruFamEventFree(fe);
    ruFamEventFree(req);
}

static void writeFile(char* filePath, char* content) {
    ruVerbLogf("Writing file: %s", filePath);
    FILE* fh;
    int ret;
    if (ruFileExists(filePath)) {
        fh = ruFOpen(filePath, "a", &ret);
    } else {
        fh = ruFOpen(filePath, "w", &ret);
    }
    rusize len = strlen(content);
    rusize wlen = fwrite(content, 1,len,fh);
    if (len != wlen) {
        ck_abort_msg("Only wrote %lu of %lu bytes to: %s", wlen, len, filePath);
    }
    fclose(fh);
}

static void killFile(char* filePath) {
    ruVerbLogf("Removing file: %s", filePath);
    if (!ruFileExists(filePath)) return;
    int ret;
    if (ruIsDir(filePath)) {
        if ((ret = ruFolderRemove(filePath))) {
            ck_abort_msg("Failed to remove folder: %s %d", filePath, ret);
        }
    } else {
        if ((ret = ruFileRemove(filePath))) {
            ck_abort_msg("Failed to remove file: %s %d", filePath, ret);
        }
    }
}

static void moveFile(char* oldPath, char* newPath) {
    ruVerbLogf("Moving file: %s to: %s", oldPath, newPath);
    int ret;
    if ((ret = ruFileRename(oldPath, newPath))) {
        ck_abort_msg("failed renaming %s to %s", oldPath, newPath);
    }
}

static bool checkQueue(famTest* ft, char* test) {
    if (isWatching) {
        ruVerbLogf("Just did [%s]", test);
        return true;
    }
#ifdef ITS_OSX
    int i = 60; // OSX takes more time
#else
    int i = 10; // max 500ms wait
#endif
    do {
        if (failed) return false;
        if (!ruListSize(ft->events, NULL)) return true;
        i--;
        if (!i) {
            ruFamEvent* req = ruListPop(ft->events, NULL);
            ruCritLogf("Test [%s] failed.", test);
            ruFamEventLog(RU_LOG_CRIT, req, "The following event is left");
#ifdef _WIN32
            failed = true;
            return false;
#endif
            ck_abort_msg("Leftover event");
        }
        //ruDbgLogf("waiting some more %d", i);
        ruSleepMs(50);
    } while (true);
}

static void addEvent(famTest* ft, int eventType, char* filePath, char* destPath) {
    if (isWatching) return;
    ruFamEvent* fe = ruFamEventNew(eventType, filePath, destPath);
    ruFamEventLog(RU_LOG_INFO, fe, "adding");
    ruListAppend(ft->events, fe);
}

static bool testit(famTest* ctx) {

    // create folder
    ruInfoLog("create folder d2");
    addEvent(ctx, RU_FAM_CREATED, d2, NULL);
    insureDir(d2, true);
    if (!checkQueue(ctx, "create folder " d2)) return false;

//<editor-fold desc="darwin">
#ifdef ITS_OSX
    msec_t delay = 1;
    if (isWatching) delay = 3000;

    ruSleepMs(delay);
    // create file
    addEvent(ctx, RU_FAM_CREATED, d1foo, NULL);
    addEvent(ctx, RU_FAM_MODIFIED, d1foo, NULL);
    writeFile(d1foo, "foo was here ");
    checkQueue(ctx, "write " d1foo);

    ruSleepMs(delay);
    addEvent(ctx, RU_FAM_MOVED, d1foo, d2foo);
    moveFile(d1foo, d2foo);
    checkQueue(ctx, "move file within " d1foo " " d2foo);

    ruSleepMs(delay);
    addEvent(ctx, RU_FAM_MOVED, d2, d1d2);
    moveFile(d2, d1d2);
    checkQueue(ctx, "move folder within " d2 " " d1d2);

    // OSX inode tracking does not reach into sub folders, so it'll be a new file
    ruSleepMs(delay);
    addEvent(ctx, RU_FAM_MOVED, d1d2foo, d1foo);
    moveFile(d1d2foo, d1foo);
    checkQueue(ctx, "move file within again " d1d2foo " " d1foo);

    // OSX inode tracking does not reach into sub folders, so it'll be a new file
    ruSleepMs(delay);
    addEvent(ctx, RU_FAM_DELETED, d1foo, NULL);
    moveFile(d1foo, topfoo);
    checkQueue(ctx, "move file out " d1foo " " topfoo);

    ruSleepMs(delay);
    addEvent(ctx, RU_FAM_DELETED, d1d2, NULL);
    moveFile(d1d2, topD2);
    checkQueue(ctx, "move folder out " d1d2 " " topD2);

    ruSleepMs(delay);
    addEvent(ctx, RU_FAM_CREATED, d1d2, NULL);
    moveFile(topD2, d1d2);
    checkQueue(ctx, "move folder in " topD2 " " d1d2);

    ruSleepMs(delay);
    addEvent(ctx, RU_FAM_CREATED, d1foo, NULL);
    addEvent(ctx, RU_FAM_MODIFIED, d1foo, NULL);
    moveFile(topfoo, d1foo);
    checkQueue(ctx, "move file in " topfoo " " d1foo);

    ruVerbLog("rm d1foo");
    ruSleepMs(delay);
    addEvent(ctx, RU_FAM_DELETED, d1foo, NULL);
    killFile(d1foo);
    checkQueue(ctx, "delete file " d1foo);

    ruVerbLog("rm d1");
    ruSleepMs(delay);
    addEvent(ctx, RU_FAM_DELETED, d1d2, NULL);
    addEvent(ctx, RU_FAM_DELETED, d1, NULL);
    killFile(d1);
    checkQueue(ctx, "delete folder " d1);


#endif
//</editor-fold>

//<editor-fold desc="linux">
#ifdef __linux__
    ruInfoLog("create folder dö");
    addEvent(ctx, RU_FAM_CREATED, doe, NULL);
    insureDir(doe, true);
    if (!checkQueue(ctx, "create folder " doe)) return false;

    // create file
    addEvent(ctx, RU_FAM_CREATED, doefoe1, NULL);
    addEvent(ctx, RU_FAM_MODIFIED, doefoe1, NULL);
    writeFile(doefoe1, "foo was here ");
    checkQueue(ctx, "write " doefoe1);

    // create file
    addEvent(ctx, RU_FAM_CREATED, d1foo, NULL);
    addEvent(ctx, RU_FAM_MODIFIED, d1foo, NULL);
    writeFile(d1foo, "foo was here ");
    checkQueue(ctx, "write " d1foo);

    addEvent(ctx, RU_FAM_MOVED, d1foo, d2foo);
    moveFile(d1foo, d2foo);
    checkQueue(ctx, "move file within");

    addEvent(ctx, RU_FAM_MOVED, d2, d1d2);
    moveFile(d2, d1d2);
    checkQueue(ctx, "move folder within " d2 " " d1d2);

    addEvent(ctx, RU_FAM_MOVED, d1d2foo, d1foo);
    moveFile(d1d2foo, d1foo);
    checkQueue(ctx, "move file within again");

    addEvent(ctx, RU_FAM_MOVED, d1foo, NULL);
    moveFile(d1foo, topfoo);
    checkQueue(ctx, "move file out");

    addEvent(ctx, RU_FAM_MOVED, d1d2, NULL);
    moveFile(d1d2, topD2);
    checkQueue(ctx, "move folder out");

    addEvent(ctx, RU_FAM_MOVED, "", d1d2);
    moveFile(topD2, d1d2);
    checkQueue(ctx, "move folder in");

    addEvent(ctx, RU_FAM_MOVED, "", d1foo);
    moveFile(topfoo, d1foo);
    checkQueue(ctx, "move file in");

    addEvent(ctx, RU_FAM_MODIFIED, d1foo, NULL);
    writeFile(d1foo, "foo was here, bar was, too");
    checkQueue(ctx, "modify file");

    ruVerbLog("rm d1foo");
    addEvent(ctx, RU_FAM_DELETED, d1foo, NULL);
    killFile(d1foo);
    checkQueue(ctx, "delete file " d1foo);

    ruVerbLog("rm d1");
    addEvent(ctx, RU_FAM_DELETED, d1d2, NULL);
    addEvent(ctx, RU_FAM_DELETED, d1, NULL);
    killFile(d1);
    checkQueue(ctx, "delete folder " d1);

    ruVerbLog("rm dö/foö¹");
    addEvent(ctx, RU_FAM_DELETED, doefoe1, NULL);
    killFile(doefoe1);
    checkQueue(ctx, "delete file " doefoe1);

    ruVerbLog("rm dö");
    addEvent(ctx, RU_FAM_DELETED, doe, NULL);
    killFile(doe);
    checkQueue(ctx, "delete folder " doe);

#endif
//</editor-fold>

//<editor-fold desc="M$">
#ifdef _WIN32
    // create file
    ruInfoLog("create file d1foo");
    addEvent(ctx, RU_FAM_CREATED, d1foo, NULL);
    writeFile(d1foo, "foo was here ");
    if (failed || !checkQueue(ctx, "write " d1foo)) return false;

    ruInfoLog("mv d1foo d2foo");
    addEvent(ctx, RU_FAM_DELETED, d1foo, NULL);
    addEvent(ctx, RU_FAM_CREATED, d2foo, NULL);
    moveFile(d1foo, d2foo);
    if (failed || !checkQueue(ctx, "move file within " d1foo " " d2foo)) return false;

    ruInfoLog("mv d2 d1d2");
    addEvent(ctx, RU_FAM_DELETED, d2, NULL);
    addEvent(ctx, RU_FAM_CREATED, d1d2, NULL);
    moveFile(d2, d1d2);
    if (failed || !checkQueue(ctx, "move folder within " d2 " " d1d2)) return false;

    ruInfoLog("mv d1d2foo d1foo");
    addEvent(ctx, RU_FAM_DELETED, d1d2foo, NULL);
    addEvent(ctx, RU_FAM_CREATED, d1foo, NULL);
    moveFile(d1d2foo, d1foo);
    if (failed || !checkQueue(ctx, "move file within again " d1d2foo " " d1foo)) return false;

    ruInfoLog("rm d1foo");
    addEvent(ctx, RU_FAM_DELETED, d1foo, NULL);
    moveFile(d1foo, topfoo);
    if (failed || !checkQueue(ctx, "move file out " d1foo " " topfoo)) return false;

    ruInfoLog("mv d1d2 d2");
    addEvent(ctx, RU_FAM_DELETED, d1d2, NULL);
    moveFile(d1d2, topD2);
    if (failed || !checkQueue(ctx, "move folder out " d1d2 " " topD2)) return false;

    ruInfoLog("mv d2 d1d2");
    addEvent(ctx, RU_FAM_CREATED, d1d2, NULL);
    moveFile(topD2, d1d2);
    if (failed || !checkQueue(ctx, "move folder in " topD2 " " d1d2)) return false;

    ruInfoLog("create file d1foo");
    addEvent(ctx, RU_FAM_CREATED, d1foo, NULL);
    moveFile(topfoo, d1foo);
    if (failed || !checkQueue(ctx, "move file in " topfoo " " d1foo)) return false;

    ruInfoLog("mv d1foo d1bar");
    addEvent(ctx, RU_FAM_MOVED, d1foo, d1bar);
    moveFile(d1foo, d1bar);
    if (failed || !checkQueue(ctx, "rename file 1 " d1foo " " d1bar)) return false;

    ruInfoLog("mv d1bar d1foo");
    addEvent(ctx, RU_FAM_MOVED, d1bar, d1foo);
    moveFile(d1bar, d1foo);
    if (failed || !checkQueue(ctx, "rename file 2 " d1bar " " d1foo)) return false;

    ctx->lastFile = NULL;
    ruInfoLog("modify d1foo");
    addEvent(ctx, RU_FAM_MODIFIED, d1foo, NULL);
    writeFile(d1foo, "foo was here, bar was, too");
    if (failed || !checkQueue(ctx, "modify file " d1foo)) return false;

    ruInfoLog("rm d1foo");
    addEvent(ctx, RU_FAM_DELETED, d1foo, NULL);
    killFile(d1foo);
    if (failed || !checkQueue(ctx, "delete file " d1foo)) return false;

    // windows is nondeterministic on the order of these so quit testing this
//    ruInfoLog("rm d1");
//    addEvent(ctx, RU_FAM_DELETED, d1d2, NULL);
//    addEvent(ctx, RU_FAM_MODIFIED, d1, NULL);
//    addEvent(ctx, RU_FAM_DELETED, d1, NULL);
//    killFile(d1);
//    if (failed || !checkQueue(ctx, "delete folder " d1)) return false;

#endif
//</editor-fold>
    return true;
}

START_TEST(run) {
    ruFamCtx famCtx = NULL;
    ruZeroedStruct(famTest, ctx);
    ctx.events = ruListNew(ruTypePtr(ruFamEventFree));

    insureDir(topDir, false);
    insureDir(watchDir, true);
    insureDir(d1, true);
    //isWatching = true;

    ruVerbLog("Launching monitor");
    famCtx = ruFamMonitorFilePath(watchDir, "test",
            myfamHandler, &ctx);
    ruSleepMs(500);

    ruVerbLog("Starting test");
    bool pass = testit(&ctx);

    if (isWatching) {
        // wait for events to come in
        ruSleepMs(3000);
    }
    ruFamKillMonitor(famCtx);
    ctx.events = ruListFree(ctx.events);
    // this is needed for windows
    if (failed || !pass) ck_abort_msg("tests failed");
}
END_TEST

TCase* famTests(void) {
    TCase *tcase = tcase_create("fam");
    tcase_add_test(tcase, run);
    return tcase;
}
