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

const char *failText = "failed wanted ret '%d' but got '%d'";

START_TEST ( api ) {
    int32_t ret, exp;
    perm_chars test = "ruStringNewf";
    perm_chars retText = "%s failed wanted ret '%d' but got '%d'";

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

    trans_chars foo = ruStringGetCString(rs);
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

    int64_t num = ruStrParseInt64(NULL, NULL, 0);
    fail_unless(0 == num, retText, test, 0, num);
    fail_unless(EINVAL == errno, retText, test, EINVAL, errno);

    num = ruStrParseInt64("", NULL, 0);
    fail_unless(0 == num, retText, test, 0, num);

    ruStrTrim(NULL);
    ruStrTrim("");

    ruStripChars(NULL, NULL);
    ruStripChars("", NULL);
    ruStripChars("", "");
    ruStripChars(" ", "");
    ruStripChars("", " ");

    ruStrByteReplace(NULL, 0, 0);
    ruStrByteReplace(0, 0, 0);

    long lres = ruStrParseLong(NULL);
    fail_unless(0 == num, retText, test, 0, num);
    fail_unless(EINVAL == errno, retText, test, EINVAL, errno);

    lres = ruStrToLong(NULL);
    fail_unless(0 == num, retText, test, 0, num);
    fail_unless(EINVAL == errno, retText, test, EINVAL, errno);

    lres = ruStrToInt64(NULL);
    fail_unless(0 == num, retText, test, 0, num);
    fail_unless(EINVAL == errno, retText, test, EINVAL, errno);
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
    char* subst = "test^^^^foo";
    trans_chars ptr = ruLastSubstr(subst, "^^");
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

    char* heap = ruStrDup(subst);
    ruStrByteReplace(heap, '^', 'v');
    ck_assert_str_eq("testvfoo", heap);

    ruStrByteReplace(heap, 'v', '\0');
    ck_assert_str_eq("test", heap);
    ruFree(heap);

    test = "ruStrParseInt64";
    int64_t expNum = 666;
    int64_t num = ruStrParseInt64(" 0666NULL", NULL, 10);
    fail_unless(expNum == num, retText, test, expNum, num);

    expNum = -666;
    num = ruStrParseInt64("-66,6.5", NULL, 10);
    fail_unless(expNum == num, retText, test, expNum, num);

    test = "ruStrParseLong";
    long lexp = 0;
    long lnum = ruStrParseLong(" 0666NULL");
    fail_unless(lexp == lnum, retText, test, lexp, lnum);

    test = "ruStrParseLong";
    lexp = 666;
    lnum = ruStrToLong(" 0666NULL");
    fail_unless(lexp == lnum, retText, test, lexp, lnum);

    test = "ruStrToInt64";
    expNum = 666;
    num = ruStrToInt64(" 0666NULL");
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
    trans_chars alstr = ptr;
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


    test = "ruStrCmp";
    want = 0;
    got = ruStrCmp(NULL, NULL);
    fail_unless(got == want, retText, test, want, got);

    want = 1;
    got = ruStrCmp("foo", NULL);
    fail_unless(got == want, retText, test, want, got);

    want = -1;
    got = ruStrCmp(NULL, "foo");
    fail_unless(got == want, retText, test, want, got);

    want = 0;
    got = ruStrCmp("foo", "foo");
    fail_unless(got == want, retText, test, want, got);

    want = -1;
    got = ruStrCmp("bar", "foo");
    fail_unless(got == want, retText, test, want, got);

    test = "ruStrEquals";
    bool is, be = true;
    is = ruStrEquals(NULL, NULL);
    fail_unless(be == is, retText, test, be, is);

    is = ruStrEquals("foo", "foo");
    fail_unless(be == is, retText, test, be, is);

    be = false;
    is = ruStrEquals("foo", NULL);
    fail_unless(be == is, retText, test, be, is);

    is = ruStrEquals(NULL, "foo");
    fail_unless(be == is, retText, test, be, is);

    test = "ruStrNEquals";
    be = true;
    is = ruStrNEquals(NULL, 10, NULL);
    fail_unless(be == is, retText, test, be, is);

    is = ruStrNEquals("foo", 0, NULL);
    fail_unless(be == is, retText, test, be, is);

    is = ruStrNEquals(NULL, 0, "foo");
    fail_unless(be == is, retText, test, be, is);

    is = ruStrNEquals("foobar", 0, "foo");
    fail_unless(be == is, retText, test, be, is);

    is = ruStrNEquals("foobar", 3, "foo");
    fail_unless(be == is, retText, test, be, is);

    is = ruStrNEquals("foobar", 4, "foo");
    fail_unless(be == is, retText, test, be, is);

    be = false;
    is = ruStrNEquals("foo", 3, "foobar");
    fail_unless(be == is, retText, test, be, is);

    is = ruStrNEquals("foo", 4, "foobar");
    fail_unless(be == is, retText, test, be, is);

    test = "ruStrCaseCmp";
    want = 0;
    got = ruStrCaseCmp(NULL, NULL);
    fail_unless(got == want, retText, test, want, got);

    want = 1;
    got = ruStrCaseCmp("foo", NULL);
    fail_unless(got == want, retText, test, want, got);

    want = -1;
    got = ruStrCaseCmp(NULL, "foo");
    fail_unless(got == want, retText, test, want, got);

    want = 0;
    got = ruStrCaseCmp("foo", "Foo");
    fail_unless(got == want, retText, test, want, got);

    want = -1;
    got = ruStrCaseCmp("bar", "foo");
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
    perm_chars str = ruStrStr(NULL, NULL);
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

    alloc_chars out2 = ruUtf8ToUpper(exStr);
    ck_assert_str_eq("!FÖO¡", out2);

    ruFree(exStr);
    ruFree(out2);
    out2 = ruStrDup("foo was\nhere");
    ruStripChars(out2, "\n ");
    ck_assert_str_eq(out2, "foowashere");
    ruFree(out2);

    out2 = ruStrDup("foo was\there");
    ruStripChars(out2, "\n");
    ck_assert_str_eq(out2, "foo was\there");
    ruFree(out2);

    out2 = ruStrDup("  foo\t \n\r");
    ruStrTrim(out2);
    ck_assert_str_eq(out2, "  foo");
    ruFree(out2);

    out2 = ruStrTrimDup(NULL);
    fail_unless(NULL == out2, retText, test, NULL, out2);

    out2 = ruStrTrimDup("foo");
    fail_unless(NULL == out2, retText, test, NULL, out2);

    out2 = ruStrTrimDup("  foo\t \n\r");
    ck_assert_str_eq(out2, "foo");
    ruFree(out2);

    out2 = ruStrTrimDup("foo\t \n\r");
    ck_assert_str_eq(out2, "foo");
    ruFree(out2);

    out2 = ruStrTrimDup("  foo");
    ck_assert_str_eq(out2, "foo");
    ruFree(out2);

    test = "ruStrEmpty";
    doexp = true;
    does = ruStrEmpty(NULL);
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruStrEmpty("");
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruStrEmpty(" \n\t ");
    fail_unless(doexp == does, retText, test, doexp, does);

    doexp = false;
    does = ruStrEmpty(" 1");
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruStrEmpty("1 ");
    fail_unless(doexp == does, retText, test, doexp, does);

    does = ruStrEmpty(" 1\n\t ");
    fail_unless(doexp == does, retText, test, doexp, does);
}
END_TEST

START_TEST (StrNSplit) {
    ruList rl = ruStrSplit(NULL, NULL, 0);
    fail_unless(NULL == rl, failText, NULL, rl);

    perm_chars istr = "foo:bar";
    perm_chars delim = ":";
    rusize_s limit = -1;
    rl = ruStrNSplit(NULL, limit, NULL, 0);
    fail_unless(NULL == rl, failText, NULL, rl);

    rl = ruStrNSplit(istr, limit, NULL, 0);
    fail_unless(NULL == rl, failText, NULL, rl);

    rl = ruStrNSplit(istr, limit, "", 0);
    fail_unless(NULL == rl, failText, NULL, rl);

    limit = 0;
    rl = ruStrNSplit(istr, limit, delim, 1);
    fail_unless(NULL == rl, failText, NULL, rl);

    int32_t want = 1;
    rl = ruStrSplit(istr, delim, 1);
    fail_if(NULL == rl, failText, NULL, rl);
    int32_t got = ruListSize(rl, NULL);
    fail_unless(want == got, failText, want, got);
    rl = ruListFree(rl);

    want = 2;
    rl = ruStrSplit(istr, delim, 0);
    fail_if(NULL == rl, failText, NULL, rl);
    got = ruListSize(rl, NULL);
    fail_unless(want == got, failText, want, got);
    rl = ruListFree(rl);

    limit = 6;
    rl = ruStrNSplit(istr, limit, delim, 0);
    fail_if(NULL == rl, failText, NULL, rl);
    got = ruListSize(rl, NULL);
    fail_unless(want == got, failText, want, got);
    alloc_chars pcs = ruListPop(rl, NULL);
    ck_assert_str_eq(pcs, "foo");
    ruFree(pcs);
    pcs = ruListPop(rl, NULL);
    ck_assert_str_eq(pcs, "ba");
    ruFree(pcs);
    rl = ruListFree(rl);

    istr = ":foo:bar:";
    want = 4;
    rl = ruStrSplit(istr, delim, 0);
    fail_if(NULL == rl, failText, NULL, rl);
    got = ruListSize(rl, NULL);
    fail_unless(want == got, failText, want, got);
    rl = ruListFree(rl);

    limit = 7;
    want = 3;
    rl = ruStrNSplit(istr, limit, delim, 0);
    fail_if(NULL == rl, failText, NULL, rl);
    got = ruListSize(rl, NULL);
    fail_unless(want == got, failText, want, got);
    pcs = ruListPop(rl, NULL);
    ck_assert_str_eq(pcs, "");
    ruFree(pcs);
    pcs = ruListPop(rl, NULL);
    ck_assert_str_eq(pcs, "foo");
    ruFree(pcs);
    pcs = ruListPop(rl, NULL);
    ck_assert_str_eq(pcs, "ba");
    ruFree(pcs);
    rl = ruListFree(rl);
}
END_TEST

START_TEST (StrTrimBounds) {
    perm_chars str = ruStrTrimBounds(NULL, 0, NULL);
    fail_unless(NULL == str, failText, NULL, str);

    rusize len = 99;
    str = ruStrTrimBounds(NULL, -1, &len);
    fail_unless(NULL == str, failText, NULL, str);
    fail_unless(0 == len, failText, 0, len);

    perm_chars instr = " foo ";
    perm_chars expstr = instr+1;
    str = ruStrTrimBounds(instr, -1, NULL);
    ck_assert_str_eq(str, "foo ");

    str = ruStrTrimBounds(instr, 3, &len);
    fail_unless(expstr == str, failText, expstr, str);
    fail_unless(2 == len, failText, 2, len);

    str = ruStrTrimBounds(instr, -1, &len);
    fail_unless(expstr == str, failText, expstr, str);
    ck_assert_str_eq(str, "foo ");
    fail_unless(3 == len, failText, 3, len);

    instr = "  ";
    expstr = NULL;
    str = ruStrTrimBounds(instr, -1, &len);
    fail_unless(expstr == str, failText, expstr, str);
    fail_unless(0 == len, failText, 0, len);

    instr = "";
    expstr = NULL;
    str = ruStrTrimBounds(instr, -1, &len);
    fail_unless(expstr == str, failText, expstr, str);
    fail_unless(0 == len, failText, 0, len);

}
END_TEST

START_TEST (StrFindKeyVal) {
    bool is, be = false;
    is = ruStrFindKeyVal(NULL, 0, NULL,
                         NULL, 0,
                         NULL, 0);
    fail_unless(be == is, failText, be, is);

    perm_chars instr = " foo ";
    is = ruStrFindKeyVal(instr, -1, NULL,
                         NULL, 0,
                         NULL, 0);
    fail_unless(be == is, failText, be, is);

    trans_chars keyStart;
    rusize keyLen;
    rusize ekLen = 3;
    trans_chars valStart;
    rusize valLen;
    rusize evLen = 0;
    trans_chars delim = ":";
    perm_chars ekey = instr + 1;
    perm_chars eval = NULL;
    is = ruStrFindKeyVal(instr, -1, delim,
                         &keyStart, &keyLen,
                         &valStart, &valLen);
    fail_unless(be == is, failText, be, is);
    fail_unless(ekey == keyStart, failText, ekey, keyStart);
    fail_unless(ekLen == keyLen, failText, ekLen, keyLen);
    fail_unless(eval == valStart, failText, eval, valStart);
    fail_unless(evLen == valLen, failText, evLen, valLen);

    instr = "foo : bar ";
    is = ruStrFindKeyVal(instr, 4, delim,
                         NULL, 0,
                         NULL, 0);
    fail_unless(be == is, failText, be, is);

    be = true;
    is = ruStrFindKeyVal(instr, -1, delim,
                         NULL, 0,
                         NULL, 0);
    fail_unless(be == is, failText, be, is);

    ekey = instr;
    eval = instr + 6;
    ekLen = 3;
    evLen = 3;
    is = ruStrFindKeyVal(instr, -1, delim,
                         &keyStart, &keyLen,
                         &valStart, &valLen);
    fail_unless(be == is, failText, be, is);
    fail_unless(ekey == keyStart, failText, ekey, keyStart);
    fail_unless(ekLen == keyLen, failText, ekLen, keyLen);
    fail_unless(eval == valStart, failText, eval, valStart);
    fail_unless(evLen == valLen, failText, evLen, valLen);

    instr = " foo : bar ";
    ekey = instr + 1;
    be = false;
    ekLen = 9;
    evLen = 0;
    eval = NULL;
    is = ruStrFindKeyVal(instr, -1, NULL,
                         &keyStart, &keyLen,
                         &valStart, &valLen);
    fail_unless(be == is, failText, be, is);
    fail_unless(ekey == keyStart, failText, ekey, keyStart);
    fail_unless(ekLen == keyLen, failText, ekLen, keyLen);
    fail_unless(eval == valStart, failText, eval, valStart);
    fail_unless(evLen == valLen, failText, evLen, valLen);

    instr = " : bar ";
    ekey = NULL;
    eval = instr + 3;
    ekLen = 0;
    evLen = 3;
    be = true;
    is = ruStrFindKeyVal(instr, -1, delim,
                         &keyStart, &keyLen,
                         &valStart, &valLen);
    fail_unless(be == is, failText, be, is);
    fail_unless(ekey == keyStart, failText, ekey, keyStart);
    fail_unless(ekLen == keyLen, failText, ekLen, keyLen);
    fail_unless(eval == valStart, failText, eval, valStart);
    fail_unless(evLen == valLen, failText, evLen, valLen);

    instr = " foo : ";
    ekey = instr + 1;
    eval = NULL;
    ekLen = 3;
    evLen = 0;
    is = ruStrFindKeyVal(instr, -1, delim,
                         &keyStart, &keyLen,
                         &valStart, &valLen);
    fail_unless(be == is, failText, be, is);
    fail_unless(ekey == keyStart, failText, ekey, keyStart);
    fail_unless(ekLen == keyLen, failText, ekLen, keyLen);
    fail_unless(eval == valStart, failText, eval, valStart);
    fail_unless(evLen == valLen, failText, evLen, valLen);

    instr = " foo   bahr ";
    ekey = NULL;
    eval = instr + 1;
    delim = " ";
    ekLen = 0;
    evLen = 10;
    is = ruStrFindKeyVal(instr, -1, delim,
                         &keyStart, &keyLen,
                         &valStart, &valLen);
    fail_unless(be == is, failText, be, is);
    fail_unless(ekey == keyStart, failText, ekey, keyStart);
    fail_unless(ekLen == keyLen, failText, ekLen, keyLen);
    fail_unless(eval == valStart, failText, eval, valStart);
    fail_unless(evLen == valLen, failText, evLen, valLen);

    instr = "foo   bahr ";
    ekey = instr;
    eval = instr + 6;
    delim = " ";
    ekLen = 3;
    evLen = 4;
    is = ruStrFindKeyVal(instr, -1, delim,
                         &keyStart, &keyLen,
                         &valStart, &valLen);
    fail_unless(be == is, failText, be, is);
    fail_unless(ekey == keyStart, failText, ekey, keyStart);
    fail_unless(ekLen == keyLen, failText, ekLen, keyLen);
    fail_unless(eval == valStart, failText, eval, valStart);
    fail_unless(evLen == valLen, failText, evLen, valLen);
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
    tcase_add_test(tcase, api);
    tcase_add_test(tcase, run);
    tcase_add_test(tcase, util);
    tcase_add_test(tcase, StrNSplit);
    tcase_add_test(tcase, StrTrimBounds);
    tcase_add_test(tcase, StrFindKeyVal);
    tcase_add_test(tcase, buffer);
    return tcase;
}
