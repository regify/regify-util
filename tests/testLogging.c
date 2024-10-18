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
        rc = ruGetCleaner();
        ruCleanAdd(rc, "testsecret", "^^^TEST_SECRET^^^");
    }

    int clexp = 0;
    ruPreCtx rpc = ruPreCtxNew();
    fail_if(rpc == NULL, retText, rpc, NULL);
    fail_unless(clexp == closed, retText, test, clexp, closed);

    ruSetLogger(ruPreLogSink, RU_LOG_DBUG, rpc, cleaning, !threaded);
    ruInfoLogf("'%s' prelog entry 1", test);
    ruVerbLogf("'%s' prelog entry 2 with testsecret", test);
    ruDbgLogf("'%s' prelog entry 3", test);

    ruSinkCtx rsc = ruSinkCtxNew(log, closeCb, NULL);
    fail_if(rsc == NULL, retText, rsc, NULL);
    fail_unless(clexp == closed, retText, test, clexp, closed);

    ruSetLogger(ruFileLogSink, RU_LOG_VERB, rsc, cleaning, threaded);
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
    ruInfoLogf("'%s' closing log1", test);
    ruSinkCtxPath(rsc, log);
    logDbg("'%s' post ruSinkCtxPath: %d\n", test, closed);
    ruInfoLogf("'%s' log2 is '%s'", test, log);
    logDbg("'%s' post info: %d\n", test, closed);
    sec_t end = ruTimeSec() + 1;
    while (clexp != closed && !ruTimeEllapsed(end)) ruSleepMs(1);
    fail_unless(clexp == closed, retText, test, clexp, closed);

    ruFlushLog();
    fail_unless(clexp == closed, retText, test, clexp, closed);

    clexp = 2;
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

#define thraedCount 5
ruRegex rr = NULL;
bool results[thraedCount][2];

int writeCb(trans_chars msg ,FILE* wh) {
    ruSleepMs(250);
    alloc_chars msg2 = ruMakeLogMsg(RU_LOG_VERB, "", "", 0, "%s", (char*)msg);
    int out = fputs(msg2, wh);
    ruFree(msg2);

    int32_t ret;
    ruList matches = ruRegexFindGroups(rr, msg, &ret);
    do {
        if (ret != RUE_OK) break;
        int tokenIdx = -1;
        char *tok = ruListIdx(matches, 2, char*, &ret);
        if (ret != RUE_OK) break;
        if (ruStrEquals("start", tok)) {
            tokenIdx = 0;
        } else if (ruStrEquals("end", tok)) {
            tokenIdx = 1;
        }
        if (tokenIdx < 0) break;

        char *num = ruListIdx(matches, 1, char*, &ret);
        if (ret != RUE_OK) break;
        int thread = ruStrToInt(num);
        if (thread < 1 || thread > thraedCount) break;
        results[thread - 1][tokenIdx] = true;
    } while (0);
    if (matches) ruListFree(matches);

    return out;
}

static void* logr(void* arg) {
    ruInfoLog("start");
    int32_t last = 10;
    for (int i = 0 ; i < last; i++) {
        ruVerbLogf("item %d", i);
        ruSleepMs(10);
    }
    ruInfoLog("end");
    closed++;
    return NULL;
}

START_TEST (cutoff) {
    // here we see that things that got logged before ruStopLogger is called
    // are indeed placed into the log file before ruStopLogger proceeds
    perm_chars retText = "wanted '%d' but got '%d'";
    perm_chars logM = makeOutPath("logMany.log");
    int32_t ret, exp = RUE_OK;
    ruThread rts[thraedCount];
    memset(&results, 0, sizeof(results));

    closed = 0;
    ruFileRemove(logM);

    ruList matches = NULL;
    rr = ruRegexNew("thread-00(\\d).*: (start|end)", 0, &ret);
    fail_unless(exp == ret, retText, exp, ret);

    ruSinkCtx rsc = ruSinkCtxNew(logM, NULL, NULL);
    fail_if(rsc == NULL, retText, rsc, NULL);

    ret = ruSinkWriteCb(rsc, &writeCb);
    fail_unless(exp == ret, retText, exp, ret);

    ruSetLogger(ruFileLogSink, RU_LOG_DBUG, rsc, true, true);
    ruInfoLogf("start in  '%s'", logM);

    for (int i = 0; i < thraedCount; i++) {
        rts[i] = ruThreadCreate(logr, NULL, NULL);
        fail_if(NULL == rts[i], retText, i, rts[i]);
    }

    sec_t end = ruTimeSec() + 20;
    while (thraedCount != closed && !ruTimeEllapsed(end)) ruSleepMs(1);
    fail_unless(thraedCount == closed, retText, thraedCount, closed);
    ruFlushLog();

    ruInfoLogf("end of '%s'", logM);
    ruStopLogger();
    ruCritLog("i must not be here!");
    if (rr) ruRegexFree(rr);
    ruSinkCtxFree(rsc);

    bool want = true;
    for (int i = 0; i < thraedCount; i++) {
        bool is = ruThreadWait(rts[i], 1,NULL);
        fail_unless(want == is, retText, want, is);
    }

    for (int i = 0; i < thraedCount; i++) {
        fail_unless(results[i][0], "did not get start of thread-%d", i+1);
        fail_unless(results[i][1], "did not get end of thread-%d", i+1);
    }

}
END_TEST

TCase* logTests(void) {
    TCase *tcase = tcase_create("log");
    tcase_add_test(tcase, filesink);
    tcase_add_test(tcase, cutoff);
    return tcase;
}
