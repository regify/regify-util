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
#define d1foo d1 "/foo"
#define d1bar d1 "/bar"
#define d1d2 d1 "/d2"
#define d1d2foo d1d2 "/foo"
#define d2foo d2 "/foo"

static bool isLenient = false;
static bool isWatching = false;

typedef struct famTest_ famTest;
struct famTest_ {
    ruList events;
#ifdef RUMS
    char* lastFile;  // last file for multiple mods
#endif
};

static void insureDir(char* dir, bool isEmpty) {
    ruVerbLogf("Creating folder: %s", dir);
    if (ruStrEmpty(dir) || ruStrEquals(dir, "/") ||
            ruStrEquals(dir, "c:\\")) {
        fail("Refusing to delete: %s", dir);
    }
    int ret;
    if (ruFileExists(dir)) {
        // already there
        if (ruIsDir(dir)) {
            if (isEmpty) {
                if ((ret = ruFolderRemove(dir))) {
                    fail("Failed to remove folder: %s %d", dir, ret);
                }
            } else {
                return;
            }
        } else {
            if ((ret = ruFileRemove(dir))) {
                fail("Failed to remove file: %s %d", dir, ret);
            }
        }
    }
    if ((ret = ruMkdir(dir, 0775, true))) {
        fail("Failed to create folder: %s %d", dir, ret);
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
    if (isWatching) return;
    ruFamEventLog(RU_LOG_VERB, fe, "got");
    famTest* ft = (famTest*)ctx;

#ifdef RUMS
    if (fe->eventType == fam_modified &&
        ruStrCmp(fe->filePath, ft->lastFile) == 0) {
        // ignore subsequent mods on M$
        ruVerbLogf( "Ignored");
        ruFamEventFree(fe);
        return;
    }
#endif
    ruFamEvent* req = ruListPop(ft->events, NULL);
    if (req == 0) {
        ruFamEventLog(RU_LOG_CRIT, fe, "unexpected event:");
        fail("got unexpected event");
    }
    if (!compareFamEvents(req, fe)) {
        fail("Failed compare");
    }
#ifdef RUMS
    ruFree(ft->lastFile);
    if ( fe->eventType == fam_created || fe->eventType == fam_modified) {
      ft->lastFile = ruStrDup(fe->filePath);
    } else if ( fe->eventType == fam_moved) {
      ft->lastFile = ruStrDup(fe->destPath);
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
    } else { ;
        fh = ruFOpen(filePath, "w", &ret);
    }
    rusize len = strlen(content);
    rusize wlen = fwrite(content, 1,len,fh);
    if (len != wlen) {
        fail("Only wrote %d of %d bytes to: %s", wlen, len, filePath);
    }
    fclose(fh);
}

static void killFile(char* filePath) {
    ruVerbLogf("Removing file: %s", filePath);
    if (!ruFileExists(filePath)) return;
    int ret;
    if (ruIsDir(filePath)) {
        if ((ret = ruFolderRemove(filePath))) {
            fail("Failed to remove folder: %s %d", filePath, ret);
        }
    } else { ;
        if ((ret = ruFileRemove(filePath))) {
            fail("Failed to remove file: %s %d", filePath, ret);
        }
    }
}

static void moveFile(char* oldPath, char* newPath) {
    ruVerbLogf("Moving file: %s to: %s", oldPath, newPath);
    int ret;
    if ((ret = ruFileRename(oldPath, newPath))) {
        fail("failed renaming %s to %s", oldPath, newPath);
    }
}

static void checkQueue(famTest* ft, char* test) {
    if (isWatching) {
        ruVerbLogf("Just did [%s]", test);
        return;
    }

    int i = 10; // max 500ms wait
    do {
        if (!ruListSize(ft->events, NULL)) return;
        i--;
        if (!i) {
            ruFamEvent* req = ruListPop(ft->events, NULL);
            ruCritLogf("Test [%s] failed.", test);
            ruFamEventLog(RU_LOG_CRIT, req, "The following event is left");
            fail("Leftover event");
        }
//        ruDbgLogf("waiting some more %d", i);
        ruSleepMs(50);
    } while (true);
}

static void addEvent(famTest* ft, int eventType, char* filePath, char* destPath) {
    if (isWatching) return;
    ruFamEvent* fe = ruFamEventNew(eventType, filePath, destPath);
    ruFamEventLog(RU_LOG_VERB, fe, "adding");
    ruListAppend(ft->events, fe);
}

static int testit(famTest* ctx) {

    // create folder
    ruVerbLog( "create folder d2");
    addEvent(ctx, RU_FAM_CREATED, d2, NULL);
    insureDir(d2, true);
    checkQueue(ctx, "create folder " d2);

//<editor-fold desc="*nix">
#ifndef _WIN32
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


#endif
//</editor-fold>

//<editor-fold desc="M$">
#ifdef _WIN32
    // create file
  ruVerbLog("create file d1foo");
  addEvent(ctx, fam_created, d1foo, NULL);
  addEvent(ctx, fam_attrib, d1, NULL);
  addEvent(ctx, fam_modified, d1foo, NULL);
  writeFile(d1foo, "foo was here ");
  checkQueue(ctx, "write " d1foo);

  ruVerbLog("mv d1foo d2foo");
  addEvent(ctx, fam_deleted, d1foo, NULL);
  addEvent(ctx, fam_created, d2foo, NULL);
  addEvent(ctx, fam_modified, d2, NULL);
  moveFile(d1foo, d2foo);
  checkQueue(ctx, "move file within " d1foo " " d2foo);

  ruVerbLog("mv d2 d1d2");
  addEvent(ctx, fam_deleted, d2, NULL);
  addEvent(ctx, fam_created, d1d2, NULL);
  addEvent(ctx, fam_attrib, d1, NULL);
  moveFile(d2, d1d2);
  checkQueue(ctx, "move folder within " d2 " " d1d2);

  ruVerbLog("mv d1d2foo d1foo");
  addEvent(ctx, fam_deleted, d1d2foo, NULL);
  addEvent(ctx, fam_created, d1foo, NULL);
  addEvent(ctx, fam_attrib, d1, NULL);
  addEvent(ctx, fam_attrib, d1d2, NULL);
  moveFile(d1d2foo, d1foo);
  checkQueue(ctx, "move file within again " d1d2foo " " d1foo);

  ruVerbLog("rm d1foo");
  addEvent(ctx, fam_deleted, d1foo, NULL);
  moveFile(d1foo, topfoo);
  checkQueue(ctx, "move file out " d1foo " " topfoo);

  ruVerbLog("mv d1d2 d2");
  addEvent(ctx, fam_deleted, d1d2, NULL);
  addEvent(ctx, fam_attrib, d1, NULL);
  moveFile(d1d2, topD2);
  checkQueue(ctx, "move folder out " d1d2 " " topD2);

  ruVerbLog("mv d2 d1d2");
  addEvent(ctx, fam_created, d1d2, NULL);
  addEvent(ctx, fam_attrib, d1, NULL);
  moveFile(topD2, d1d2);
  checkQueue(ctx, "move folder in " topD2 " " d1d2);

  ruVerbLog("create file d1foo");
  addEvent(ctx, fam_created, d1foo, NULL);
  addEvent(ctx, fam_attrib, d1, NULL);
  moveFile(topfoo, d1foo);
  checkQueue(ctx, "move file in " topfoo " " d1foo);

  ruVerbLog("mv d1foo d1bar");
  addEvent(ctx, fam_moved, d1foo, d1bar);
  addEvent(ctx, fam_attrib, d1, NULL);
  moveFile(d1foo, d1bar);
  checkQueue(ctx, "rename file 1 " d1foo " " d1bar);

  ruVerbLog("mv d1bar d1foo");
  addEvent(ctx, fam_moved, d1bar, d1foo);
  addEvent(ctx, fam_attrib, d1, NULL);
  moveFile(d1bar, d1foo);
  checkQueue(ctx, "rename file 2 " d1bar " " d1foo);

  ctx->lastFile = NULL;
  ruVerbLog("modify d1foo");
  addEvent(ctx, fam_modified, d1foo, NULL);
  writeFile(d1foo, "foo was here, bar was, too");
  checkQueue(ctx, "modify file " d1foo);

  ruVerbLog("rm d1foo");
  addEvent(ctx, fam_deleted, d1foo, NULL);
  addEvent(ctx, fam_modified, d1, NULL);
  killFile(d1foo);
  checkQueue(ctx, "delete file " d1foo);

  ruVerbLog("rm d1");
  addEvent(ctx, fam_deleted, d1d2, NULL);
  addEvent(ctx, fam_modified, d1, NULL);
  addEvent(ctx, fam_deleted, d1, NULL);
  killFile(d1);
  checkQueue(ctx, "delete folder " d1);

#endif
//</editor-fold>

}

START_TEST (run) {
    ruFamCtx famCtx = NULL;
    famTest ctx;
    ctx.events = ruListNew(ruFamEventFreeV);

    insureDir(topDir, false);
    insureDir(watchDir, true);
    insureDir(d1, true);

    //isWatching = true;
    isLenient = true;

    ruVerbLog("Launching monitor");
    famCtx = ruFamMonitorFilePath(watchDir, "test",
            myfamHandler, &ctx);
    ruSleepMs(500);

    ruVerbLog( "Starting test");
    testit(&ctx);

    if (isWatching) {
        // wait for events to come in
        ruSleepMs(1000);
    }
    ruFamKillMonitor(famCtx);
    ctx.events = ruListFree(ctx.events);
}
END_TEST

TCase* famTests(void) {
    TCase *tcase = tcase_create("fam");
    tcase_add_test(tcase, run);
    return tcase;
}
