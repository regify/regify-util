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

static void runApi(ruJson rj, perm_chars test, int32_t exp) {
    int32_t ret;
    const char *retText = "%s failed wanted ret '%d' but got '%d'";

    ret = ruJsonStartMap(rj);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruJsonEndMap(rj);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruJsonStartArray(rj);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruJsonEndArray(rj);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruJsonSetInt(rj, 0);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruJsonSetDouble(rj, 0);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruJsonSetStr(rj, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruJsonSetKeyStr(rj, NULL, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruJsonSetKeyInt(rj, NULL, 0);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruJsonSetKeyDouble(rj, NULL, 0);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruJsonStartKeyMap(rj, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruJsonStartKeyArray(rj, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruJsonWrite(rj, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    perm_chars str;
    perm_chars estr = NULL;
    int64_t i64, e64 = 0;
    double d, ed = 0.0;
    ruJson jsn, ejsn = NULL;

    str = ruJsonStr(rj, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(estr == str, retText, test, estr, str);

    i64 = ruJsonParseInt(rj, &ret);
    fail_if(RUE_OK == ret, retText, test, RUE_OK, ret);
    fail_unless(e64 == i64, retText, test, e64, i64);

    i64 = ruJsonInt(rj, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(e64 == i64, retText, test, e64, i64);

    str = ruJsonKeyStr(rj, NULL, &ret);
    fail_unless(RUE_PARAMETER_NOT_SET == ret, retText, test, RUE_PARAMETER_NOT_SET, ret);
    fail_unless(estr == str, retText, test, estr, str);

    str = ruJsonKeyStrDup(rj, NULL, &ret);
    fail_unless(RUE_PARAMETER_NOT_SET == ret, retText, test, RUE_PARAMETER_NOT_SET, ret);
    fail_unless(estr == str, retText, test, estr, str);

    i64 = ruJsonKeyInt(rj, NULL, &ret);
    fail_unless(RUE_PARAMETER_NOT_SET == ret, retText, test, RUE_PARAMETER_NOT_SET, ret);
    fail_unless(e64 == i64, retText, test, e64, i64);

    i64 = ruJsonKeyParseInt(rj, NULL, &ret);
    fail_unless(RUE_PARAMETER_NOT_SET == ret, retText, test, RUE_PARAMETER_NOT_SET, ret);
    fail_unless(e64 == i64, retText, test, e64, i64);

    d = ruJsonKeyDouble(rj, NULL, &ret);
    fail_unless(RUE_PARAMETER_NOT_SET == ret, retText, test, RUE_PARAMETER_NOT_SET, ret);
    fail_unless(ed == d, retText, test, ed, d);

    jsn = ruJsonKeyMap(rj, NULL, &ret);
    fail_unless(RUE_PARAMETER_NOT_SET == ret, retText, test, RUE_PARAMETER_NOT_SET, ret);
    fail_unless(ejsn == jsn, retText, test, ejsn, jsn);

    jsn = ruJsonKeyArray(rj, NULL, &ret);
    fail_unless(RUE_PARAMETER_NOT_SET == ret, retText, test, RUE_PARAMETER_NOT_SET, ret);
    fail_unless(ejsn == jsn, retText, test, ejsn, jsn);

    rusize sz, esz = 0;
    sz = ruJsonArrayLen(rj, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    str = ruJsonIdxStr(rj, 99, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(estr == str, retText, test, estr, str);

    str = ruJsonIdxStrDup(rj, 99, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(estr == str, retText, test, estr, str);

    i64 = ruJsonIdxParseInt(rj, 0, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(e64 == i64, retText, test, e64, i64);

    i64 = ruJsonIdxInt(rj, 0, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(e64 == i64, retText, test, e64, i64);

    d = ruJsonIdxDouble(rj, 0, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(ed == d, retText, test, ed, d);

    jsn = ruJsonIdxMap(rj, 0, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(ejsn == jsn, retText, test, ejsn, jsn);

    jsn = ruJsonIdxArray(rj, 0, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(ejsn == jsn, retText, test, ejsn, jsn);
}

START_TEST(api) {
    runApi(NULL, "NULL", RUE_PARAMETER_NOT_SET);
    runApi((ruJson)"foo", "foo", RUE_INVALID_PARAMETER);

    int32_t ret, exp = RUE_PARAMETER_NOT_SET;
    perm_chars test = "ruJsonParse";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    ruJson jsn, ejsn = NULL;
    perm_chars estr = NULL;

    jsn = ruJsonParse(NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(ejsn == jsn, retText, test, ejsn, jsn);
}
END_TEST

START_TEST(get) {
    int32_t ret, exp;
    const char *retText = "failed wanted ret '%d' but got '%d'";
    ruJson jsn = NULL;
    perm_chars str;
    perm_chars estr;
    // {"num":23}
    perm_chars jsonStr = "{\"key\":\"2342";
    exp = RUE_INVALID_PARAMETER;

    jsn = ruJsonParse(jsonStr, &ret);
    fail_unless(NULL == jsn, retText, NULL, jsn);
    fail_unless(ret == exp, retText, exp, ret);

    jsonStr = "{\"key\":\"2342\", \"num\": 2342}";
    exp = RUE_OK;
    jsn = ruJsonParse(jsonStr, &ret);
    fail_if(NULL == jsn, retText, NULL, jsn);
    fail_unless(ret == exp, retText, exp, ret);

    estr = "2342";
    str = ruJsonKeyStr(jsn, "key", &ret);
    fail_unless(ret == exp, retText, exp, ret);
    ck_assert_str_eq(str, estr);
    fail_if(NULL == jsn, retText, NULL, jsn);

    int64_t i64, e64 = 2342;
    i64 = ruJsonKeyInt(jsn, "num", &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(e64 == i64, retText, e64, i64);

    jsn = ruJsonFree(jsn);
    fail_unless(NULL == jsn, retText, NULL, jsn);

    jsonStr = "[2342,\"2342\"]";
    jsn = ruJsonParse(jsonStr, &ret);
    fail_if(NULL == jsn, retText, NULL, jsn);
    fail_unless(ret == exp, retText, exp, ret);

    ruJson jo = NULL;
    exp = RUE_WRONG_PARAMETER_LENGTH;
    str = ruJsonIdxStr(jsn, 9, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(NULL == str, retText, NULL, str);

    exp = RUE_OK;
    rusize sz, esz = 2;
    sz = ruJsonArrayLen(jsn, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(esz == sz, retText, esz, sz);

    i64 = ruJsonIdxInt(jsn, 0, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(e64 == i64, retText, e64, i64);

    str = ruJsonIdxStr(jsn, 1, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    ck_assert_str_eq(str, estr);

    jsn = ruJsonFree(jsn);
    fail_unless(NULL == jsn, retText, NULL, jsn);

    jsonStr = "[{\"key\": \"2342\", \"num\": 2342}, [2342, \"2342\"]]";
    jsn = ruJsonParse(jsonStr, &ret);
    fail_if(NULL == jsn, retText, NULL, jsn);
    fail_unless(ret == exp, retText, exp, ret);

    sz = ruJsonArrayLen(jsn, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(esz == sz, retText, esz, sz);

    ruJson jm;
    jm = ruJsonIdxMap(jsn, 0, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_if(NULL == jm, retText, NULL, jm);

    str = ruJsonKeyStr(jm, "key", &ret);
    fail_unless(ret == exp, retText, exp, ret);
    ck_assert_str_eq(str, estr);
    fail_if(NULL == jsn, retText, NULL, jsn);

    i64 = ruJsonKeyInt(jm, "num", &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(e64 == i64, retText, e64, i64);

    ruJson ja;
    ja = ruJsonIdxArray(jsn, 1, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_if(NULL == ja, retText, NULL, ja);

    sz = ruJsonArrayLen(ja, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(esz == sz, retText, esz, sz);

    i64 = ruJsonIdxInt(ja, 0, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(e64 == i64, retText, e64, i64);

    str = ruJsonIdxStr(ja, 1, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    ck_assert_str_eq(str, estr);

    jsn = ruJsonFree(jsn);
    fail_unless(NULL == jsn, retText, NULL, jsn);

    ruList keys = NULL;
    uint32_t ecnt, cnt;

    // {"map": {"key": "2342", "num": 2342, "dbl": 23.42}, "arr": [2342, "2342", 23.42]}
    jsonStr = "{\"map\": {\"key\": \"2342\", \"num\": 2342, \"dbl\": 23.42}, \"arr\": [2342, \"2342\", 23.42]}";
    jsn = ruJsonParse(jsonStr, &ret);
    fail_if(NULL == jsn, retText, NULL, jsn);
    fail_unless(ret == exp, retText, exp, ret);

    keys = ruJsonKeys(jsn, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_if(NULL == keys, retText, NULL, keys);

    ecnt = 2;
    cnt = ruListSize(keys, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(cnt == ecnt, retText, cnt, ecnt);
    keys = ruListFree(keys);

    jm = ruJsonKeyMap(jsn, "map", &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_if(NULL == jm, retText, NULL, jm);

    keys = ruJsonKeys(jm, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_if(NULL == keys, retText, NULL, keys);

    ecnt = 3;
    cnt = ruListSize(keys, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(cnt == ecnt, retText, cnt, ecnt);
    keys = ruListFree(keys);

    str = ruJsonKeyStr(jm, "key", &ret);
    fail_unless(ret == exp, retText, exp, ret);
    ck_assert_str_eq(str, estr);
    fail_if(NULL == jsn, retText, NULL, jsn);

    i64 = ruJsonKeyInt(jm, "num", &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(e64 == i64, retText, e64, i64);

    double db, edb = 23.42;
    db = ruJsonKeyDouble(jm, "dbl", &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(edb == db, retText, edb, db);

    ja = ruJsonKeyArray(jsn, "arr", &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_if(NULL == ja, retText, NULL, ja);

    i64 = ruJsonIdxInt(ja, 0, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(e64 == i64, retText, e64, i64);

    str = ruJsonIdxStr(ja, 1, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    ck_assert_str_eq(str, estr);

    i64 = ruJsonIdxParseInt(ja, 1, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(e64 == i64, retText, e64, i64);

    db = ruJsonIdxDouble(ja, 2, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(edb == db, retText, edb, db);

    jsn = ruJsonFree(jsn);
    fail_unless(NULL == jsn, retText, NULL, jsn);

    // "2342"
    jsonStr = "\"2342\"";
    jsn = ruJsonParse(jsonStr, &ret);
    fail_if(NULL == jsn, retText, NULL, jsn);
    fail_unless(ret == exp, retText, exp, ret);

    str = ruJsonStr(jsn, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    ck_assert_str_eq(str, estr);

    i64 = ruJsonParseInt(jsn, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(e64 == i64, retText, e64, i64);

    jsn = ruJsonFree(jsn);
    fail_unless(NULL == jsn, retText, NULL, jsn);

    // 2342
    jsonStr = "2342";
    jsn = ruJsonParse(jsonStr, &ret);
    fail_if(NULL == jsn, retText, NULL, jsn);
    fail_unless(ret == exp, retText, exp, ret);

    i64 = ruJsonInt(jsn, &ret);
    fail_unless(ret == exp, retText, exp, ret);
    fail_unless(e64 == i64, retText, e64, i64);

    jsn = ruJsonFree(jsn);
    fail_unless(NULL == jsn, retText, NULL, jsn);
}
END_TEST

START_TEST(set) {
    int32_t ret, exp = RUE_OK;
    perm_chars retText = "failed wanted ret '%d' but got '%d'";
    perm_chars key = "key";
    perm_chars val = "2342";
    perm_chars num = "num";
    int64_t n = 2342;
    perm_chars dbl = "dbl";
    double d = 23.0;

    // simple
    ruJson jsn = ruJsonStart(true);
    ret = ruJsonSetKeyStr(jsn, key, val);
    fail_unless(ret == exp, retText, exp, ret);
    fail_if(NULL == jsn, retText, NULL, jsn);

    ret = ruJsonSetKeyInt(jsn, num, n);
    fail_unless(ret == exp, retText, exp, ret);

    ret = ruJsonSetKeyDouble(jsn, dbl, d);
    fail_unless(ret == exp, retText, exp, ret);

    perm_chars estr = "{\"key\":\"2342\",\"num\":2342,\"dbl\":23.0}";
    perm_chars str =  NULL;
    ret = ruJsonWrite(jsn, &str);
    fail_unless(ret == exp, retText, exp, ret);
    ck_assert_str_eq(str, estr);

    jsn = ruJsonFree(jsn);
    fail_unless(NULL == jsn, retText, NULL, jsn);

    // nested array
    jsn = ruJsonNew();
    fail_if(NULL == jsn, retText, NULL, jsn);

    ret = ruJsonStartArray(jsn);
    fail_unless(ret == exp, retText, exp, ret);

    ret = ruJsonSetStr(jsn, val);
    fail_unless(ret == exp, retText, exp, ret);
    fail_if(NULL == jsn, retText, NULL, jsn);

    ret = ruJsonSetInt(jsn, n);
    fail_unless(ret == exp, retText, exp, ret);

    ret = ruJsonSetDouble(jsn, d);
    fail_unless(ret == exp, retText, exp, ret);

    ret = ruJsonStartMap(jsn);
    fail_unless(ret == exp, retText, exp, ret);

    ret = ruJsonSetKeyStr(jsn, key, val);
    fail_unless(ret == exp, retText, exp, ret);
    fail_if(NULL == jsn, retText, NULL, jsn);

    ret = ruJsonSetKeyInt(jsn, num, n);
    fail_unless(ret == exp, retText, exp, ret);

    ret = ruJsonEndMap(jsn);
    fail_unless(ret == exp, retText, exp, ret);

    ret = ruJsonEndArray(jsn);
    fail_unless(ret == exp, retText, exp, ret);

    estr = "[\"2342\",2342,23.0,{\"key\":\"2342\",\"num\":2342}]";
    str = NULL;
    ret = ruJsonWrite(jsn, &str);
    fail_unless(ret == exp, retText, exp, ret);
    ck_assert_str_eq(str, estr);

    jsn = ruJsonFree(jsn);
    fail_unless(NULL == jsn, retText, NULL, jsn);

    // nested map
    perm_chars map = "map";
    perm_chars arr = "arr";
    jsn = ruJsonStart(true);
    fail_if(NULL == jsn, retText, NULL, jsn);

    ret = ruJsonStartKeyMap(jsn, map);
    fail_unless(ret == exp, retText, exp, ret);

    ret = ruJsonSetKeyStr(jsn, key, val);
    fail_unless(ret == exp, retText, exp, ret);
    fail_if(NULL == jsn, retText, NULL, jsn);

    ret = ruJsonSetKeyInt(jsn, num, n);
    fail_unless(ret == exp, retText, exp, ret);

    ret = ruJsonEndMap(jsn);
    fail_unless(ret == exp, retText, exp, ret);

    ret = ruJsonStartKeyArray(jsn, arr);
    fail_unless(ret == exp, retText, exp, ret);

    ret = ruJsonSetStr(jsn, val);
    fail_unless(ret == exp, retText, exp, ret);
    fail_if(NULL == jsn, retText, NULL, jsn);

    ret = ruJsonSetInt(jsn, n);
    fail_unless(ret == exp, retText, exp, ret);

    ret = ruJsonEndArray(jsn);
    fail_unless(ret == exp, retText, exp, ret);

    estr = "{\"map\":{\"key\":\"2342\",\"num\":2342},\"arr\":[\"2342\",2342]}";
    ret = ruJsonWrite(jsn, &str);
    fail_unless(ret == exp, retText, exp, ret);
    ck_assert_str_eq(str, estr);

    jsn = ruJsonFree(jsn);
    fail_unless(NULL == jsn, retText, NULL, jsn);
}
END_TEST

void sample() {
    ruJson jsn = NULL;
    int32_t ret;
    do{

        perm_chars jsonStr = "{"
                 "\"map\": {"
                    "\"key\": \"2342\", "
                    "\"num\": 2342, "
                    "\"dbl\": 23.42"
                    "}, "
                 "\"arr\": ["
                    "2342, \"2342\", 23.42"
                    "]"
                 "}";
        jsn = ruJsonParse(jsonStr, &ret);
        if (ret != RUE_OK) break;

        // the map
        ruJson jm = ruJsonKeyMap(jsn, "map", &ret);
        if (ret != RUE_OK) break;

        perm_chars str = ruJsonKeyStr(jm, "key", &ret);
        if (ret != RUE_OK) break;
        printf("str: '%s'", str);

        int64_t i64 = ruJsonKeyInt(jm, "num", &ret);
        if (ret != RUE_OK) break;
        printf("int: %ld", i64);

        double db = ruJsonKeyDouble(jm, "dbl", &ret);
        if (ret != RUE_OK) break;
        printf("float: %f", db);

        // the array
        ruJson ja = ruJsonKeyArray(jsn, "arr", &ret);
        if (ret != RUE_OK) break;

        i64 = ruJsonIdxInt(ja, 0, &ret);
        if (ret != RUE_OK) break;
        printf("int: %ld", i64);

        str = ruJsonIdxStr(ja, 1, &ret);
        if (ret != RUE_OK) break;
        printf("str: '%s'", str);

        i64 = ruJsonIdxParseInt(ja, 1, &ret);
        if (ret != RUE_OK) break;
        printf("int: %ld", i64);

        db = ruJsonIdxDouble(ja, 2, &ret);
        if (ret != RUE_OK) break;
        printf("float: %f", db);

    } while(0);
    ruJsonFree(jsn);
}

TCase* jsonTests(void) {
    TCase *tcase = tcase_create("json");
    tcase_add_test(tcase, api);
    tcase_add_test(tcase, get);
    tcase_add_test(tcase, set);
    return tcase;
}
