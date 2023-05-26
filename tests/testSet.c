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
    const char *test = "ruSetNew";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    ruList keys = NULL;

    ruSet rs = ruSetNew(NULL, NULL, NULL, 0);
    fail_unless(NULL == rs, retText, test, NULL, rs);

    ruSetFree(rs);

    ptr exist = NULL;
    exp = RUE_PARAMETER_NOT_SET;
    ret = ruSetFirstSet(rs, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruSetNext(NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruSetRemoveAll(rs);
    fail_unless(exp == ret, retText, test, exp, ret);

    rs = ruSetNewString(NULL);
    fail_if(NULL == rs, retText, test, rs, NULL);

    ret = ruSetPut(rs, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruSetRemove(NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruSetRemove(rs, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    char *store = "23";
    ret = ruSetItemList(rs, NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruSetItemList(NULL, &keys, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_INVALID_STATE;
    ret = ruSetNext(rs, &store);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_PARAMETER_NOT_SET;
    ret = ruSetNext(rs, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruSetFirstSet(rs, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

//    exp = RUE_PARAMETER_NOT_SET;
    ret = ruSetNext(rs, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_INVALID_PARAMETER;
    ruSetHas((void*)store, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_PARAMETER_NOT_SET;
    uint32_t sz = ruMapSize(NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(0 == sz, retText, test, 0, sz);

    exp = RUE_OK;
    sz = ruSetSize(rs, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(0 == sz, retText, test, 0, sz);

    ruSetFree(rs);
}
END_TEST

START_TEST ( run ) {
    int32_t ret, exp;
    perm_chars test = "ruSetNew";
    perm_chars retText = "%s failed wanted ret '%d' but got '%d'";
    perm_chars foo = "foo";

    ruSet rs = ruSetNewString(NULL);
    fail_if(NULL == rs, retText, test, rs, NULL);

    exp = RUE_OK;
    uint32_t esz = 0, sz = ruSetSize(rs, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    ret = ruSetPut(rs, foo);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruSetHas";
    exp = RUE_OK;
    bool ehas = true;
    bool has = ruSetHas(rs, foo, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(ehas == has, retText, test, ehas, has);

    ehas = false;
    has = ruSetHas(rs, "Foo", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(ehas == has, retText, test, ehas, has);

    test = "ruSetFirst";
    char *eitem;
    ret = ruSetFirst(rs, &eitem);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq(foo, eitem);

    test = "ruSetNext";
    exp = RUE_FILE_NOT_FOUND;
    ret = ruSetNext(rs, &eitem);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(NULL == eitem, retText, test, NULL, eitem);

    ret = ruSetNext(rs, &eitem);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(NULL == eitem, retText, test, NULL, eitem);

    test = "ruSetSize";
    exp = RUE_OK;
    esz = 1;
    sz = ruSetSize(rs, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    // update it
    test = "ruSetPut";
    ret = ruSetPut(rs, foo);
    fail_unless(exp == ret, retText, test, exp, ret);

    // should still be 1
    test = "ruSetSize";
    sz = ruSetSize(rs, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    // test a copied item set
    test = "ruSetItemList";
    ruList keys = NULL;
    ret = ruSetItemList(rs, &keys, (ruCloneFunc) ruStrDup, free);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_if(NULL == keys, retText, test, NULL, keys);

    test = "ruSetItemList";
    sz = ruListSize(keys, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);
    alloc_chars store = ruListPop(keys, &ret);
    ck_assert_str_eq(foo, store);
    keys = ruListFree(keys);
    ruFree(store);

    // test a scalar keyset
    test = "ruSetItemList";
    ret = ruSetItemList(rs, &keys, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_if(NULL == keys, retText, test, NULL, keys);

    test = "ruListSize";
    sz = ruListSize(keys, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);
    store = ruListPop(keys, &ret);
    ck_assert_str_eq(foo, store);
    fail_unless(foo == store, retText, test, foo, store);
    keys = ruListFree(keys);

    test = "ruSetRemove";
    exp = RUE_GENERAL;
    ret = ruSetRemove(rs, "fo");
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruSetSize";
    exp = RUE_OK;
    sz = ruSetSize(rs, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    test = "ruSetRemove";
    ret = ruSetRemove(rs, foo);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruSetSize";
    esz = 0;
    sz = ruSetSize(rs, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    test = "ruSetPut";
    ret = ruSetPut(rs, "foo1");
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruSetPut(rs, "foo2");
    fail_unless(exp == ret, retText, test, exp, ret);

    esz = 2;
    test = "ruSetSize";
    sz = ruSetSize(rs, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    test = "ruSetRemoveAll";
    ret = ruSetRemoveAll(rs);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruSetSize";
    esz = 0;
    sz = ruSetSize(rs, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    ruSetFree(rs);
}
END_TEST

START_TEST ( iter ) {
    int32_t ret, exp = RUE_OK;
    const char *test = "ruMapNew";
    const char *retText = "%s failed wanted ret '%x' but got '%x'";

    ruSet rs = ruSetNew(ruIntHash, ruIntMatch,
                        NULL, 3);
    fail_if(NULL == rs, retText, test, rs, NULL);

    test = "ruSetPut";
    ret = ruSetPut(rs, rs);
    fail_unless(exp == ret, retText, test, exp, ret);

    uint32_t esz  = 1;
    test = "ruSetSize";
    uint32_t sz = ruSetSize(rs, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(esz == sz, retText, test, esz, sz);

    ptr key = NULL;
    test = "ruMapNew";
    bool exb = true;
    for (ret = ruSetFirst(rs, &key); ret == RUE_OK;
         ret = ruSetNext(rs, &key)) {
        fail_unless(rs == key, retText, test, rs, key);
    }

    ruSetFree(rs);
}
END_TEST

TCase* setTests(void) {
    TCase *tcase = tcase_create("set");
    tcase_add_test(tcase, api);
    tcase_add_test(tcase, run);
    tcase_add_test(tcase, iter);
    return tcase;
}
