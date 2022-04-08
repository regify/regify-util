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
    const char *test = "ruMapNew";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";

    ruMap rm = ruMapNew(NULL, NULL, NULL, NULL, 0);
    fail_unless(NULL == rm, retText, test, NULL, rm);

    rm = ruMapNew(ruStrHash, NULL, NULL, NULL, 0);
    fail_unless(NULL == rm, retText, test, NULL, rm);

    rm = ruMapNew(NULL, ruStrMatch, NULL, NULL, 0);
    fail_unless(NULL == rm, retText, test, NULL, rm);

    ruMapFree(rm);

    exp = RUE_PARAMETER_NOT_SET;
    ret = ruMapPut(rm, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruMapIterInit(rm);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruMapNext(rm, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruMapRemoveAll(rm);
    fail_unless(exp == ret, retText, test, exp, ret);

    rm = ruMapNewString(NULL, NULL);
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

    exp = RUE_INVALID_STATE;
    ret = ruMapNext(rm, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    ret = ruMapIterInit(rm);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_PARAMETER_NOT_SET;
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

    u_int32_t sz = ruMapSize(NULL, &ret);
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
    const char *test = "ruMapNew";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";

    ruMap rm = ruMapNewString(NULL, NULL);
    fail_if(NULL == rm, retText, test, rm, NULL);

    exp = RUE_OK;
    u_int32_t esz = 0, sz = ruMapSize(rm, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    ret = ruMapPut(rm, "foo", "bar");
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    ret = ruMapIterInit(rm);
    fail_unless(exp == ret, retText, test, exp, ret);

    bool ehas = true;
    bool has = ruMapHas(rm, "foo", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(ehas == has, retText, test, ehas, has);

    ehas = false;
    has = ruMapHas(rm, "Foo", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(ehas == has, retText, test, ehas, has);

    char *ekey, *eval;
    ret = ruMapNext(rm, (void**)&ekey, (void**)&eval);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq("foo", ekey);
    ck_assert_str_eq("bar", eval);

    exp = RUE_FILE_NOT_FOUND;
    ret = ruMapNext(rm, (void**)&ekey, (void**)&eval);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(NULL == ekey, retText, test, NULL, ekey);
    fail_unless(NULL == eval, retText, test, NULL, eval);

    ret = ruMapNext(rm, (void**)&ekey, (void**)&eval);
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
    ret = ruMapGet(rm, "foo", NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    ret = ruMapGet(rm, "foo", (void**)&store);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq("bar", store);

    // update it
    ret = ruMapPut(rm, "foo", "not");
    fail_unless(exp == ret, retText, test, exp, ret);

    // should still be 1
    sz = ruMapSize(rm, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    ret = ruMapGet(rm, "foo", (void**)&store);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq("not", store);

    exp = RUE_GENERAL;
    ret = ruMapRemove(rm, "fo", (void**)&store);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    sz = ruMapSize(rm, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    ret = ruMapRemove(rm, "foo", (void**)&store);
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

TCase* mapTests ( void ) {
    TCase *tcase = tcase_create ( "map" );
    tcase_add_test ( tcase, api );
    tcase_add_test ( tcase, run );
    return tcase;
}
