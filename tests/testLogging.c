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

volatile int closed = 0;

#if 0
#define logDbg(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
#else
#define logDbg(fmt, ...)
#endif

static void closeCb(perm_ptr p) {
    closed++;
    logDbg("called closed: %d\n", closed);
}

static void sinkTest(bool threaded, bool cleaning) {
    closed = 0;
    perm_chars test;
    if (threaded) {
        if(cleaning) {
            test = "threaded clean";
        } else {
            test = "threaded";
        }
    } else {
        if(cleaning) {
            test = "non threaded clean";
        } else {
            test = "non threaded";
        }
    }
    logDbg("'%s' reset closed: %d\n", test, closed);
    perm_chars retText = "%s wanted ret '%d' but got '%d'";
    perm_chars log = makeOutPath("log1.log");

    ruCleaner rc = NULL;
    if (cleaning) {
        rc = ruCleanNew(0);
        ruCleanAdd(rc, "testsecret", "^^^TEST_SECRET^^^");
    }

    int clexp = 0;
    ruPreCtx rpc = ruPreCtxNew();
    fail_if(rpc == NULL, retText, rpc, NULL);
    fail_unless(clexp == closed, retText, test, clexp, closed);

    ruSetLogger(ruPreLogSink, RU_LOG_DBUG, rpc, rc, !threaded);
    ruInfoLogf("'%s' prelog entry 1", test);
    ruVerbLogf("'%s' prelog entry 2 with testsecret", test);
    ruDbgLogf("'%s' prelog entry 3", test);

    ruSinkCtx rsc = ruSinkCtxNew(log, closeCb, NULL);
    fail_if(rsc == NULL, retText, rsc, NULL);
    fail_unless(clexp == closed, retText, test, clexp, closed);

    ruSetLogger(ruFileLogSink, RU_LOG_VERB, rsc, rc, threaded);
    fail_unless(clexp == closed, retText, test, clexp, closed);

    // flush the log
    rpc = ruPreCtxFree(rpc, true);

    ruInfoLogf("'%s' log1 is '%s'", test, log);
    ruFlushLog();
    fail_unless(clexp == closed, retText, test, clexp, closed);

    ruSetLogLevel(RU_LOG_DBUG);
    fail_unless(clexp == closed, retText, test, clexp, closed);

    ruInfoLogf("starting '%s' with testsecret", test);
    fail_unless(clexp == closed, retText, test, clexp, closed);

    ruFlushLog();
    fail_unless(clexp == closed, retText, test, clexp, closed);

    clexp = 1;
    log = makeOutPath("log2.log");
    logDbg("'%s' pre ruSinkCtxPath: %d\n", test, closed);
    ruSinkCtxPath(rsc, log);
    logDbg("'%s' post ruSinkCtxPath: %d\n", test, closed);
    ruInfoLogf("'%s' log2 is '%s'", test, log);
    logDbg("'%s' post info: %d\n", test, closed);
    sec_t end = ruTimeSec() + 1;
    while (clexp != closed && !ruTimeEllapsed(end)) ruSleepMs(1);
    fail_unless(clexp == closed, retText, test, clexp, closed);

    ruFlushLog();
    fail_unless(clexp == closed, retText, test, clexp, closed);

    ruStopLogger();
    fail_unless(clexp == closed, retText, test, clexp, closed);

    ruSinkCtxFree(rsc);
    ruCleanFree(rc);
}

START_TEST (filesink) {
    ruFileRemove(makeOutPath("log1.log"));
    ruFileRemove(makeOutPath("log2.log"));
    sinkTest(false, true);
    sinkTest(false, false);
    sinkTest(true, false);
    sinkTest(true, true);
    sinkTest(false, true);
    setLogger();
}
END_TEST

TCase* logTests(void) {
    TCase *tcase = tcase_create("log");
    tcase_add_test(tcase, filesink);
    return tcase;
}
