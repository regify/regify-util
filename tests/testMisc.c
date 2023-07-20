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

START_TEST ( mem ) {
    //int32_t ret;
    const char *test = "ruDupPrintf";
    const char *retText = "%s failed wanted ret '%s' but got '%s'";
    char *res = NULL;
    char *exp = NULL;

    /* get a context for logging */
    exp = NULL;
    res = ruDupPrintf("", NULL);
    fail_unless(res == exp, retText, test, exp, res);

    res = ruDupPrintf(NULL, NULL);
    fail_unless(res == exp, retText, test, exp, res);

    exp = "foo is 2";
    res = ruDupPrintf("foo is %d", 2);
    ck_assert_str_eq((char*)res, exp);
    ruFree(res);

    exp = "bar was foo";
    res = ruDupPrintf("bar %s %s", "was", "foo");
    ck_assert_str_eq((char*)res, exp);
    ruFree(res);

    test = "ruStrDup";
    res = ruStrDup(exp);
    ck_assert_str_eq((char*)res, exp);
    ruFree(res);

    exp = NULL;
    res = ruStrDup(exp);
    fail_unless(res == exp, retText, test, exp, res);

    test = "ruMalloc0";
    res = ruMalloc0(0, char);
    fail_unless(res == exp, retText, test, exp, res);

    res = ruMalloc0(1, char);
    fail_unless(res != 0, retText, test, exp, res);

    char *mymem = res;

    test = "ruRealloc";
    res = ruRealloc(NULL, 0, char);
    fail_unless(res == exp, retText, test, exp, res);

    res = ruRealloc(mymem, 0, char);
    fail_unless(res == exp, retText, test, exp, res);

    res = ruRealloc(NULL, 4, char);
    fail_unless(res == exp, retText, test, exp, res);

    res = ruRealloc(mymem, 14, char);
    fail_unless(res != 0, retText, test, exp, res);

    ruFree(res);

    test = "ruMemDup";
    void *in = "foo", *out = NULL;
    out = ruMemDup(NULL, 0);
    fail_unless(NULL == out, retText, test, NULL, out);

    out = ruMemDup(in, 0);
    fail_unless(NULL == out, retText, test, NULL, out);

    out = ruMemDup(NULL, 4);
    fail_unless(NULL == out, retText, test, NULL, out);

    out = ruMemDup(in, 4);
    fail_if(NULL == out, retText, test, NULL, out);
    ck_assert_str_eq(out, in);
    ruFree(out);

    test = "ruGetenv";
    res = (char*) ruGetenv(NULL);
    fail_unless(NULL == res, retText, test, NULL, res);

    res = (char*) ruGetenv("PATH");
    fail_if(NULL == res, retText, test, NULL, res);

    }
END_TEST

START_TEST ( misc ) {
    const char *test = "ruGetTimeVal";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    const char *zeroText = "%s failed got 0 for '%s'";
    ruTimeVal tv;
    int ret, exp = RUE_PARAMETER_NOT_SET;

    ret = ruGetTimeVal(NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    memset(&tv, 0, sizeof(ruTimeVal));
    ret = ruGetTimeVal(&tv);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_if(0 == tv.sec, zeroText, test, "tv.sec");
    fail_if(0 == tv.usec, zeroText, test, "tv.usec");

    test = "ruSetError";
    ruSetError("foo");
    ck_assert_str_eq("foo", ruLastError());

    ruSetError(NULL);
    ck_assert_str_eq("", ruLastError());

    ruSetError("foo #%d", 2);
    ck_assert_str_eq("foo #2", ruLastError());

    bool is, want = true;
    test = "ruIsInt64";
    is = ruIsInt64("123");
    fail_unless(want == is, retText, test, want, is);

    is = ruIsInt64(" 123");
    fail_unless(want == is, retText, test, want, is);

    is = ruIsInt64(" +123");
    fail_unless(want == is, retText, test, want, is);

    is = ruIsInt64(" -123");
    fail_unless(want == is, retText, test, want, is);

    want = false;
    is = ruIsInt64(NULL);
    fail_unless(want == is, retText, test, want, is);

    is = ruIsInt64(" *123");
    fail_unless(want == is, retText, test, want, is);

    is = ruIsInt64(" 123a  ");
    fail_unless(want == is, retText, test, want, is);

    is = ruIsInt64(" a123  ");
    fail_unless(want == is, retText, test, want, is);

    test = "ruSemiRandomNumber";
    ret = ruSemiRandomNumber(11, 0);
    fail_if(ret < 0, retText, test, 0, ret);
    fail_if(ret > 11, retText, test, 11, ret);

    ret = ruSemiRandomNumber(11, -6);
    fail_if(ret < -6, retText, test, -6, ret);
    fail_if(ret > 5, retText, test, 5, ret);

    test = "ruTimeFormat";
    char timeStr[20];
    char timeStr2[20];
    rusize stlen = sizeof(timeStr);
    memset(timeStr, 0, stlen);
    const char* format = "%Y-%m-%d %H:%M:%S";
    ruTimeFormat(NULL, stlen, timeStr, 0);
    ck_assert_str_eq(timeStr, "");

    ruTimeFormat(format, 0, timeStr, 0);
    ck_assert_str_eq(timeStr, "");

    ruTimeFormat(format, stlen, NULL, 0);
    ck_assert_str_eq(timeStr, "");

    ruTimeFormat(format, stlen, timeStr, 0);
    fail_unless(timeStr[2] == '2', retText, test, timeStr[2], '2');
    rusize alen = strlen(timeStr)+1;
    fail_unless(alen == stlen, retText, test, alen, stlen);

    sec_t estamp;
    sec_t stamp;
    sec_t localStamp;
    sec_t mystamp = 1234567890;
    perm_chars utcStr = "2009-02-13 23:31:30";

    // format utc for predictability
    ruUtcFormat(format, stlen, timeStr, mystamp);
    ck_assert_str_eq(timeStr, utcStr);

    // and back to stamp
    stamp = ruUtcParse(format, timeStr);
    fail_unless(mystamp == stamp, retText, test, mystamp, stamp);

    // get local TZ string
    ruTimeFormat(format, stlen, timeStr, mystamp);
    // at least in my timezone
    //ck_assert_str_eq(timeStr, "2009-02-14 00:31:30");

    // parse local string
    stamp = ruTimeParse(format, timeStr);
    fail_unless(mystamp == stamp, retText, test, mystamp, stamp);

    // get local TZ stamp
    localStamp = ruTimeUtcToLocal(mystamp);
    // should match local TZ string
    ruUtcFormat(format, stlen, timeStr2, localStamp);
    ck_assert_str_eq(timeStr, timeStr2);

    // go back to utc
    stamp = ruTimeLocalToUtc(localStamp);
    fail_unless(mystamp == stamp, retText, test, mystamp, stamp);

    // misc testing
    estamp = -1;
    stamp = ruTimeParse(NULL, NULL);
    fail_unless(estamp == stamp, retText, test, estamp, stamp);

    stamp = ruTimeParse(format, NULL);
    fail_unless(estamp == stamp, retText, test, estamp, stamp);

    stamp = ruTimeParse(NULL, timeStr);
    fail_unless(estamp == stamp, retText, test, estamp, stamp);

    test = "ruVersionComp";
    exp = 0;
    ret = ruVersionComp(NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruVersionComp("01", "1");
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruVersionComp("10.11.23-12324", "10.011.23-012324");
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = -1;
    ret = ruVersionComp(NULL, "0");
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruVersionComp("09", "10");
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruVersionComp("10.11.23-12324", "10.011.23-112324");
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = 1;
    ret = ruVersionComp("1", NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruVersionComp("109", "10");
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruVersionComp("10.11.24-12324", "10.011.23-112324");
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruGetLanguage";
    char *lang = ruGetLanguage();
    const char *expLang = "en";
    fail_unless(expLang, lang, retText, test, expLang, lang);
    ruFree(lang);

    char *out = ruGetHostname();
    fail_if(NULL == out, retText, test, NULL, out);
    ruFree(out);

}
END_TEST

START_TEST ( mux ) {
    ruMutex mx = ruMutexInit();
    ruMutexLock(mx);
    ruMutexUnlock(mx);
    ruMutexFree(mx);
    ruMutexFree(mx);
}
END_TEST

START_TEST(counter) {
    const char *retText = "failed wanted ret '%d' but got '%d'";
    ruCount rc = NULL;
    int32_t ret, exp = RUE_PARAMETER_NOT_SET;
    int64_t val, want = 0;

    val = ruCountSetValue(rc, 23, &ret);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(want == val, retText, want, val);

    val = ruCounterIncValue(rc, 23, &ret);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(want == val, retText, want, val);

    perm_chars foo = "foo was here";
    exp = RUE_INVALID_PARAMETER;
    val = ruCounterIncValue((ruCount)foo, 23, &ret);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(want == val, retText, want, val);

    rc = ruCountFree(rc);
    fail_unless(NULL == rc, retText, NULL, rc);

    want = 42;
    exp = RUE_OK;
    rc = ruCounterNew(want);
    fail_if(NULL == rc, retText, NULL, rc);

    val = ruCountSetValue(rc, 23, &ret);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(want == val, retText, want, val);

    want = 23;
    val = ruCountSet(rc, 23);
    fail_unless(want == val, retText, want, val);

    want = 25;
    val = ruCounterIncValue(rc, 2, &ret);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(want == val, retText, want, val);

    want = 23;
    val = ruCounterInc(rc, -2);
    fail_unless(want == val, retText, want, val);

    val = ruCounterRead(rc);
    fail_unless(want == val, retText, want, val);

    rc = ruCountFree(rc);
    fail_unless(NULL == rc, retText, NULL, rc);
}
END_TEST

START_TEST(process) {
    // these tests assume cygwin root at c:\ for windows
    const char *test = "ruRunProg";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    int ret, exp = RUE_RUN_FAILED;
    const char* command[] = {"true", NULL, NULL};
#ifdef ITS_OSX
#define bindir  "/usr/bin/"
#else
#define bindir  "/bin/"
#endif
    ret = ruRunProg(command, RU_NO_TIMEOUT);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = 0;
    command[0] = bindir "true";
    ret = ruRunProg(command, RU_NO_TIMEOUT);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruRunProg(command, RU_NON_BLOCK);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = 1;
    command[0] = bindir "false";
    ret = ruRunProg(command, RU_NO_TIMEOUT);
    fail_unless(exp == ret, retText, test, exp, ret);

    // not really specific
    //ret = ruRunProg(command, RU_NON_BLOCK);
    //fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_TIMEOUT;
    command[0] = "/bin/sleep";
    command[1] = "5";
    ret = ruRunProg(command, 1);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = 0;
    command[1] = "1";
    ret = ruRunProg(command, 3);
    fail_unless(exp == ret, retText, test, exp, ret);

    command[1] = "60";
    // this guy should linger after these test have been completed
    ret = ruRunProg(command, RU_NON_BLOCK);
    fail_unless(exp == ret, retText, test, exp, ret);
}
END_TEST

typedef struct optest_ {
    char* opts;
    int argc;
    char* argv[10];
} optest;

START_TEST (getoptmap) {
    int32_t ret, exp, argc;
    perm_chars retText = "failed wanted ret '%d' but got '%d'";
    ruMap params = NULL;
    optest t1 = {"a:", 3, {[1]="-a", "opta"}};
    optest* ot = &t1;

    argc = 0;
    exp = RUE_PARAMETER_NOT_SET;
    ret = ruGetOptMap(NULL, NULL, argc, NULL);
    fail_unless(exp == ret, retText, exp, ret);

    ret = ruGetOptMap(NULL, NULL, ot->argc, ot->argv);
    fail_unless(exp == ret, retText, exp, ret);

    ret = ruGetOptMap(NULL, ot->opts, ot->argc, ot->argv);
    fail_unless(exp == ret, retText, exp, ret);

    exp = RUE_OK;
    ret = ruGetOptMap(&params, ot->opts, ot->argc, ot->argv);
    fail_unless(exp == ret, retText, exp, ret);

    perm_chars val = NULL;
    ret = ruMapGet(params, "a", &val);
    fail_unless(exp == ret, retText, exp, ret);
    ck_assert_str_eq("opta", val);

    params = ruMapFree(params);

    optest t2 = {"ab:", 3, {[1]="-ab", "foo"}};
    ot = &t2;
    ret = ruGetOptMap(&params, ot->opts, ot->argc, ot->argv);
    fail_unless(exp == ret, retText, exp, ret);

    bool want = true;
    bool is = ruMapHas(params, "a", &ret);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(want == is, retText, want, is);

    val = NULL;
    ret = ruMapGet(params, "b", &val);
    fail_unless(exp == ret, retText, exp, ret);
    ck_assert_str_eq("foo", val);

    params = ruMapFree(params);

    optest t3 = {"a", 3, {[1]="arg", "-a"}};
    ot = &t3;
    exp = RUE_FILE_NOT_FOUND;
    ret = ruGetOptMap(&params, ot->opts, ot->argc, ot->argv);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(params == NULL, retText, params, NULL);

    optest t4 = {"ab:", 4, {[1]="-c", "foo", "-a"}};
    ot = &t4;
    exp = RUE_INVALID_PARAMETER;
    ret = ruGetOptMap(&params, ot->opts, ot->argc, ot->argv);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(params == NULL, retText, params, NULL);

    optest t5 = {"ab:", 4, {[1]="-a", "-c", "foo"}};
    ot = &t5;
    exp = RUE_INVALID_PARAMETER;
    ret = ruGetOptMap(&params, ot->opts, ot->argc, ot->argv);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(params == NULL, retText, params, NULL);
}
END_TEST


#ifdef _WIN32
START_TEST ( reg ) {
    perm_chars test = "ruGetRegistryEntry";
    perm_chars retText = "%s failed wanted ret '%d' but got '%d'";
    char *want, *out;
    int ret, exp;

    // Note these legacy tests will only pass on a system that has cygwin
    // installed with root in c:/, i.e. my system --mario
    exp = RUE_PARAMETER_NOT_SET;
    ret = ruGetRegistryEntry(0, NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruGetRegistryEntry(HKEY_LOCAL_MACHINE, "SOFTWARE\\Cygwin\\setup",
                             "rootdir", NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruGetRegistryEntry(HKEY_LOCAL_MACHINE, "SOFTWARE\\Cygwin\\setup",
                             NULL, &out);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruGetRegistryEntry(HKEY_LOCAL_MACHINE, NULL,
                             "rootdir", &out);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruGetRegistryEntry(0, "SOFTWARE\\Cygwin\\setup",
                             "rootdir", &out);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    want = "c:\\";
    ret = ruGetRegistryEntry(HKEY_LOCAL_MACHINE, "SOFTWARE\\Cygwin\\setup",
                             "rootdir", &out);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq(want, out);
    ruFree(out);

    want = "1";
    ret = ruGetRegistryEntry(HKEY_LOCAL_MACHINE, "SYSTEM\\Select",
                             "Default", &out);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq(want, out);
    ruFree(out);

}
END_TEST

START_TEST ( vol ) {
    const char *test = "ruGetVolumeInfo";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    int ret, exp;

    exp = RUE_PARAMETER_NOT_SET;
    ret = ruGetVolumeInfo(NULL, NULL, NULL,
                          NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_FILE_NOT_FOUND;
    ret = ruGetVolumeInfo("foo", NULL, NULL,
                          NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    ret = ruGetVolumeInfo("c:\\", NULL, NULL,
            NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    u_long serial = 0, compLen = 0, flags = 0;
    char *fsName = NULL;
    ret = ruGetVolumeInfo("c:\\", &serial, &compLen,
                          &flags, NULL, &fsName);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_if(0 == serial, retText, test, 0, serial);
    fail_unless(255 == compLen, retText, test, 255, compLen);
    fail_if(0 == flags, retText, test, 0, flags);
    fail_if(NULL == fsName, retText, test, NULL, fsName);
    ruFree(fsName);

}
END_TEST

typedef struct optestw_ {
    char* opts;
    DWORD argc;
    LPWSTR argv[10];
} optestw;

START_TEST (getoptmapw) {
    int32_t ret, exp;
    DWORD argc;
    perm_chars retText = "failed wanted ret '%d' but got '%d'";
    ruMap params = NULL;
    optestw t1 = {"a:", 3, {[1]=L"-a", L"opta"}};
    optestw* ot = &t1;

    argc = 0;
    exp = RUE_PARAMETER_NOT_SET;
    ret = ruGetOptMapW(NULL, NULL, argc, NULL);
    fail_unless(exp == ret, retText, exp, ret);

    ret = ruGetOptMapW(NULL, NULL, ot->argc, ot->argv);
    fail_unless(exp == ret, retText, exp, ret);

    ret = ruGetOptMapW(NULL, ot->opts, ot->argc, ot->argv);
    fail_unless(exp == ret, retText, exp, ret);

    exp = RUE_OK;
    ret = ruGetOptMapW(&params, ot->opts, ot->argc, ot->argv);
    ret = ruGetOptMapW(&params, t1.opts, t1.argc, t1.argv);
    fail_unless(exp == ret, retText, exp, ret);

    perm_chars val = NULL;
    ret = ruMapGet(params, "a", &val);
    fail_unless(exp == ret, retText, exp, ret);
    ck_assert_str_eq("opta", val);

    params = ruMapFree(params);

    optestw t2 = {"ab:", 3, {[1]=L"-ab", L"foo"}};
    ot = &t2;
    ret = ruGetOptMapW(&params, ot->opts, ot->argc, ot->argv);
    fail_unless(exp == ret, retText, exp, ret);

    bool want = true;
    bool is = ruMapHas(params, "a", &ret);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(want == is, retText, want, is);

    val = NULL;
    ret = ruMapGet(params, "b", &val);
    fail_unless(exp == ret, retText, exp, ret);
    ck_assert_str_eq("foo", val);

    params = ruMapFree(params);

    optestw t3 = {"a", 3, {[1]=L"arg", L"-a"}};
    ot = &t3;
    exp = RUE_FILE_NOT_FOUND;
    ret = ruGetOptMapW(&params, ot->opts, ot->argc, ot->argv);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(params == NULL, retText, params, NULL);

    optestw t4 = {"ab:", 4, {[1]=L"-c", L"foo", L"-a"}};
    ot = &t4;
    exp = RUE_INVALID_PARAMETER;
    ret = ruGetOptMapW(&params, ot->opts, ot->argc, ot->argv);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(params == NULL, retText, params, NULL);

    optestw t5 = {"ab:", 4, {[1]=L"-a", L"-c", L"foo"}};
    ot = &t5;
    exp = RUE_INVALID_PARAMETER;
    ret = ruGetOptMapW(&params, ot->opts, ot->argc, ot->argv);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(params == NULL, retText, params, NULL);
}
END_TEST

#endif


TCase* miscTests(void) {
    TCase *tcase = tcase_create("misc");
    tcase_add_test(tcase, mem);
    tcase_add_test(tcase, misc);
    tcase_add_test(tcase, mux);
    tcase_add_test(tcase, counter);
    tcase_add_test(tcase, process);
    tcase_add_test(tcase, getoptmap);
#ifdef _WIN32
    tcase_add_test ( tcase, reg );
    tcase_add_test ( tcase, vol );
    tcase_add_test(tcase, getoptmapw);
#endif
    return tcase;
}

