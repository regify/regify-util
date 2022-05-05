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
    const char *test = "ruStringNewf";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";

    ruString rs = ruStringNewf(NULL, NULL);
    fail_unless(rs == NULL, retText, test, NULL, rs);

    rs = ruStringNew(NULL);
    fail_unless(rs == NULL, retText, test, NULL, rs);

    ruStringFree(rs, false);

    exp = RUE_PARAMETER_NOT_SET;
    ret = ruStringAppend(rs, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruStringAppendf(rs, NULL, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    char *foo = ruStringGetCString(rs);
    fail_unless(foo == NULL, retText, test, NULL, foo);

    rusize explen = 0;
    rusize len = ruStringLen(NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(explen == len, retText, test, explen, len);

    bool doexp = false;
    bool does = ruStringEndsWith(NULL, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    foo = ruLastSubstr(NULL, NULL);
    fail_unless(foo == NULL, retText, test, NULL, foo);

    char *subst = "test^^^^foo";
    foo = ruLastSubstr(subst, NULL);
    fail_unless(foo == NULL, retText, test, NULL, foo);

    foo = ruLastSubstr(NULL, "^^");
    fail_unless(foo == NULL, retText, test, NULL, foo);

    int64_t num = ruStrToll(NULL, NULL, 0);
    fail_unless(0 == num, retText, test, 0, num);
    fail_unless(EINVAL == errno, retText, test, EINVAL, errno);

    num = ruStrToll("", NULL, 0);
    fail_unless(0 == num, retText, test, 0, num);

    ruStripChars(NULL, NULL);
    ruStripChars("", NULL);
    ruStripChars("", "");
    ruStripChars(" ", "");
    ruStripChars("", " ");

}
END_TEST

START_TEST ( run ) {
    int32_t ret, exp;
    const char *test = "ruStringNewf";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";

    char *want = "foo was here";
    ruString rs = ruStringNewf("%s was here", "foo");
    fail_if(rs == NULL, retText, test, NULL, rs);
    ck_assert_str_eq(ruStringGetCString(rs), want);

    exp = RUE_OK;
    test = "ruStringAppendf";
    want = "foo was here and bar was not";
    ret = ruStringAppendf(rs, " and %s was not", "bar");
    fail_unless(ret == exp, retText, test, exp, ret);
    ck_assert_str_eq(ruStringGetCString(rs), want);
    ruStringFree(rs, false);

    want = "foo";
    test = "ruStringNewn";
    rs = ruStringNewn("foo", 2);
    fail_if(rs == NULL, retText, test, NULL, rs);
    ck_assert_str_eq(ruStringGetCString(rs), want);

    rusize explen = 3;
    rusize len = ruStringLen(rs, &ret);
    fail_unless(explen == len, retText, test, explen, len);

    ruStringFree(rs, false);

    want = "";
    rs = ruStringNewn(NULL, 2);
    fail_if(rs == NULL, retText, test, NULL, rs);
    ck_assert_str_eq(ruStringGetCString(rs), want);

    want = "foo";
    test = "ruStringAppend";
    ret = ruStringAppend(rs, (const char*)want);
    fail_unless(ret == exp, retText, test, exp, ret);
    ck_assert_str_eq(ruStringGetCString(rs), want);

    want = "foo &";
    test = "ruStringAppendn";
    ret = ruStringAppendn(rs, " & bar", 2);
    fail_unless(ret == exp, retText, test, exp, ret);
    char *str = ruStringGetCString(rs);
    ck_assert_str_eq(str, want);

    explen = 5;
    test = "ruStringLen";
    len = ruStringLen(rs, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(explen == len, retText, test, explen, len);

    test = "ruStringAppendUriEncoded";
    want = "foo &%20f%c3%bcr%20bar";
    ret = ruStringAppendUriEncoded(rs, " für bar");
    fail_unless(ret == exp, retText, test, exp, ret);
    str = ruStringGetCString(rs);
    ck_assert_str_eq(str, want);

    exp = RUE_PARAMETER_NOT_SET;
    test = "ruStringEndsWith";
    bool doexp = false;
    bool does = ruStringEndsWith(rs, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    exp = RUE_OK;
    does = ruStringEndsWith(rs, "foo", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    doexp = true;
    does = ruStringEndsWith(rs, "bar", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(doexp == does, retText, test, doexp, does);

    ruStringFree(rs, true);
    ruFree(str);

    test = "ruLastSubstr";
    char *subst = "test^^^^foo";
    char *ptr = ruLastSubstr(subst, "^^");
    ck_assert_str_eq(ptr, "^^foo");

    ptr = ruLastSubstrLen(subst, "^^", 6);
    ck_assert_str_eq(ptr, "^^^^foo");

    ptr = ruLastSubstrLen(subst, "^^", 5);
    fail_unless(NULL == ptr, retText, test, NULL, ptr);

    subst = "test^foo";
    ptr = ruLastSubstr(subst, "^^");
    fail_unless(NULL == ptr, retText, test, NULL, ptr);

    ptr = ruLastSubstr(subst, "");
    ck_assert_str_eq(ptr, subst);

    test = "ruStrToll";
    int64_t expNum = 666;
    int64_t num = ruStrToll(" 0666NULL", NULL, 10);
    fail_unless(expNum == num, retText, test, expNum, num);

    expNum = -666;
    num = ruStrToll("-66,6.5", NULL, 10);
    fail_unless(expNum == num, retText, test, expNum, num);

    test = "ruUtf8CaseNormalize";
    ptr = ruUtf8CaseNormalize(NULL, 0, 0);
    fail_unless(NULL == ptr, retText, test, NULL, ptr);

    ptr = ruUtf8CaseNormalize(subst, 0, 0);
    ck_assert_str_eq(subst, ptr);
    ruFree(ptr);

    char *expstr = "TEST^FOO";
    ptr = ruUtf8CaseNormalize(subst, 0, ruUtf8Upper);
    ck_assert_str_eq(expstr, ptr);
    ruFree(ptr);

    ptr = ruUtf8CaseNormalize(subst, 57, ruUtf8Upper);
    fail_unless(NULL == ptr, retText, test, NULL, ptr);

    subst = "T\xc3\xa4st"; // Täst
    expstr = "ta\xcc\x88st"; // Täst
    ptr = ruUtf8CaseNormalize(subst, ruUtf8Nfd, ruUtf8Lower);
    ck_assert_str_eq(expstr, ptr);
/*
    // these normalizers seem to lack in our icu builds
    ruFree(ptr);
    ptr = ruUtf8CaseNormalize(subst, ruUtf8Nfkd, ruUtf8Lower);
    ck_assert_str_eq(expstr, ptr);

    char *alstr = ptr;
    expstr = "T\xc3\x84ST"; // Täst
    ptr = ruUtf8CaseNormalize(alstr, ruUtf8Nfkc, ruUtf8Upper);
    ck_assert_str_eq(expstr, ptr);
    ruFree(ptr);
*/
    char *alstr = ptr;
    expstr = "T\xc3\x84ST"; // Täst
    ptr = ruUtf8CaseNormalize(alstr, ruUtf8Nfc, ruUtf8Upper);
    ck_assert_str_eq(expstr, ptr);
    ruFree(ptr);
    ruFree(alstr);

}
END_TEST

START_TEST ( util ) {
    int32_t want, got;
    const char *test = "";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";


    test = "ruStrcmp";
    want = 0;
    got = ruStrcmp(NULL, NULL);
    fail_unless(got == want, retText, test, want, got);

    want = 1;
    got = ruStrcmp("foo", NULL);
    fail_unless(got == want, retText, test, want, got);

    want = -1;
    got = ruStrcmp(NULL, "foo");
    fail_unless(got == want, retText, test, want, got);

    want = 0;
    got = ruStrcmp("foo", "foo");
    fail_unless(got == want, retText, test, want, got);

    want = -1;
    got = ruStrcmp("bar", "foo");
    fail_unless(got == want, retText, test, want, got);

    test = "ruStrcasecmp";
    want = 0;
    got = ruStrcasecmp(NULL, NULL);
    fail_unless(got == want, retText, test, want, got);

    want = 1;
    got = ruStrcasecmp("foo", NULL);
    fail_unless(got == want, retText, test, want, got);

    want = -1;
    got = ruStrcasecmp(NULL, "foo");
    fail_unless(got == want, retText, test, want, got);

    want = 0;
    got = ruStrcasecmp("foo", "Foo");
    fail_unless(got == want, retText, test, want, got);

    want = -1;
    got = ruStrcasecmp("bar", "foo");
    fail_unless(got == want, retText, test, want, got);

    test = "ruStrStartsWith";
    want = RUE_PARAMETER_NOT_SET;
    bool doexp = false;
    bool does = ruStrStartsWith(NULL, NULL, &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruStrStartsWith("foobar", NULL, &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruStrStartsWith(NULL, "bar", &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    want = RUE_OK;
    doexp = true;
    does = ruStrStartsWith("foobar", "", &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruStrStartsWith("foobar", "foo", &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    doexp = false;
    does = ruStrStartsWith("foobar", "oo", &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    test = "ruStrEndsWith";
    want = RUE_PARAMETER_NOT_SET;
    doexp = false;
    does = ruStrEndsWith(NULL, NULL, &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruStrEndsCaseWith(NULL, NULL, &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruStrEndsWith("foobar", NULL, &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruStrEndsWith(NULL, "bar", &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    want = RUE_OK;
    doexp = true;
    does = ruStrEndsWith("foobar", "", &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruStrEndsWith("foobar", "bar", &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruStrEndsCaseWith("fooBaR", "bar", &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    doexp = false;
    does = ruStrEndsWith("foobar", "ba", &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruStrEndsCaseWith("fooBaR", "ba", &got);
    fail_unless(want == got, retText, test, want, got);
    fail_unless(doexp == does, retText, test, doexp, does);

    test = "ruStrStrLen";
    char *exStr = NULL;
    char *str = ruStrStr(NULL, NULL);
    fail_unless(exStr == str, retText, test, exStr, str);

    str = ruStrStrLen(NULL, NULL, 0);
    fail_unless(exStr == str, retText, test, exStr, str);

    str = ruStrStr("foo", NULL);
    fail_unless(exStr == str, retText, test, exStr, str);

    str = ruStrStrLen("foo", NULL, 0);
    fail_unless(exStr == str, retText, test, exStr, str);

    str = ruStrStr(NULL, "bar");
    fail_unless(exStr == str, retText, test, exStr, str);

    str = ruStrStrLen(NULL, "bar", 0);
    fail_unless(exStr == str, retText, test, exStr, str);

    want = RUE_OK;
    exStr = "foobar";
    str = ruStrStr("foobar", "");
    ck_assert_str_eq(exStr, str);

    str = ruStrStrLen("foobar", "", 0);
    ck_assert_str_eq(exStr, str);

    exStr = "bar";
    str = ruStrStr("foobar", "bar");
    ck_assert_str_eq(exStr, str);

    str = ruStrStrLen("foobar", "bar", 6);
    ck_assert_str_eq(exStr, str);

    str = ruStrStrLen("foobar", "ba", 5);
    ck_assert_str_eq(exStr, str);

    exStr = NULL;
    str = ruStrStrLen("foobar", "bar", 5);
    fail_unless(exStr == str, retText, test, exStr, str);

    test = "ruStrsplit";
    ruList rl = ruStrsplit(NULL, NULL, 0);
    fail_unless(NULL == rl, retText, test, NULL, rl);

    const char *istr = "foo:bar", *delim = ":";
    rl = ruStrsplit(NULL, NULL, 0);
    fail_unless(NULL == rl, retText, test, NULL, rl);

    rl = ruStrsplit(istr, NULL, 0);
    fail_unless(NULL == rl, retText, test, NULL, rl);

    rl = ruStrsplit(istr, "", 0);
    fail_unless(NULL == rl, retText, test, NULL, rl);

    want = 1;
    rl = ruStrsplit(istr, delim, 1);
    fail_if(NULL == rl, retText, test, NULL, rl);
    got = ruListSize(rl, NULL);
    fail_unless(want == got, retText, test, want, got);
    ruListFree(rl);

    want = 2;
    rl = ruStrsplit(istr, delim, 0);
    fail_if(NULL == rl, retText, test, NULL, rl);
    got = ruListSize(rl, NULL);
    fail_unless(want == got, retText, test, want, got);
    ruListFree(rl);

    istr = ":foo:bar:";
    want = 4;
    rl = ruStrsplit(istr, delim, 0);
    fail_if(NULL == rl, retText, test, NULL, rl);
    got = ruListSize(rl, NULL);
    fail_unless(want == got, retText, test, want, got);
    ruListFree(rl);

    test = "ruStrAciiToLower";
    str = "!Foo¡";
    exStr = ruAsciiToLower(NULL);
    fail_unless(NULL == exStr, retText, test, NULL, exStr);

    exStr = ruAsciiToLower(str);
    ck_assert_str_eq("!foo¡", exStr);
    ruFree(exStr);

    str = "!FÖO¡";
    exStr = ruAsciiToLower(str);
    ck_assert_str_eq("!fÖo¡", exStr);
    ruFree(exStr);

    str = "!F0O¡";
    exStr = ruAsciiNToLower(str, 4);
    ck_assert_str_eq("!f0o", exStr);
    ruFree(exStr);

    test = "ruStrAciiToUpper";
    str = "ßFoo";
    exStr = ruAsciiToUpper(NULL);
    fail_unless(NULL == exStr, retText, test, NULL, exStr);

    exStr = ruAsciiToUpper(str);
    ck_assert_str_eq("ßFOO", exStr);
    ruFree(exStr);

    str = "¡füo";
    exStr = ruAsciiToUpper(str);
    ck_assert_str_eq("¡FüO", exStr);
    ruFree(exStr);

    str = "af3o";
    exStr = ruAsciiNToUpper(str, 3);
    ck_assert_str_eq("AF3", exStr);
    ruFree(exStr);

    str = "!FÖO¡";
    exStr = ruUtf8ToLower(str);
    ck_assert_str_eq("!föo¡", exStr);

    char *out2 = ruUtf8ToUpper(exStr);
    ck_assert_str_eq("!FÖO¡", out2);

    ruFree(exStr);
    ruFree(out2);
    str = ruStrdup("foo was\nhere");
    ruStripChars(str, "\n ");
    ck_assert_str_eq(str, "foowashere");
    ruFree(str);

    str = ruStrdup("foo was\there");
    ruStripChars(str, "\n");
    ck_assert_str_eq(str, "foo was\there");
    ruFree(str);

}
END_TEST

START_TEST ( buffer ) {
    int32_t ret, exp;
    const char *test = "ruBufferNew";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";

    char *data;
    char *want = NULL;
    ruBuffer rb = ruBufferNew(3);
    fail_if(rb == NULL, retText, test, NULL, rb);
    rusize explen = 0;
    rusize len = ruBufferLen(rb, &ret);
    fail_unless(explen == len, retText, test, explen, len);

    test = "ruBufferAppend";
    exp = RUE_OK;
    want = "f\x00o";
    explen = 3;
    data = "f\x00o";
    ret = ruBufferAppend(rb, data, 3);
    fail_unless(ret == exp, retText, test, exp, ret);
    len = ruBufferLen(rb, &ret);
    fail_unless(explen == len, retText, test, explen, len);
    data = ruBufferGetData(rb);
    ck_assert_mem_eq(data, want, explen);

    ret = ruBufferAppendUriEncoded(rb, data, 3);
    fail_unless(ret == exp, retText, test, exp, ret);
    explen = 8;
    want = "f\x00of%00o";
    len = ruBufferLen(rb, &ret);
    fail_unless(explen == len, retText, test, explen, len);
    data = ruBufferGetData(rb);
    ck_assert_mem_eq(data, want, explen);

    ruBufferFree(rb, true);
    ruFree(data);

}
END_TEST

TCase* stringTests ( void ) {
    TCase *tcase = tcase_create ( "string" );
    tcase_add_test ( tcase, api );
    tcase_add_test ( tcase, run );
    tcase_add_test ( tcase, util );
    tcase_add_test ( tcase, buffer );
    return tcase;
}
