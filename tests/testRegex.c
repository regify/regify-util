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
    const char *test = "ruRegexNew";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";

    ruRegex rr = ruRegexNew(NULL, 0, NULL);
    fail_unless(rr == NULL, retText, test, NULL, rr);

    exp = RUE_PARAMETER_NOT_SET;
    rr = ruRegexNew(NULL, 0, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rr == NULL, retText, test, NULL, rr);

    exp = RUE_INVALID_PARAMETER;
    rr = ruRegexNew("", 0, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rr == NULL, retText, test, NULL, rr);

    rr = ruRegexNew("(", 0, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rr == NULL, retText, test, NULL, rr);

    char *rep = ruRegexReplace(NULL, NULL, NULL, NULL);
    fail_unless(rep == NULL, retText, test, NULL, rep);

    exp = RUE_PARAMETER_NOT_SET;
    rep = ruRegexReplace(NULL, NULL, NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rep == NULL, retText, test, NULL, rep);

    rep = ruRegexReplace(NULL, "foo", NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rep == NULL, retText, test, NULL, rep);

    rep = ruRegexReplace(NULL, NULL, "bar", &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rep == NULL, retText, test, NULL, rep);

    bool doexp = false;
    bool does = ruRegexMatch(NULL, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruRegexMatch(NULL, "foo", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruRegexFind(NULL, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruRegexFind(NULL, "foo", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    ruList matches = NULL;
    matches = ruRegexMatchGroups(NULL, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(NULL == matches, retText, test, NULL, matches);

    matches = ruRegexMatchGroups(NULL, "foo", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(NULL == matches, retText, test, NULL, matches);

    matches = ruRegexFindGroups(NULL, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(NULL == matches, retText, test, NULL, matches);

    matches = ruRegexFindGroups(NULL, "foo", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(NULL == matches, retText, test, NULL, matches);

    exp = RUE_OK;
    rr = ruRegexNew("foo", 0, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_if(rr == NULL, retText, test, NULL, rr);

    exp = RUE_PARAMETER_NOT_SET;
    rep = ruRegexReplace(rr, NULL, NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rep == NULL, retText, test, NULL, rep);

    does = ruRegexMatch(rr, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruRegexFind(rr, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    matches = ruRegexMatchGroups(rr, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(NULL == matches, retText, test, NULL, matches);

    matches = ruRegexFindGroups(rr, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(NULL == matches, retText, test, NULL, matches);

    matches = ruRegexMatchGroups(rr, "bar", NULL);
    fail_unless(NULL == matches, retText, test, NULL, matches);

    matches = ruRegexFindGroups(rr, "bar", NULL);
    fail_unless(NULL == matches, retText, test, NULL, matches);

    ruRegexFree(rr);
}
END_TEST

START_TEST ( run ) {
    int32_t ret, exp;
    const char *test = "ruRegexNew";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";

    exp = RUE_OK;
    ruRegex rr = ruRegexNew("@MANNAME@", 0, &ret);
            fail_unless(ret == exp, retText, test, exp, ret);
    fail_if(rr == NULL, retText, test, NULL, rr);

    char *orig = "who's your @MANNAME@?";
    char *repexp = "who's your foo?";

    char *rep = ruRegexReplace(rr, orig, "foo", &ret);
            fail_unless(ret == exp, retText, test, exp, ret);
    ck_assert_str_eq(rep, repexp);
    ruFree(rep);

    bool doexp = false;
    bool does = ruRegexMatch(rr, "", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruRegexFind(rr, "", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    doexp = true;
    does = ruRegexFind(rr, orig, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    orig = "@MANNAME@";
    does = ruRegexMatch(rr, orig, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    orig = "";
    repexp = "";

    rep = ruRegexReplace(rr, orig, "foo", &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    ck_assert_str_eq(rep, repexp);
    ruFree(rep);
    ruRegexFree(rr);

}
END_TEST

START_TEST ( groups ) {
    int32_t ret, exp;
    const char *test = "ruRegexNew";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";

    exp = RUE_OK;
    ruRegex rr = ruRegexNew(".*@(\\w+)@.*", RUREGEX_CASE_INSENSITIVE, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_if(rr == NULL, retText, test, NULL, rr);

    char *orig = "who's your @foo@?";
    char *foo = "foo";
    ruList matches = NULL;

    test = "ruRegexFindGroups";
    matches = ruRegexFindGroups(rr, orig, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_if(NULL == matches, retText, test, NULL, matches);

    int32_t expSz = 2, sz = ruListSize(matches, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(expSz == sz, retText, test, expSz, sz);

    char* entry = ruListIdxData(matches, 0, char*, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq(entry, orig);

    entry = ruListIdxData(matches, 1, char*, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq(entry, foo);

    matches = ruListFree(matches);

    test = "ruRegexMatchGroups";
    matches = ruRegexMatchGroups(rr, orig, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_if(NULL == matches, retText, test, NULL, matches);

    sz = ruListSize(matches, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(expSz == sz, retText, test, expSz, sz);

    entry = ruListIdxData(matches, 0, char*, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq(entry, orig);

    entry = ruListIdxData(matches, 1, char*, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq(entry, foo);

    matches = ruListFree(matches);

    ruRegexFree(rr);

}
END_TEST

TCase* regexTests ( void ) {
    TCase *tcase = tcase_create ( "regex" );
    tcase_add_test ( tcase, api );
    tcase_add_test ( tcase, run );
    tcase_add_test ( tcase, groups );
    return tcase;
}
