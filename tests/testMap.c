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
    const char *test = "ruMapNewType";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    ruList keys = NULL;

    ruMap rm = NULL;
    fail_unless(NULL == rm, retText, test, NULL, rm);

    rm = ruMapNewType(NULL, NULL);
    fail_unless(NULL == rm, retText, test, NULL, rm);

    ruMapFree(rm);

    ptr exist = NULL;
    exp = RUE_PARAMETER_NOT_SET;
    ret = ruMapTryPut(rm, NULL, NULL, &exist);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(NULL == exist, retText, test, NULL, exist);

    ret = ruMapFirstSet(rm, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruMapNext(NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruMapRemoveAll(rm);
    fail_unless(exp == ret, retText, test, exp, ret);

    rm = ruMapNewType(ruTypeStrRef(), ruTypeStrRef());
    fail_if(NULL == rm, retText, test, rm, NULL);

    ret = ruMapPut(rm, NULL, "foo");
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruMapRemove(NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruMapRemove(rm, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    char *store = "23";
    ret = ruMapRemove(rm, NULL, (void**)&store);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruMapKeyList(rm, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruMapKeyList(NULL, &keys);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_INVALID_STATE;
    ret = ruMapNext(rm, NULL, &store);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_PARAMETER_NOT_SET;
    ret = ruMapNext(rm, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruMapFirstSet(rm, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

//    exp = RUE_PARAMETER_NOT_SET;
    ret = ruMapNext(rm, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_INVALID_PARAMETER;
    ret = ruMapGet((void*)store, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ruMapHas((void*)store, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_PARAMETER_NOT_SET;
    ret = ruMapGet(NULL, "bar", NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruMapGet(rm, NULL, (void**)&store);
    fail_unless(exp == ret, retText, test, exp, ret);

    uint32_t sz = ruMapSize(NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(0 == sz, retText, test, 0, sz);

    exp = RUE_OK;
    sz = ruMapSize(rm, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(0 == sz, retText, test, 0, sz);

    ruMapFree(rm);
}
END_TEST

START_TEST ( run ) {
    int32_t ret, exp;
    perm_chars test = "ruMapNewType";
    perm_chars retText = "%s failed wanted ret '%d' but got '%d'";
    perm_chars foo = "foo";
    perm_chars bar = "bar";

    ruMap rm = ruMapNewType(ruTypeStrRef(), ruTypeStrRef());
    fail_if(NULL == rm, retText, test, rm, NULL);

    exp = RUE_OK;
    uint32_t esz = 0, sz = ruMapSize(rm, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    ret = ruMapPut(rm, foo, bar);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_FILE_EXISTS;
    perm_chars exist = NULL;
    ret = ruMapTryPut(rm, foo,bar, &exist);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(bar == exist, retText, test, bar, exist);

    exp = RUE_OK;
    bool ehas = true;
    bool has = ruMapHas(rm, foo, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(ehas == has, retText, test, ehas, has);

    ehas = false;
    has = ruMapHas(rm, "Foo", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(ehas == has, retText, test, ehas, has);

    char *ekey, *eval;
    ret = ruMapFirst(rm, &ekey, &eval);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq(foo, ekey);
    ck_assert_str_eq("bar", eval);

    exp = RUE_FILE_NOT_FOUND;
    ret = ruMapNext(rm, &ekey, &eval);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(NULL == ekey, retText, test, NULL, ekey);
    fail_unless(NULL == eval, retText, test, NULL, eval);

    ret = ruMapNext(rm, &ekey, &eval);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(NULL == ekey, retText, test, NULL, ekey);
    fail_unless(NULL == eval, retText, test, NULL, eval);

    exp = RUE_OK;
    esz = 1;
    sz = ruMapSize(rm, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    char *store = NULL;
    exp = RUE_GENERAL;
    ret = ruMapGet(rm, "fo", (void**)&store);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_PARAMETER_NOT_SET;
    ret = ruMapGet(rm, foo, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    ret = ruMapGet(rm, foo, (void**)&store);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq("bar", store);

    // update it
    ret = ruMapPut(rm, foo, "not");
    fail_unless(exp == ret, retText, test, exp, ret);

    // should still be 1
    sz = ruMapSize(rm, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    ret = ruMapGet(rm, foo, (void**)&store);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq("not", store);

    // test a copied keyset
    ruList keys = NULL;
    ret = ruMapKeyList(rm, &keys);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_if(NULL == keys, retText, test, NULL, keys);

    sz = ruListSize(keys, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);
    store = ruListPop(keys, &ret);
    ck_assert_str_eq(foo, store);
    keys = ruListFree(keys);

    // test a scalar keyset
    ret = ruMapKeyList(rm, &keys);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_if(NULL == keys, retText, test, NULL, keys);

    sz = ruListSize(keys, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);
    store = ruListPop(keys, &ret);
    ck_assert_str_eq(foo, store);
    fail_unless(foo == store, retText, test, foo, store);
    keys = ruListFree(keys);

    exp = RUE_GENERAL;
    ret = ruMapRemove(rm, "fo", (void**)&store);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    sz = ruMapSize(rm, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    ret = ruMapRemove(rm, foo, (void**)&store);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq("not", store);

    esz = 0;
    sz = ruMapSize(rm, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    ret = ruMapPut(rm, "foo1", "bar1");
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruMapPut(rm, "foo2", "bar2");
    fail_unless(exp == ret, retText, test, exp, ret);

    esz = 2;
    sz = ruMapSize(rm, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    ret = ruMapRemoveAll(rm);
    fail_unless(exp == ret, retText, test, exp, ret);

    esz = 0;
    sz = ruMapSize(rm, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    ruMapFree(rm);
}
END_TEST

START_TEST(dups) {
    int32_t ret, exp = RUE_OK;
    const char *retText = "failed wanted '%x' but got '%x'";
    perm_chars v1 = "42";
    perm_chars v2 = "23";
    perm_chars k1 = "23";
    perm_chars k2 = "42";
    perm_chars r;
    perm_chars e;

    ruMap rm = ruMapNewType(ruTypeStrDup(),
                            ruTypeStrDup());
    fail_if(NULL == rm, retText, rm, NULL);

    ret = ruMapPut(rm, k1, v1);
    fail_unless(exp == ret, retText, exp, ret);

    ret = ruMapPut(rm, k2, v2);
    fail_unless(exp == ret, retText, exp, ret);

    r = NULL;
    e = v1;
    ret = ruMapGet(rm, k1, &r);
    fail_unless(exp == ret, retText, exp, ret);
    ck_assert_str_eq(e, r);

    // checking for leaks
    ret = ruMapRemove(rm, k1, &r);
    fail_unless(exp == ret, retText, exp, ret);
    ck_assert_str_eq(v1, r);
    ruFree(r);
    ruMapFree(rm);
}
END_TEST

START_TEST(perms) {
    int32_t ret, exp = RUE_OK;
    const char *retText = "failed wanted '%x' but got '%x'";
    perm_chars v1 = "42";
    perm_chars k1 = "23";
    perm_chars r;
    perm_chars e;

    ruMap rm = ruMapNewType(ruTypeStrRef(),
                            ruTypeStrRef());
    fail_if(NULL == rm, retText, rm, NULL);

    ret = ruMapPut(rm, k1, v1);
    fail_unless(exp == ret, retText, exp, ret);

    r = NULL;
    e = v1;
    ret = ruMapGet(rm, k1, &r);
    fail_unless(exp == ret, retText, exp, ret);
    ck_assert_str_eq(e, r);

    ruMapFree(rm);
}
END_TEST

START_TEST(bools) {
    int32_t ret, exp = RUE_OK;
    const char *retText = "failed wanted '%x' but got '%x'";
    long k1 = 23, k2 = 42;
    bool r, e;
    bool v1 = false;
    bool v2 = true;

    ruMap rm = ruMapNewType(ruTypeLong(),
                            ruTypeBool());
    fail_if(NULL == rm, retText, rm, NULL);

    ret = ruMapPut(rm, &k1, &v1);
    fail_unless(exp == ret, retText, exp, ret);

    ret = ruMapPut(rm, &k2, &v2);
    fail_unless(exp == ret, retText, exp, ret);

    r = false;
    e = v1;
    ret = ruMapGet(rm, &k1, &r);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(e == r, retText, e, r);

    r = false;
    e = v2;
    ret = ruMapGet(rm, &k2, &r);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(e == r, retText, e, r);

    int i = 0;
    long keys[] = {k1, k2};
    bool vals[] = {v1, v2};
    long key;
    bool val;
    for (ret = ruMapFirst(rm, &key, &val); ret == RUE_OK;
         ret = ruMapNext(rm, &key, &val)) {
        fail_unless(keys[i] == key, retText, keys[i], key);
        fail_unless(vals[i] == val, retText, vals[i], val);
        i++;
    }
    // checking for leaks
    ret = ruMapRemove(rm, &k1, &val);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(v1 == val, retText, v1, val);
    ruMapFree(rm);
}
END_TEST

START_TEST(int8s) {
    int32_t ret, exp = RUE_OK;
    const char *retText = "failed wanted '%x' but got '%x'";
    int8_t v1 = 42;
    int8_t k1 = 23;
    int8_t r, e;

    ruMap rm = ruMapNewType(ruTypeInt8(),
                            ruTypeInt8());
    fail_if(NULL == rm, retText, rm, NULL);

    ret = ruMapPut(rm, &k1, &v1);
    fail_unless(exp == ret, retText, exp, ret);

    r = 0;
    e = v1;
    ret = ruMapGet(rm, &k1, &r);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(e == r, retText, e, r);

    int i = 0;
    int8_t keys[] = {k1};
    int8_t vals[] = {v1};
    int8_t key;
    int8_t val;
    for (ret = ruMapFirst(rm, &key, &val); ret == RUE_OK;
         ret = ruMapNext(rm, &key, &val)) {
        fail_unless(keys[i] == key, retText, keys[i], key);
        fail_unless(vals[i] == val, retText, vals[i], val);
        i++;
    }
    // checking for leaks
    ret = ruMapRemove(rm, &k1, &val);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(v1 == val, retText, v1, val);
    ruMapFree(rm);
}
END_TEST

START_TEST(int16s) {
    int32_t ret, exp = RUE_OK;
    const char *retText = "failed wanted '%x' but got '%x'";
    int16_t v1 = 42;
    int16_t k1 = 23;
    int16_t r, e;

    ruMap rm = ruMapNewType(ruTypeInt16(),
                            ruTypeInt16());
    fail_if(NULL == rm, retText, rm, NULL);

    ret = ruMapPut(rm, &k1, &v1);
    fail_unless(exp == ret, retText, exp, ret);

    r = 0;
    e = v1;
    ret = ruMapGet(rm, &k1, &r);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(e == r, retText, e, r);

    int i = 0;
    int16_t keys[] = {k1};
    int16_t vals[] = {v1};
    int16_t key;
    int16_t val;
    for (ret = ruMapFirst(rm, &key, &val); ret == RUE_OK;
         ret = ruMapNext(rm, &key, &val)) {
        fail_unless(keys[i] == key, retText, keys[i], key);
        fail_unless(vals[i] == val, retText, vals[i], val);
        i++;
    }
    // checking for leaks
    ret = ruMapRemove(rm, &k1, &val);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(v1 == val, retText, v1, val);
    ruMapFree(rm);
}
END_TEST

START_TEST(int32s) {
    int32_t ret, exp = RUE_OK;
    const char *retText = "failed wanted '%x' but got '%x'";
    int32_t v1 = 42;
    int32_t k1 = 23;
    int32_t r, e;

    ruMap rm = ruMapNewType(ruTypeInt32(),
                            ruTypeInt32());
    fail_if(NULL == rm, retText, rm, NULL);

    ret = ruMapPut(rm, &k1, &v1);
    fail_unless(exp == ret, retText, exp, ret);

    r = 0;
    e = v1;
    ret = ruMapGet(rm, &k1, &r);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(e == r, retText, e, r);

    int i = 0;
    int32_t keys[] = {k1};
    int32_t vals[] = {v1};
    int32_t key;
    int32_t val;
    for (ret = ruMapFirst(rm, &key, &val); ret == RUE_OK;
         ret = ruMapNext(rm, &key, &val)) {
        fail_unless(keys[i] == key, retText, keys[i], key);
        fail_unless(vals[i] == val, retText, vals[i], val);
        i++;
    }
    // checking for leaks
    ret = ruMapRemove(rm, &k1, &val);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(v1 == val, retText, v1, val);
    ruMapFree(rm);
}
END_TEST

START_TEST(longs) {
    int32_t ret, exp = RUE_OK;
    const char *retText = "failed wanted '%x' but got '%x'";
    long v1 = 42;
    long k1 = 23;
    long r, e;

    ruMap rm = ruMapNewType(ruTypeLong(),
                            ruTypeLong());
    fail_if(NULL == rm, retText, rm, NULL);

    ret = ruMapPut(rm, &k1, &v1);
    fail_unless(exp == ret, retText, exp, ret);

    r = 0;
    e = v1;
    ret = ruMapGet(rm, &k1, &r);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(e == r, retText, e, r);

    int i = 0;
    long keys[] = {k1};
    long vals[] = {v1};
    long key;
    long val;
    for (ret = ruMapFirst(rm, &key, &val); ret == RUE_OK;
         ret = ruMapNext(rm, &key, &val)) {
        fail_unless(keys[i] == key, retText, keys[i], key);
        fail_unless(vals[i] == val, retText, vals[i], val);
        i++;
    }
    // checking for leaks
    ret = ruMapRemove(rm, &k1, &val);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(v1 == val, retText, v1, val);
    ruMapFree(rm);
}
END_TEST

START_TEST(int64s) {
    int32_t ret, exp = RUE_OK;
    const char *retText = "failed wanted '%x' but got '%x'";
    int64_t v1 = 42, v2 = 23;
    int64_t k1 = 23, k2 = 42;
    int64_t r, e;

    ruMap rm = ruMapNewType(ruTypeInt64(),
                            ruTypeInt64());
    fail_if(NULL == rm, retText, rm, NULL);

    ret = ruMapPut(rm, &k1, &v1);
    fail_unless(exp == ret, retText, exp, ret);

    ret = ruMapPut(rm, &k2, &v2);
    fail_unless(exp == ret, retText, exp, ret);

    r = 0;
    e = v1;
    ret = ruMapGet(rm, &k1, &r);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(e == r, retText, e, r);

    r = 0;
    e = v2;
    ret = ruMapGet(rm, &k2, &r);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(e == r, retText, e, r);

    int i = 0;
    int64_t keys[] = {k1, k2};
    int64_t vals[] = {v1, v2};
    int64_t key;
    int64_t val;
    for (ret = ruMapFirst(rm, &key, &val); ret == RUE_OK;
         ret = ruMapNext(rm, &key, &val)) {
        fail_unless(keys[i] == key, retText, keys[i], key);
        fail_unless(vals[i] == val, retText, vals[i], val);
        i++;
    }
    // checking for leaks
    ret = ruMapRemove(rm, &k1, &val);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(v1 == val, retText, v1, val);
    ruMapFree(rm);
}
END_TEST

START_TEST(custom) {
    int32_t ret, exp = RUE_OK;
    const char *retText = "failed wanted '%x' but got '%x'";
    int64_t v1 = 42;
    int64_t v2 = 23;
    perm_chars k1 = "23";
    perm_chars k2 = "42";
    int64_t r;
    int64_t e;

    ruType ks = ruTypeNew(ruStrHash, ruStrMatch, NULL,
                          NULL, NULL, NULL);
    ruMap rm = ruMapNewType(ks, ruTypeInt64());
    fail_if(NULL == rm, retText, rm, NULL);

    ret = ruMapPut(rm, k1, &v1);
    fail_unless(exp == ret, retText, exp, ret);

    ret = ruMapPut(rm, k2, &v2);
    fail_unless(exp == ret, retText, exp, ret);

    r = 0;
    e = v1;
    ret = ruMapGet(rm, k1, &r);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(e == r, retText, e, r);

    r = 0;
    e = v2;
    ret = ruMapGet(rm, k2, &r);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(e == r, retText, e, r);

    int i = 0;
    // map order is not guaranteed but deterministic based on key value
    perm_chars keys[] = {k2, k1};
    int64_t vals[] = {v2, v1};
    perm_chars key;
    int64_t val;
    for (ret = ruMapFirst(rm, &key, &val); ret == RUE_OK;
         ret = ruMapNext(rm, &key, &val)) {
        fail_unless(keys[i] == key, retText, keys[i], key);
        fail_unless(vals[i] == val, retText, vals[i], val);
        i++;
    }
    // checking for leaks
    ret = ruMapRemove(rm, k1, &val);
    fail_unless(exp == ret, retText, exp, ret);
    fail_unless(v1 == val, retText, v1, val);
    ruMapFree(rm);
}
END_TEST

TCase* mapTests(void) {
    TCase *tcase = tcase_create ( "map" );
    tcase_add_test(tcase, api);
    tcase_add_test(tcase, run);
    tcase_add_test(tcase, dups);
    tcase_add_test(tcase, perms);
    tcase_add_test(tcase, bools);
    tcase_add_test(tcase, int8s);
    tcase_add_test(tcase, int16s);
    tcase_add_test(tcase, int32s);
    tcase_add_test(tcase, longs);
    tcase_add_test(tcase, int64s);
    tcase_add_test(tcase, custom);
    return tcase;
}
