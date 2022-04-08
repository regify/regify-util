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

    test = "ruListNew";
    rl = ruListNew(NULL);
    fail_unless(rl != NULL, retText, test, 0, RUE_OUT_OF_MEMORY);

    exp = RUE_OK;
    ret = ruListAppend(rl, retText);
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
    ruListRemoveAfter(NULL, NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);

    exp = RUE_INVALID_PARAMETER;
    ruListRemoveAfter((ruList) test, NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);

    ruListRemoveAfter(rl, (ruListElmt) test, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);


    test = "ruListSize";
    exp = RUE_PARAMETER_NOT_SET;
    sz = ruListSize(NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(sz == -1, retText, test, -1, sz);

    exp = RUE_INVALID_PARAMETER;
    sz = ruListSize((ruList) test, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(sz == -1, retText, test, -1, sz);

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
    is = ruListElmtIsHead(NULL, NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(is == false, retText, test, false, is);

    is = ruListElmtIsHead(rl, NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(is == false, retText, test, false, is);

    exp = RUE_INVALID_PARAMETER;
    is = ruListElmtIsHead(rl, (ruListElmt) test, &ret);
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
    fail_unless(rle == 0, retText, test, 0, rle);

    test = "ruListNextData";
    exp = RUE_PARAMETER_NOT_SET;
    rle = ruListNextData(NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);

    exp = RUE_INVALID_PARAMETER;
    rle = ruListNextData(rl, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle == 0, retText, test, 0, rle);

    rle = ruListNextData(&rl, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle == 0, retText, test, 0, rle);

    rle = head;
    rle = ruListNextData(rle, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle == 0, retText, test, 0, rle);

    exp = RUE_OK;
    rle = head;
    rle = ruListNextData(&rle, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(rle == 0, retText, test, 0, rle);

    test = "ruListElmtData";
    exp = RUE_PARAMETER_NOT_SET;
    void* ptr = ruListElmtData(NULL, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(ptr == 0, retText, test, 0, ptr);

    exp = RUE_INVALID_PARAMETER;
    rle = ruListElmtData(rl, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(ptr == 0, retText, test, 0, ptr);

    exp = RUE_OK;
    ptr = ruListElmtData(head, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_if(ptr == 0, retText, test, -1, ptr);

    ruListFree(rl);

    test = "ruListIdxData";
    exp = RUE_PARAMETER_NOT_SET;
    ptr = ruListIdxElmtData(NULL, 0, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(0 == ptr, retText, test, 0, ptr);

}
END_TEST

START_TEST ( usage ) {
    int32_t ret, exp, sz, esz;
    const char *test = "ruListAppendPtr";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    ruList rl = NULL;
    ruListElmt rle = NULL, head = NULL;
    char *res = NULL;
    const char *f[] = {"field0", "field1", "field2"};

    exp = RUE_OK;
    rl = ruListNew(NULL);
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
    ck_assert_str_eq(ruListData(head, char*, &ret), f[0]);

    // add one on top
    ret = ruListInsertAfter(rl, rle, f[1]);
    fail_unless(ret == exp, retText, test, exp, ret);
    // verify the size
    esz = 2;
    sz = ruListSize(rl, &ret);
    fail_unless(sz == esz, retText, test, esz, sz);

    // remove again and check it
    res = ruListRemoveAfter(rl, rle, &ret);
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

    // iterate over the list
    int32_t i = 0;
    rle = ruListHead(rl, &ret);
    for (char*field = ruListData(rle, char*, &ret);
         ret == RUE_OK && field;
        field = ruListNext(&rle, char*, &ret)) {
        ck_assert_str_eq(field, f[i++]);
    }
    fail_unless(i == esz, retText, test, i, esz);

    i = 0;
    ruIterator li = ruListIter(rl);
    for (char*field = ruIterCurrent(li, char*);
         li; field = ruIterNext(li, char*)) {
        ck_assert_str_eq(field, f[i++]);
    }
    fail_unless(i == esz, retText, test, i, esz);

    test = "ruListIdxData";
    exp = RUE_OK;
    for (i = 0; i < esz; i++) {
        char *field = ruListIdxData(rl, i, char*, &ret);
        fail_unless(ret == exp, retText, test, exp, ret);
        ck_assert_str_eq(field, f[i]);
    }

    exp = RUE_INVALID_PARAMETER;
    res = ruListIdxData(rl, esz, char*, &ret);
    fail_unless(ret == exp, retText, test, exp, ret);
    fail_unless(NULL == res, retText, test, NULL, res);

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

    rl = ruListNew(free);
    for (i = 1; i < 10; i++) {
        ruListAppend(rl, ruDupPrintf(fieldText, i));
    }

    // iterate over the list
    i = 1;
    rle = ruListHead(rl, &ret);
    for (char*field = ruListData(rle, char*, &ret);
         ret == RUE_OK && field;
        field = ruListNext(rle, char*, &ret)) {
        snprintf(&buf[0], 18, fieldText, i++);
        ck_assert_str_eq(field, buf);
    }

    // free the list
    ruListFree(rl);
    #undef fieldText
}
END_TEST

TCase* listTests ( void ) {
    TCase *tcase = tcase_create ( "list" );
    tcase_add_test ( tcase, api );
    tcase_add_test ( tcase, usage );
    tcase_add_test ( tcase, memalloc );
    return tcase;
}
