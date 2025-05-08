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
/**
 * regify-util unit tests
 *
 * kate: mode C;
 */
#include <stdlib.h>
#include <stdio.h>
#include "tests.h"

// logging sink
const char* logPath = OUT_BASE "/test.log";

Suite* getSuite(void) {
    Suite *suite = suite_create("util");
    suite_add_tcase(suite, logTests());
    suite_add_tcase(suite, miscTests());
    suite_add_tcase(suite, listTests());
    suite_add_tcase(suite, stringTests());
    suite_add_tcase(suite, mapTests());
    suite_add_tcase(suite, setTests());
    suite_add_tcase(suite, htmlTests());
    suite_add_tcase(suite, regexTests());
    suite_add_tcase(suite, ioTests());
    suite_add_tcase(suite, iniTests());
    suite_add_tcase(suite, jsonTests());
    suite_add_tcase(suite, storeTests());
    suite_add_tcase(suite, cleanerTests());
    suite_add_tcase(suite, threadTests());
#if defined(__linux__) || defined(ITS_OSX) || defined(_WIN32)
    suite_add_tcase(suite, famTests());
#endif
    return suite;
}

char* insureTestFolder(const char* folderName) {
    int ret;
    const char *tmp = OUT_BASE;
    char *tmpDir;
    if(folderName) {
        tmpDir = ruDupPrintf("%s/ruTest/%s", tmp, folderName);
    } else {
        tmpDir = ruDupPrintf("%s/ruTest", tmp);
    }

    ret = ruFolderRemove(tmpDir);
    fail_unless(ret == RUE_OK, "Could not delete previous test folder '%s' error: %s",
                tmpDir, ruLastError());
    fail_unless(!ruIsDir(tmpDir), "Did not delete test folder '%s'", tmpDir);

    ret = ruMkdir(tmpDir, 0755, false);
    fail_unless(ruIsDir(tmpDir), "Could not create test folder '%s'", tmpDir);

    return tmpDir;
}

const char* testBase = SRC_BASE;
char pathBuffer[1024];
perm_chars makePath(const char *filepath) {
    snprintf(&pathBuffer[0], 1023, "%s/%s", testBase, filepath);
    return &pathBuffer[0];
}

const char* outBase = OUT_BASE;
char outBuffer[1024];
perm_chars makeOutPath(const char *filepath) {
    snprintf(&outBuffer[0], 1023, "%s/%s", outBase, filepath);
    return &outBuffer[0];
}

ruSinkCtx sc = NULL;

void setLogger(void) {
    if (!sc) sc = ruSinkCtxNew(logPath, NULL, NULL);
    if (!ruStrEmpty(logPath)) {
        ruSetLogger(ruFileLogSink, RU_LOG_DBUG, sc, true, true);
    } else {
        ruSetLogger(ruStdErrLogSink, RU_LOG_DBUG, NULL, true, true);
    }
}

#ifdef DO_IOS
int32_t mainTest (const char *tmpDir, const char *treepath) {
#else
int32_t main ( int32_t argc, char *argv[] ) {
#endif
    int32_t number_failed;
    // for failure debugging
    ruThreadSetName("main");
    setLogger();

    ruCleanAdd(ruGetCleaner(), "mainSecrét", "^^^MAIN_SECRET^^^");
    Suite *suite = getSuite();
    ruInfoLog("starting with mainSecrét and cleaner");
    SRunner *runner = srunner_create ( suite );
    srunner_set_fork_status (runner, CK_NOFORK);
    ruBacktraceInit(argv[0]);
#ifdef DO_IOS
    setenv("TEMP", tmpDir, 1);
    testBase = treepath;
#endif
    srunner_run_all ( runner, CK_NORMAL );
    number_failed = srunner_ntests_failed ( runner );
    srunner_free ( runner );
    ruFlushLog();
    ruInfoLog("stopping logger");
    ruStopLogger();
    ruSinkCtxFree(sc);
    return number_failed;
}
