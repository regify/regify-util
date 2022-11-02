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
    const char *test = "ruNewFileStore";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    const char *retErrText = "%s failed wanted ret '%d' but got '%d' error '%s'";
    char *kvDir = insureTestFolder("kvstoreApi");

    exp = RUE_PARAMETER_NOT_SET;
    KvStore *evs = NULL, *kvs = ruNewFileStore (NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(evs == kvs, retText, test, evs, kvs);

    ret = ruFileStoreSet(NULL, NULL, NULL, 0);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruFileStoreList(NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    kvs = ruNewFileStore(kvDir, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_PARAMETER_NOT_SET;
    ret = ruFileStoreSet(kvs, NULL, NULL, 0);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruFileStoreList(kvs, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruFileStoreList(kvs, "*", NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruFileStoreGet(NULL, NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruFileStoreGet(kvs->ctx, NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    const char* key = "foo";
    ret = ruFileStoreGet(kvs->ctx, key, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    char* val = NULL;
    ret = ruFileStoreGet(kvs->ctx, key, &val, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_INVALID_PARAMETER;
    ret = ruValidStore((KvStore*)key);
    fail_unless(exp == ret, retText, test, exp, ret);

    rusize len = 0;
    key = "";
    ret = ruFileStoreGet(kvs->ctx, key, &val, &len);
    fail_unless(exp == ret, retText, test, exp, ret);

    ruFreeStore(kvs);

    exp = RUE_OK;
    ret = ruFolderRemove(kvDir);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ruFree(kvDir);
}
END_TEST

START_TEST ( run ) {
    int32_t ret, exp;
    const char *test = "ruNewFileStore";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    char *kvDir = insureTestFolder("kvstoreRun");
    const char* key = "!https://test.foo-bar.com config.json";
    const char* val = "foo\0bar\0\n";
    rusize len = 9;
    ruList lst = NULL;

    exp = RUE_OK;
    KvStore *evs = NULL;
    KvStore *kvs = ruNewFileStore(kvDir, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(evs != kvs, retText, test, evs, kvs);

    test = "ruFileStoreSet";
    exp = RUE_INVALID_PARAMETER;
    ret = ruFileStoreSet(kvs->ctx, key, val, len);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    ret = ruFileStoreSet(kvs, key, val, len);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruIsFile";
    char* filepath = ruDupPrintf("%s/_21https_3a_2f_2ftest.foo-bar.com/config.json", kvDir);
    fail_unless(true == ruIsFile(filepath), retText, test, true, false);

    ret = ruFileStoreList(kvs, "*", &lst);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_if(NULL == lst, retText, test, NULL, lst);

    test = "ruListIdxData";
    char *rkey = ruListIdxData(lst, 0, char*, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    ck_assert_str_eq(rkey, key);

    test = "ruListSize";
    rusize sz = ruListSize(lst, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(1 == sz, retText, test, 1, sz);

    lst = ruListFree(lst);

    test = "ruFileStoreGet";
    char* eval = NULL, *rval = NULL;
    rusize elen = 0, rlen = 0;
    exp = RUE_INVALID_PARAMETER;
    ret = ruFileStoreGet(kvs->ctx, key, &rval, &rlen);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(eval == rval, retText, test, eval, rval);

    exp = RUE_OK;
    ret = ruFileStoreSet(kvs, key, NULL, 0);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_FILE_NOT_FOUND;
    ret = ruFileStoreGet(kvs, key, &rval, &rlen);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruFileStoreGet(kvs, "none", &rval, &rlen);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(eval == rval, retText, test, eval, rval);

    ret = ruFileStoreGet(kvs, key, &rval, &rlen);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(elen == rlen, retText, test, elen, rlen);

    key = "!https://test.foo-bar.com *";
    exp = RUE_OK;
    val = NULL;
    ret = ruFileStoreSet(kvs, key, val, len);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruIsFile";
    fail_unless(false == ruIsFile(filepath), retText, test, false, true);

    // clear
    key = "*";
    ret = ruFileStoreSet(kvs, key, val, len);
    fail_unless(exp == ret, retText, test, exp, ret);

    ruFree(rval);
    ruFree(filepath);
    ruFree(kvDir);
    ruFreeStore(kvs);
}
END_TEST

START_TEST ( usage ) {
    int32_t ret, exp;
    const char *test = "ruNewFileStore";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    char *kvDir = insureTestFolder("kvstoreUsage");
    const char* key = "!https://test.foo-bar.com config.json";
    const char* val = "foo\0bar\0\n";
    rusize len = 9;

    exp = RUE_OK;
    KvStore *evs = NULL;
    KvStore *kvs = ruNewFileStore(kvDir, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(evs != kvs, retText, test, evs, kvs);

    test = "ruFileStoreSet";
    exp = RUE_INVALID_PARAMETER;
    ret = kvs->set(kvs->ctx, key, val, len);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruValidStore(kvs->ctx);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    ret = kvs->set(kvs, key, val, len);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruValidStore(kvs);
    fail_unless(exp == ret, retText, test, exp, ret);

    char *rval = NULL;
    rusize elen = 0, rlen = 0;
    exp = RUE_INVALID_PARAMETER;
    ret = kvs->get(kvs->ctx, key, &rval, &rlen);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(elen == rlen, retText, test, elen, rlen);

    exp = RUE_OK;
    elen = len;
    ret = kvs->get(kvs, key, &rval, &rlen);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(elen == rlen, retText, test, elen, rlen);

    exp = RUE_INVALID_PARAMETER;
    key = "*";
    ret = kvs->set(kvs->ctx, key, NULL, 0);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    ret = kvs->set(kvs, key, NULL, 0);
    fail_unless(exp == ret, retText, test, exp, ret);

    ruFree(rval);
    ruFree(kvDir);
    ruFreeStore(kvs);
}
END_TEST

TCase* storeTests ( void ) {
    TCase *tcase = tcase_create ( "kvstore" );
    tcase_add_test ( tcase, api );
    tcase_add_test ( tcase, run );
    tcase_add_test ( tcase, usage );
    return tcase;
}
