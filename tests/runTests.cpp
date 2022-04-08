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

Suite* getSuite ( void ) {
    Suite *suite = suite_create ( "util" );
    suite_add_tcase ( suite, miscTests() );
    suite_add_tcase ( suite, listTests() );
    suite_add_tcase ( suite, stringTests() );
    suite_add_tcase ( suite, mapTests() );
    suite_add_tcase ( suite, regexTests() );
    suite_add_tcase ( suite, ioTests() );
    suite_add_tcase ( suite, storeTests() );
    suite_add_tcase ( suite, cleanerTests() );
    return suite;
}

char* insureTestFolder(const char* folderName) {
    int ret;
    const char *tmp = ruGetenv("TEMP");
    if (!tmp) tmp = ruGetenv("TMPDIR");
    fail_if(tmp == NULL, "Could get temp folder. Please set TEMP env "
                         "variable to a writable folder");
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

#ifdef TEST_BASE
const char* testBase = TEST_BASE;
#else
const char* testBase = NULL;
#endif
char pathBuffer[1024];

char * makePath(const char *filepath) {
    snprintf(&pathBuffer[0], 1023, "%s/%s", testBase, filepath);
    return &pathBuffer[0];
}

#ifdef DO_IOS
int32_t mainTest (const char *tmpDir, const char *treepath) {
#else
int32_t main ( int32_t argc, char *argv[] ) {
#endif
    const char *testDir = ruGetenv("TEST_BASE");
    if (testDir) testBase = testDir;

    int32_t number_failed;
    ruSetLogger(ruStdErrorLogger, RU_LOG_VERB, NULL);
    Suite *suite = getSuite();
    SRunner *runner = srunner_create ( suite );
#ifdef DO_IOS
    setenv("TEMP", tmpDir, 1);
    srunner_set_fork_status (runner, CK_NOFORK);
    testBase = treepath;
#endif
    srunner_run_all ( runner, CK_NORMAL );
    number_failed = srunner_ntests_failed ( runner );
    srunner_free ( runner );
    return number_failed;
}
