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

bool quit = false;
bool finished = false;

static void reset() {
    quit = false;
    finished = false;
}

static ptr thRunner(ptr o) {
    perm_chars str = (perm_chars)o;
    ruDbgLogf("starting id: %ld param: '%s'", ruThreadGetId(), str);
    while(!quit) ruSleepUs(10);

    ruTraceLog("THREAD", 0);
    finished = true;
    ruDbgLogf("quitting %ld", ruThreadGetId());
    return (ptr)23;
}

START_TEST(api) {
    const char *test = "";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    int ret, exp;
    bool is, want;

    ruThread t1;
    t1 = ruThreadCreate(NULL, NULL, NULL);
    fail_unless(NULL == t1, retText, test, NULL, t1);

    t1 = ruThreadCreateBg(NULL, NULL, NULL);
    fail_unless(NULL == t1, retText, test, NULL, t1);

    exp = RUE_PARAMETER_NOT_SET;
    want = true;
    is = ruThreadFinished(t1, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(want == is, retText, test, want, is);

    want = false;
    is = ruThreadWait(t1, 1,NULL);
    fail_unless(want == is, retText, test, want, is);

    ret = ruThreadJoin(t1, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruThreadKill(t1);
    fail_unless(exp == ret, retText, test, exp, ret);
}
END_TEST

START_TEST(run) {
    const char *test = "";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    int ret, exp;
    bool is, want;
    ruThread t1;

    reset();
    t1 = ruThreadCreate(thRunner, NULL, NULL);
    fail_if(NULL == t1, retText, test, NULL, t1);

    exp = RUE_OK;
    want = false;
    is = ruThreadFinished(t1, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(want == is, retText, test, want, is);

    ruTraceLog("MAIN", 0);

    // stop it
    quit = true;
    ruSleepMs(100);

    want = true;
    is = ruThreadFinished(t1, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(want == is, retText, test, want, is);

    ptr exitRes = NULL;
    ret = ruThreadJoin(t1, &exitRes);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(23 == (intptr_t)exitRes, retText, test, 23, (intptr_t)exitRes);

    reset();
    t1 = ruThreadCreate(thRunner, NULL, "just kill");
    fail_if(NULL == t1, retText, test, NULL, t1);
    ruSleepMs(100);

    ret = ruThreadKill(t1);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_INVALID_PARAMETER;
    ret = ruThreadKill(t1);
    fail_unless(exp == ret, retText, test, exp, ret);

    reset();
    t1 = ruThreadCreate(thRunner, NULL, "wait kill");
    fail_if(NULL == t1, retText, test, NULL, t1);
    ruSleepMs(100);

    want = false;
    is = ruThreadWait(t1, 1,NULL);
    fail_unless(want == is, retText, test, want, is);

    want = false;
    is = ruThreadWait(t1, 1,NULL);
    fail_unless(want == is, retText, test, want, is);

}
END_TEST

volatile int shared_data = 0;
int ticks = 0;
int tocks = 0;
int max = 3;
int pos = 0;
int ticktocks[6];
ruMutex tmux = NULL;
ruCond tcond = NULL;

static void* producer(void* arg) {
    while (ticks < max) {
        ruVerbLog("lock start");
        ruMutexLock(tmux);
        while (!shared_data) {
            shared_data = 1;
            ticks++;
            ticktocks[pos++] = ticks;
            ruVerbLogf("pre signal ticks: %d tocks: %d", ticks, tocks);
            ruCondSignal(tcond);
            ruVerbLog("post signal");
        }
        ruMutexUnlock(tmux);
        ruVerbLog("unlocked");
        ruSleepMs(100);
    }
    return NULL;
}

void* consumer(void* arg) {
    while (-tocks < max) {
        ruVerbLog("lock start");
        ruMutexLock(tmux);
        ruVerbLogf("pre wait shared: %d", shared_data);
        int tries = 0;
        while (!shared_data) {
            ruCondWaitTil(tcond, tmux, 25);
            tries++;
            if (tries < 5 && !shared_data) {
                ruVerbLogf("post wait shared: %d tries: %d", shared_data, tries);
            } else if (shared_data){
                ruVerbLogf("post wait shared: %d tries: %d", shared_data, tries);
            }
        }
        tocks--;
        ticktocks[pos++] = tocks;
        shared_data = 0;
        ruVerbLogf("post wait has ticks: %d tocks: %d", ticks, tocks);
        ruMutexUnlock(tmux);
        ruVerbLog("unlocked");
    }
    return NULL;
}

START_TEST(conds) {
    const char *test = "";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    //int ret, exp;
    bool is, want;
    ruThread t1, t2;
    tmux = ruMutexInit();
    tcond = ruCondInit();
    memset(&ticktocks[0], 0, sizeof(ticktocks));
    alloc_chars tick = ruStrDup("tick");
    alloc_chars tock = ruStrDup("tock");

    t1 = ruThreadCreate(producer, tick, NULL);
    fail_if(NULL == t1, retText, test, NULL, t1);

    t2 = ruThreadCreate(consumer, tock, NULL);
    fail_if(NULL == t2, retText, test, NULL, t2);

    want = true;
    is = ruThreadWait(t1, 5, NULL);
    fail_unless(want == is, retText, test, want, is);

    is = ruThreadWait(t2, 1, NULL);
    fail_unless(want == is, retText, test, want, is);

    ruMutexFree(tmux);
    ruCondFree(tcond);

    int wants[] = {1, -1, 2, -2, 3, -3};
    int last = sizeof(ticktocks) / sizeof (ticktocks[0]);
    for (int i = 0; i < last; i++) {
        fail_unless(wants[i] == ticktocks[i], retText, test, wants[i], ticktocks[i]);
    }
}
END_TEST

TCase* threadTests(void) {
    TCase *tcase = tcase_create("thread");
    tcase_add_test(tcase, api);
    tcase_add_test(tcase, run);
    tcase_add_test(tcase, conds);
    return tcase;
}

