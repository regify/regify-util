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

    test = "ruStrdup";
    res = ruStrdup(exp);
    ck_assert_str_eq((char*)res, exp);
    ruFree(res);

    exp = NULL;
    res = ruStrdup(exp);
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

#ifdef _WIN32
START_TEST ( reg ) {
    const char *test = "ruGetRegistryEntry";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    char *want, *out;
    int ret, exp;

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
    want = "C:\\";
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
#endif


TCase* miscTests ( void ) {
    TCase *tcase = tcase_create ( "misc" );
    tcase_add_test ( tcase, mem );
    tcase_add_test ( tcase, misc );
    tcase_add_test ( tcase, mux );
#ifdef _WIN32
    tcase_add_test ( tcase, reg );
    tcase_add_test ( tcase, vol );
#endif
    return tcase;
}

