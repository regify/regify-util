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

START_TEST ( api ) {
    int32_t ret, exp;
    const char *test;
    const char *retText = "%s failed wanted ret '%d' but got '%d'";

    ruIni cf = ruIniNew();

    exp = RUE_PARAMETER_NOT_SET;
    test = "ruIniGet";
    ret = ruIniGet(NULL, NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniGet(cf, NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniGet(cf, NULL, "foo", NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    const char* res = NULL;
    test = "ruIniGetDef";
    res = ruIniGetDef(NULL, NULL, NULL, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    res = NULL;
    res = ruIniGetDef(NULL, NULL, NULL, "bar", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    res = NULL;
    res = ruIniGetDef(cf, NULL, NULL, "bar", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruIniSet";
    ret = ruIniSet(NULL, NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniSet(cf, NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniSet(cf, NULL, NULL, test);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruIniKeys";
    ret = ruIniKeys(NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniKeys(cf, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniKeys(cf, "foo", NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruIniSections";
    ret = ruIniSections(NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniSections(cf, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruIniRead";
    ret = ruIniRead(NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniRead("foo", NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniRead(NULL, cf);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruIniWrite";
    ret = ruIniWrite(NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniWrite(cf, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniWrite(NULL, "foo");
    fail_unless(exp == ret, retText, test, exp, ret);

    cf = ruIniFree(cf);
    fail_unless(NULL == cf, retText, test, NULL, cf);

    cf = ruIniFree(cf);
    fail_unless(NULL == cf, retText, test, NULL, cf);

    ruIniFree(NULL);
}
END_TEST

START_TEST ( run ) {
    int32_t ret, exp;
    const char *test;
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    const char *sec = "section", *key = "key", *val = "value";
    char* iniDir = insureTestFolder("iniRun");
    char* inifile = ruPathJoin(iniDir, "run.ini");

    ruIni cf = ruIniNew();

    exp = RUE_OK;
    test = "ruIniSet";
    ret = ruIniSet(cf, NULL, key, val);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniSet(cf, sec, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniSet(cf, sec, key, val);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniSet(cf, sec, key, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIniSet(cf, sec, key, val);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruIniWrite";
    ret = ruIniWrite(cf, inifile);
    fail_unless(exp == ret, retText, test, exp, ret);

    cf = ruIniFree(cf);
    fail_unless(NULL == cf, retText, test, NULL, cf);

    test = "ruIniWrite";
    ret = ruIniRead(inifile, &cf);
    fail_unless(exp == ret, retText, test, exp, ret);

    ruList lst = NULL;
    rusize sz, esz = 1;
    test = "ruIniKeys";
    ret = ruIniKeys(cf, NULL, &lst);
    fail_unless(exp == ret, retText, test, exp, ret);

    sz = ruListSize(lst, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    char* item = NULL;
    item = ruListIdxData(lst, 0, char*, &ret);
    ck_assert_str_eq(key, item);

    const char* res = NULL;
    test = "ruIniGet";
    ret = ruIniGet(cf, NULL, item, &res);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq(val, res);
    lst = ruListFree(lst);

    test = "ruIniGetDef";
    res = NULL;
    res = ruIniGetDef(cf, NULL, "bar", "default", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq("default", res);

    test = "ruIniSections";
    ret = ruIniSections(cf, &lst);
    fail_unless(exp == ret, retText, test, exp, ret);

    sz = ruListSize(lst, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    item = NULL;
    item = ruListIdxData(lst, 0, char*, &ret);
    ck_assert_str_eq(sec, item);
    lst = ruListFree(lst);

    test = "ruIniKeys";
    ret = ruIniKeys(cf, sec, &lst);
    fail_unless(exp == ret, retText, test, exp, ret);

    sz = ruListSize(lst, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    item = NULL;
    item = ruListIdxData(lst, 0, char*, &ret);
    ck_assert_str_eq(key, item);
    lst = ruListFree(lst);

    res = NULL;
    test = "ruIniGetDef";
    res = ruIniGetDef(cf, sec, key, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq(val, res);

    res = NULL;
    test = "ruIniGet";
    ret = ruIniGet(cf, sec, key, &res);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq(val, res);

    cf = ruIniFree(cf);
    fail_unless(NULL == cf, retText, test, NULL, cf);

    ruFree(iniDir);
    ruFree(inifile);
}
END_TEST
TCase* iniTests ( void ) {
    TCase *tcase = tcase_create ( "ini" );
    tcase_add_test ( tcase, api );
    tcase_add_test ( tcase, run );
    return tcase;
}
