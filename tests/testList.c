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
    int32_t ret, exp, sz;
    const char *test = "ruListAppendPtr";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    ruList rl = NULL;
    ruListElmt rle = NULL, head = NULL;
    bool is;

    exp = RUE_PARAMETER_NOT_SET;
    ret = ruListAppendPtr(rl, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    exp = RUE_INVALID_PARAMETER;
    ret = ruListAppendPtr((ruList) test, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    test = "ruListNewType";
    rl = ruListNewType(NULL);
    fail_unless(rl != NULL, retText, test, 0, RUE_OUT_OF_MEMORY);

    test = "ruListClear";
    exp = RUE_PARAMETER_NOT_SET;
    ret = ruListClear(NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    exp = RUE_OK;
    ret = ruListClear(rl);
    fail_unless(ret == exp, retText, test, exp, ret);

    exp = RUE_OK;
    test = "ruListAppend";
    ret = ruListAppend(rl, retText);
    fail_unless(ret == exp, retText, test, exp, ret);

    test = "ruListInsertBefore";
    exp = RUE_PARAMETER_NOT_SET;
    ret = ruListInsertBefore(NULL, NULL, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    exp = RUE_INVALID_PARAMETER;
    ret = ruListInsertBefore((ruList) test, NULL, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruListInsertBefore(rl, (ruListElmt) test, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    test = "ruListInsertAfter";
    exp = RUE_PARAMETER_NOT_SET;
    ret = ruListInsertAfter(NULL, NULL, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    exp = RUE_INVALID_PARAMETER;
    ret = ruListInsertAfter((ruList) test, NULL, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    ret = ruListInsertAfter(rl, (ruListElmt) test, NULL);
    fail_unless(ret == exp, retText, test, exp, ret);

    test = "ruListRemoveAfter";
    exp = RUE_PARAMETER_NOT_SET;
    ruListPop(NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);

    exp = RUE_INVALID_PARAMETER;
    ruListPop((ruList) test, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);

    test = "ruListTryPop";
    exp = RUE_PARAMETER_NOT_SET;
    void* res = ruListTryPop(NULL, 0, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(NULL == res, retText, test, NULL, res);

    test = "ruListSize";
    sz = ruListSize(NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(sz == 0, retText, test, 0, sz);

    exp = RUE_INVALID_PARAMETER;
    sz = ruListSize((ruList) test, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(sz == 0, retText, test, 0, sz);

    exp = RUE_OK;
    sz = ruListSize(rl, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(sz == 1, retText, test, 1, sz);

    test = "ruListHead";
    exp = RUE_PARAMETER_NOT_SET;
    rle = ruListHead(NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle == 0, retText, test, 0, rle);

    exp = RUE_INVALID_PARAMETER;
    rle = ruListHead((ruList) test, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle == 0, retText, test, 0, rle);

    exp = RUE_OK;
    rle = ruListHead(rl, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle != 0, retText, test, -1, rle);
    head = rle;

    rle = 0;
    rle = ruListHead(rl, NULL);
    fail_unless(rle != 0, retText, test, -1, rle);

    test = "ruListTail";
    exp = RUE_PARAMETER_NOT_SET;
    rle = ruListTail(NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle == 0, retText, test, 0, rle);

    exp = RUE_INVALID_PARAMETER;
    rle = ruListTail((ruList) test, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle == 0, retText, test, 0, rle);

    exp = RUE_OK;
    rle = ruListTail(rl, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle != 0, retText, test, -1, rle);

    rle = 0;
    rle = ruListTail(rl, NULL);
    fail_unless(rle != 0, retText, test, -1, rle);

    test = "ruListElmtIsHead";
    exp = RUE_PARAMETER_NOT_SET;
    is = ruListElmtIsHead(NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(is == false, retText, test, false, is);

    is = ruListElmtIsHead(NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(is == false, retText, test, false, is);

    exp = RUE_INVALID_PARAMETER;
    is = ruListElmtIsHead((ruListElmt) test, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(is == false, retText, test, false, is);

    test = "ruListElmtIsTail";
    exp = RUE_PARAMETER_NOT_SET;
    is = ruListElmtIsTail(NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(is == false, retText, test, false, is);

    exp = RUE_INVALID_PARAMETER;
    is = ruListElmtIsTail((ruListElmt) test, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(is == false, retText, test, false, is);


    test = "ruListNextElmt";
    exp = RUE_PARAMETER_NOT_SET;
    rle = ruListNextElmt(NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);

    exp = RUE_INVALID_PARAMETER;
    rle = ruListNextElmt((ruListElmt) rl, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);

    exp = RUE_OK;
    rle = ruListNextElmt(head, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_if(rle == 0, retText, test, 0, rle);

    rle = ruListNextElmt(rle, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle == 0, retText, test, 0, rle);

    test = "ruListNextData";
    exp = RUE_PARAMETER_NOT_SET;
    rle = ruListNextData(NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);

    exp = RUE_INVALID_PARAMETER;
    // crashes on VS 64bit
    // rle = ruListNextData(rl, &ret);
    // fail_unless(ret == exp, retText, test, exp, ret);
    // fail_unless(rle == 0, retText, test, 0, rle);

    rle = ruListNextData(&rl, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle == 0, retText, test, 0, rle);

    rle = head;
    rle = ruListNextData(rle, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle == 0, retText, test, 0, rle);

    exp = RUE_OK;
    rle = head;
    ruListNextData(&rle, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_if(rle == 0, retText, test, 0, rle);

    ruListNextData(&rle, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle == 0, retText, test, 0, rle);

    test = "ruListIdxData";
    exp = RUE_PARAMETER_NOT_SET;
    ptr o = ruListIdxData(NULL, 0, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(0 == o, retText, test, 0, o);

    test = "ruListJoin";
    exp = RUE_PARAMETER_NOT_SET;
    o = ruListJoin(NULL, NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(0 == o, retText, test, 0, o);

    test = "ruListSort";
    ret = ruListSort(NULL);
    fail_unless(ret == exp, retText, test, exp, ret);
    ret = ruListSort(rl);
    fail_unless(ret == exp, retText, test, exp, ret);

    ruListFree(rl);
}
END_TEST

START_TEST ( usage ) {
    int32_t ret, exp;
    uint32_t sz, esz;
    const char *test = "ruListAppendPtr";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    ruList rl = NULL;
    ruListElmt rle = NULL, head = NULL;
    char *res = NULL;
    const char *f[] = {"field0", "field1", "field2"};

    exp = RUE_OK;
    rl = ruListNewType(NULL);
    // add first element
    ret = ruListAppend(rl, f[0]);
    fail_unless(ret == exp, retText, test, exp, ret);
    // verify the size
    esz = 1;
    sz = ruListSize(rl, &ret);
    fail_unless(sz == esz, retText, test, esz, sz);

    // check it
    head = ruListHead(rl, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    ck_assert_str_eq(ruIterNext(head, char*), f[0]);

    // add one on top
    ret = ruListInsertAfter(rl, rle, f[1]);
    fail_unless(ret == exp, retText, test, exp, ret);
    // verify the size
    esz = 2;
    sz = ruListSize(rl, &ret);
    fail_unless(sz == esz, retText, test, esz, sz);

    // remove again and check it
    res = ruListPop(rl, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    ck_assert_str_eq(res, f[1]);
    // verify the size
    esz = 1;
    sz = ruListSize(rl, &ret);
    fail_unless(sz == esz, retText, test, esz, sz);

    // add one to the end
    // get the end first
    rle = ruListTail(rl, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);

    // then add it
    ret = ruListInsertAfter(rl, rle, f[1]);
    fail_unless(ret == exp, retText, test, exp, ret);

    // add another
    ret = ruListAppend(rl, f[2]);
    fail_unless(ret == exp, retText, test, exp, ret);
    // verify the size
    esz = 3;
    sz = ruListSize(rl, &ret);
    fail_unless(sz == esz, retText, test, esz, sz);

    test = "ruListJoin";
    alloc_chars str = ruListJoin(rl, NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    ck_assert_str_eq(str, "field0field1field2");
    ruFree(str);

    str = ruListJoin(rl, ", ", &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    ck_assert_str_eq(str, "field0, field1, field2");
    ruFree(str);

    // iterate over the list
    int32_t i = 0;
    rle = ruListIter(rl);
    for (char* field = ruIterNext(rle, char*);
         rle; field = ruIterNext(rle, char*), i++) {
        ck_assert_str_eq(field, f[i]);
        if (i == 0) {
            ret = ruListInsertBefore(rl, rle, f[2]);
            fail_unless(ret == exp, retText, test, exp, ret);
        }
        if (i==1) {
            field = ruListRemove(rl, &rle, &ret);
            fail_unless(ret == exp, retText, test, exp, ret);
            ck_assert_str_eq(field, f[i]);
        }
        if (i == 2) {
            ret = ruListInsertBefore(rl, rle, f[1]);
            fail_unless(ret == exp, retText, test, exp, ret);
        }
    }
    fail_unless(i == esz, retText, test, i, esz);

    ret = ruListInsertBefore(rl, NULL, f[1]);
    fail_unless(ret == exp, retText, test, exp, ret);

    str = ruListJoin(rl, NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    ck_assert_str_eq(str, "field1field2field0field1field2");
    ruFree(str);

    str = ruListPop(rl, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    ck_assert_str_eq(str, "field1");

    str = NULL;
    ret = ruListPopTo(rl, str);
    fail_unless(ret == exp, retText, test, exp, ret);
    ck_assert_str_eq(str, "field2");

    ret = ruListInsertIdx(rl, 1, f[1]);
    fail_unless(ret == exp, retText, test, exp, ret);

    str = ruListJoin(rl, NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    ck_assert_str_eq(str, "field0field1field1field2");
    ruFree(str);

    ruIterator li = ruListIter(rl);
    ruIterNext(li, perm_chars);
    ruIterNext(li, perm_chars);

    str = NULL;
    ret = ruListRemoveDataTo(rl, &li, (ptr*) &str);
    fail_unless(ret == exp, retText, test, exp, ret);
    ck_assert_str_eq(str, "field1");

    i = 0;
    li = ruListIter(rl);
    for (char* field = ruIterNext(li, char*);
         li; field = ruIterNext(li, char*)) {
        ck_assert_str_eq(field, f[i++]);
    }
    fail_unless(i == esz, retText, test, i, esz);

    test = "ruListIdx";
    exp = RUE_OK;
    for (uint32_t j = 0; j < esz; j++) {
        char *field = ruListIdx(rl, j, char*, &ret);
        fail_unless(ret == exp, retText, test, exp, ret);
        ck_assert_str_eq(field, f[j]);
    }

    int32_t sesz = -(int32_t)esz;
    for (int32_t j = -1; j >= sesz; j--) {
        char *field = ruListIdx(rl, j, char*, &ret);
        fail_unless(ret == exp, retText, test, exp, ret);
        ck_assert_str_eq(field, f[esz + j]);
    }

    exp = RUE_INVALID_PARAMETER;
    res = ruListIdx(rl, esz, char*, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(NULL == res, retText, test, NULL, res);

    test = "ruListTryPop";
    exp = RUE_OK;
    while(esz--) {
        res = ruListTryPop(rl, 0, &ret);
        fail_unless(ret == exp, retText, test, exp, ret);
        fail_if(NULL == res, retText, test, NULL, res);
    }
    exp = RUE_FILE_NOT_FOUND;
    res = ruListTryPop(rl, 0, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(NULL == res, retText, test, NULL, res);

    // free the list
    ruListFree(rl);

    exp = RUE_OK;
    rl = ruListNewType(NULL);
    esz = sizeof(f)/sizeof(f[0]);
    test = "ruListAppend";
    for (uint32_t j = 0; j < esz; j++) {
        ret = ruListAppend(rl, f[j]);
        fail_unless(ret == exp, retText, test, exp, ret);
    }
    test = "ruListSize";
    sz = ruListSize(rl, &ret);
    fail_unless(sz == esz, retText, test, esz, sz);

    test = "ruListRemove";
    li = ruListIter(rl);
    for(char* field = ruIterNext(li, char*);
        li; field = ruIterNext(li, char*)) {
        ruDbgLogf("Deleting %s", field);
        ruListRemove(rl, &li, &ret);
        fail_unless(ret == exp, retText, test, exp, ret);
    }

    esz = 0;
    test = "ruListSize";
    sz = ruListSize(rl, &ret);
    fail_unless(sz == esz, retText, test, esz, sz);
    // free the list
    ruListFree(rl);
}
END_TEST

START_TEST ( memalloc ) {
    int32_t ret, i;
    #define fieldText "field #%01d"
    ruList rl = NULL;
    ruListElmt rle = NULL;
    char buf[20]; // bigger to prevent -Werror=format-truncation

    rl = ruListNewType(ruTypePtrFree());
    for (i = 1; i < 10; i++) {
        ruListAppend(rl, ruDupPrintf(fieldText, i));
    }

    // iterate over the list
    i = 1;
    rle = ruListHead(rl, &ret);
    for (char*field = ruIterNext(rle, char*);
         rle && field; field = ruIterNext(rle, char*)) {
        snprintf(&buf[0], 18, fieldText, i++);
        ck_assert_str_eq(field, buf);
    }

    // free the list
    ruListFree(rl);
    #undef fieldText
}
END_TEST

START_TEST(types) {
    int32_t ret, exp = RUE_OK;
    const char *retText = "failed wanted ret '%d' but got '%d'";
    ruList rl = NULL;
    ruIterator li = NULL;
    rusize i, sz, esz;

    do {
        bool vals[] = {false, true, true};
        bool val;
        esz = sizeof(vals) / sizeof(vals[0]);
        rl = ruListNewType(ruTypeBool());
        fail_if(NULL == rl, retText, NULL, rl);

        // load the list
        for (i = 0; i < esz; i++) {
            ret = ruListAppend(rl, &vals[i]);
            fail_unless(ret == exp, retText, exp, ret);
        }
        fail_unless(esz == i, retText, esz, i);

        sz = ruListSize(rl, &ret);
        fail_unless(ret == exp, retText, exp, ret);
        fail_unless(esz == sz, retText, esz, sz);

        // go over the list
        i = 0;
        li = ruListIter(rl);
        for (ruIterTo(li, val); li; ruIterTo(li, val)) {
            fail_unless(vals[i] == val, retText, vals[i], val);
            i++;
        }
        fail_unless(esz == i, retText, esz, i);
        // pop one for leak checking
        ret = ruListPopTo(rl, val);
        fail_unless(ret == exp, retText, exp, ret);
        fail_unless(vals[0] == val, retText, vals[0], val);
        // free the list
        rl = ruListFree(rl);
    } while(0);

    do {
        int64_t vals[] = {0, 23, 0x100000000};
        int64_t val;
        esz = sizeof(vals) / sizeof(vals[0]);
        rl = ruListNewType(ruTypeInt64());
        fail_if(NULL == rl, retText, NULL, rl);

        // load the list
        for (i = 0; i < esz; i++) {
            ret = ruListAppend(rl, &vals[i]);
            fail_unless(ret == exp, retText, exp, ret);
        }
        fail_unless(esz == i, retText, esz, i);

        sz = ruListSize(rl, &ret);
        fail_unless(ret == exp, retText, exp, ret);
        fail_unless(esz == sz, retText, esz, sz);

        // go over the list
        i = 0;
        li = ruListIter(rl);
        for (ruIterTo(li, val); li; ruIterTo(li, val)) {
            fail_unless(vals[i] == val, retText, vals[i], val);
            i++;
        }
        fail_unless(esz == i, retText, esz, i);
        // pop one for leak checking
        ret = ruListPopTo(rl, val);
        fail_unless(ret == exp, retText, exp, ret);
        fail_unless(vals[0] == val, retText, vals[0], val);
        // free the list
        rl = ruListFree(rl);
    } while(0);

    do {
        perm_chars vals[] = {"0", "23", "foo"};
        perm_chars val;
        esz = sizeof(vals) / sizeof(vals[0]);
        rl = ruListNewType(ruTypeStrRef());
        fail_if(NULL == rl, retText, NULL, rl);

        // load the list
        for (i = 0; i < esz; i++) {
            ret = ruListAppend(rl, vals[i]);
            fail_unless(ret == exp, retText, exp, ret);
        }
        fail_unless(esz == i, retText, esz, i);

        sz = ruListSize(rl, &ret);
        fail_unless(ret == exp, retText, exp, ret);
        fail_unless(esz == sz, retText, esz, sz);

        // go over the list
        i = 0;
        li = ruListIter(rl);
        for (ruIterTo(li, val); li; ruIterTo(li, val)) {
            fail_unless(vals[i] == val, retText, vals[i], val);
            ck_assert_str_eq(vals[i], val);
            i++;
        }
        fail_unless(esz == i, retText, esz, i);
        // pop one for leak checking
        ret = ruListPopTo(rl, val);
        fail_unless(ret == exp, retText, exp, ret);
        ck_assert_str_eq(vals[0], val);
        // free the list
        rl = ruListFree(rl);
    } while(0);

    do {
        perm_chars vals[] = {"0", "23", "foo"};
        alloc_chars val;
        esz = sizeof(vals) / sizeof(vals[0]);
        rl = ruListNewType(ruTypeStrDup());
        fail_if(NULL == rl, retText, NULL, rl);

        // load the list
        for (i = 0; i < esz; i++) {
            ret = ruListAppend(rl, vals[i]);
            fail_unless(ret == exp, retText, exp, ret);
        }
        fail_unless(esz == i, retText, esz, i);

        sz = ruListSize(rl, &ret);
        fail_unless(ret == exp, retText, exp, ret);
        fail_unless(esz == sz, retText, esz, sz);

        // go over the list
        i = 0;
        li = ruListIter(rl);
        for (ruIterTo(li, val); li; ruIterTo(li, val)) {
            fail_if(vals[i] == val, retText, vals[i], val);
            ck_assert_str_eq(vals[i], val);
            i++;
        }
        fail_unless(esz == i, retText, esz, i);
        // pop one for leak checking
        ret = ruListPopTo(rl, val);
        fail_unless(ret == exp, retText, exp, ret);
        ck_assert_str_eq(vals[0], val);
        ruFree(val);
        // free the list
        rl = ruListFree(rl);
    } while(0);

}
END_TEST
void foo() {
    // error checking left out for brevity
    bool b1 = false;
    bool b2 = true;
    ruList rl = ruListNewType(ruTypeBool());
    ruListAppend(rl, &b1);
    ruListAppend(rl, &b2);

    ruIterator li = ruListIter(rl);
    bool vbool;
    for (ruIterTo(li, vbool); li; ruIterTo(li, vbool)) {
        printf("%d\n", vbool);
    }
    rl = ruListFree(rl);

    rl = ruListNewType(ruTypeStrDup());
    ruListAppend(rl, "foo");
    ruListAppend(rl, "bar");

    perm_chars vstr;
    ruListIdxDataTo(rl, 0, (ptr*) &vstr);
    printf("#0: %s\n", vstr);

    ruListIdxTo(rl, 1, vstr);
    printf("#1: %s\n", vstr);

    li = ruListIter(rl);
    for (ruIterTo(li, vstr); li; ruIterTo(li, vstr)) {
        printf("%s\n", vstr);
    }
    rl = ruListFree(rl);
}
void bar() {
    // error checking left out for brevity
    long k1 = 23, v1 = 42, r = 0;
    ruMap rm = ruMapNewType(ruTypeLong(), ruTypeLong());
    ruMapPut(rm, &k1, &v1);

    ruMapGet(rm, &k1, &r);
    printf("k: %ld v: %ld\n", k1, r);
    ruMapFree(rm);
}


START_TEST (sort) {
    int32_t ret, exp = RUE_OK;
    const char *test = "ruListAppend";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";

    int32_t nums[] = {99, 23, 1, 42, 53};
    int32_t exps[] = { 1, 23,42, 53, 99};
    ruList rl = ruListNewType(ruTypeInt32());
    for (int i = 0; i < sizeof(nums)/sizeof(nums[0]); i++) {
        ret = ruListAppend(rl, &nums[i]);
        fail_unless(ret == exp, retText, test, exp, ret);
    }
    test = "ruListSort";
    ret = ruListSort(rl);
    fail_unless(ret == exp, retText, test, exp, ret);

    test = "sort result";
    for (int i = 0; i < sizeof(exps)/sizeof(exps[0]); i++) {
        int32_t num = 0;
        ret = ruListPopTo(rl, num);
        fail_unless(ret == exp, retText, test, exp, ret);
        fail_unless(exps[i] == num, retText, test, exps[i], num);
    }

    ruListFree(rl);
}
END_TEST

TCase* listTests(void) {
    TCase *tcase = tcase_create("list");
    tcase_add_test(tcase, api);
    tcase_add_test(tcase, usage);
    tcase_add_test(tcase, memalloc);
    tcase_add_test(tcase, types);
    tcase_add_test(tcase, sort);
    return tcase;
}
