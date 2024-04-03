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
#if _WIN32
#define close _close
#endif

START_TEST(api) {
    bool ret, exp;
    const char *test = "ruFileExists";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";

    exp = false;
    ret = ruFileExists(NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruIsFile";
    ret = ruIsFile(NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruIsDir";
    ret = ruIsDir(NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruIsExecutable";
    ret = ruIsExecutable(NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

#ifndef _WIN32
    test = "ruIsSymlink";
    ret = ruIsSymlink(NULL);
    fail_unless(exp == ret, retText, test, exp, ret);
#endif

    test = "ruMkdir";
    int expi = RUE_PARAMETER_NOT_SET;
    int reti = ruMkdir(NULL, 0, false);
    fail_unless(expi == reti, retText, test, expi, reti);

#ifdef _WIN32
    expi = RUE_FILE_NOT_FOUND;
#else
    expi = RUE_INVALID_PARAMETER;
#endif
    reti = ruMkdir("/foo/bar", 0, false);
    fail_unless(expi == reti, retText, test, expi, reti);

#ifdef _WIN32
    expi = RUE_INVALID_PARAMETER;
    reti = ruMkdir("//foo/bar", 0666, false);
    fail_unless(expi == reti, retText, test, expi, reti);
#endif

    expi = RUE_PARAMETER_NOT_SET;
    test = "ruStat";
    reti = ruStat(NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    reti = ruStat("/tmp", NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruFileSize";
    reti = 0;
    rusize sz = 23;
    rusize esz = 0;
    sz = ruFileSize(NULL, &reti);
    fail_unless(expi == reti, retText, test, expi, reti);
    fail_unless(esz == sz, retText, test, esz, sz);

    expi = RUE_FILE_NOT_FOUND;
    sz = ruFileSize("/foo", &reti);
    fail_unless(expi == reti, retText, test, expi, reti);
    fail_unless(esz == sz, retText, test, esz, sz);

    test = "ruDiskFree";
    expi = RUE_PARAMETER_NOT_SET;
    reti = ruDiskFree(NULL, NULL, NULL);
    fail_unless(expi == reti, retText, test, expi, reti);


    test = "ruFileSetUtcTime";
    expi = RUE_PARAMETER_NOT_SET;
    reti = ruFileSetUtcTime(NULL, -1);
    fail_unless(expi == reti, retText, test, expi, reti);

    expi = RUE_INVALID_PARAMETER;
    reti = ruFileSetUtcTime("/foo", -1);
    fail_unless(expi == reti, retText, test, expi, reti);

    expi = RUE_FILE_NOT_FOUND;
    reti = ruFileSetUtcTime("/foo", 0);
    fail_unless(expi == reti, retText, test, expi, reti);

    test = "ruFileExtension";
    expi = RUE_PARAMETER_NOT_SET;
    char *expres = NULL;
    char *res = (char*)ruFileExtension(NULL);
    fail_unless(expres == res, retText, test, expres, res);

    test = "ruBaseName";
    res = (char*)ruBaseName(NULL);
    fail_unless(expres == res, retText, test, expres, res);

    test = "ruDirName";
    res = ruDirName(NULL);
    fail_unless(expres == res, retText, test, expres, res);

    test = "ruFolderRemove";
    reti = ruFolderRemove(NULL);
    fail_unless(expi == reti, retText, test, expi, reti);

    expi = RUE_INVALID_PARAMETER;
    reti = ruFolderRemove("");
    fail_unless(expi == reti, retText, test, expi, reti);

    reti = ruFolderRemove("/");
    fail_unless(expi == reti, retText, test, expi, reti);

#ifdef _WIN32
    reti = ruFolderRemove("\\");
    fail_unless(expi == reti, retText, test, expi, reti);
#endif

}
END_TEST

START_TEST(filetest) {

    perm_chars file = makePath("foess");
    bool ret, exp;
    perm_chars test = "ruFileExists";
    perm_chars retText = "%s failed wanted ret '%d' but got '%d'";

    exp = true;
    ret = ruFileExists(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIsFile(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    ruStat_t rst;
    int reti, expi = RUE_OK;
    reti = ruStat(file, &rst);
    fail_unless(expi == reti, retText, test, expi, reti);

    exp = false;
    ret = ruIsDir(file);
    fail_unless(exp == ret, retText, test, exp, ret);

#ifndef ON_SMB_SHARE
    ret = ruIsExecutable(file);
    fail_unless(exp == ret, retText, test, exp, ret);
#endif

#ifndef _WIN32
    ret = ruIsSymlink(file);
    fail_unless(exp == ret, retText, test, exp, ret);
#endif

    expi = RUE_PARAMETER_NOT_SET;
    test = "ruDiskFree";
    uint64_t tot = -1, avail = -1;
    reti = ruDiskFree(file, NULL, NULL);
    fail_unless(expi == reti, retText, test, expi, reti);

    expi = RUE_OK;
    reti = ruDiskFree(file, &tot, &avail);
    fail_unless(expi == reti, retText, test, expi, reti);
    fail_if(0 >= tot, retText, test, 0, tot);
    fail_if(0 > avail, retText, test, 0, avail);
    //ruVerbLogf("Disk space on %s free: %ld of: %ld", file, avail, tot);

    file = makePath("föss");

    exp = true;
    ret = ruFileExists(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIsFile(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = false;
    ret = ruIsDir(file);
    fail_unless(exp == ret, retText, test, exp, ret);

#ifndef ON_SMB_SHARE
    ret = ruIsExecutable(file);
    fail_unless(exp == ret, retText, test, exp, ret);
#endif

#ifndef _WIN32
    ret = ruIsSymlink(file);
    fail_unless(exp == ret, retText, test, exp, ret);
#endif

    test = "ruDiskFree";
    tot = avail = -1;
    reti = ruDiskFree(file, &tot, &avail);
    fail_unless(expi == reti, retText, test, expi, reti);
    fail_if(0 >= tot, retText, test, 0, tot);
    fail_if(0 > avail, retText, test, 0, avail);
    //ruVerbLogf("Disk space on %s free: %ld of: %ld", file, avail, tot);

    file = makePath("föss.bat");

    exp = true;
    ret = ruFileExists(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIsFile(file);
    fail_unless(exp == ret, retText, test, exp, ret);

#ifndef DO_IOS // borked on ios
#ifndef ON_SMB_SHARE
    ret = ruIsExecutable(file);
    fail_unless(exp == ret, retText, test, exp, ret);
#endif
#endif

    exp = false;
    ret = ruIsDir(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    file = makePath("döh");

    exp = true;
    ret = ruFileExists(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIsDir(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = false;
    ret = ruIsFile(file);
    fail_unless(exp == ret, retText, test, exp, ret);

#ifndef _WIN32
    file = makeOutPath("löss");

    exp = true;
    ret = ruFileExists(file);
    fail_unless(exp == ret, retText, test, exp, ret);

#ifndef DO_IOS // borked on ios
    ret = ruIsSymlink(file);
    fail_unless(exp == ret, retText, test, exp, ret);
#endif

    ret = ruIsFile(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = false;
#ifndef ON_SMB_SHARE
    ret = ruIsExecutable(file);
    fail_unless(exp == ret, retText, test, exp, ret);
#endif

    ret = ruIsDir(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    file = makeOutPath("loeh");

    exp = true;
    ret = ruFileExists(file);
    fail_unless(exp == ret, retText, test, exp, ret);

#ifndef DO_IOS // borked on ios
    ret = ruIsSymlink(file);
    fail_unless(exp == ret, retText, test, exp, ret);
#endif

    ret = ruIsDir(file);
    fail_unless(exp == ret, retText, test, exp, ret);

#ifndef ON_SMB_SHARE
    ret = ruIsExecutable(file);
    fail_unless(exp == ret, retText, test, exp, ret);
#endif

    exp = false;
    ret = ruIsFile(file);
    fail_unless(exp == ret, retText, test, exp, ret);

#endif

    alloc_chars res = NULL;
    perm_chars pres;
    file = "/bar";
    char* expres = "bar";
    pres = ruBaseName(file);
    ck_assert_str_eq(expres, pres);

    expres = "";
    res = ruDirName(file);
    ck_assert_str_eq(expres, res);
    ruFree(res);

    file = "/bar/";
    expres = "";
    pres = ruBaseName(file);
    ck_assert_str_eq(expres, pres);

    expres = "/bar";
    res = ruDirName(file);
    ck_assert_str_eq(expres, res);
    ruFree(res);

    file = "bar";
    expres = "bar";
    pres = ruBaseName(file);
    ck_assert_str_eq(expres, pres);

    expres = NULL;
    res = ruDirName(file);
    fail_unless(expres == res, retText, test, expres, res);

    file = "/";
    expres = "";
    pres = ruBaseName(file);
    ck_assert_str_eq(expres, pres);

    expres = "";
    res = ruDirName(file);
    ck_assert_str_eq(expres, res);
    ruFree(res);

    file = "/foo/bar";
    expres = "bar";
    pres = ruBaseName(file);
    ck_assert_str_eq(expres, pres);

    expres = "/foo";
    res = ruDirName(file);
    ck_assert_str_eq(expres, res);
    ruFree(res);

    file = "";
    expres = "";
    pres = ruBaseName(file);
    ck_assert_str_eq(expres, pres);

    expres = NULL;
    res = ruDirName(file);
    fail_unless(expres == res, retText, test, expres, res);


    test = "ruFileExtension";
    expres = NULL;
    pres = ruFileExtension("");
    fail_unless(expres == pres, retText, test, expres, pres);

    pres = ruFileExtension("foo");
    fail_unless(expres == pres, retText, test, expres, pres);

    pres = ruFileExtension(".foo");
    fail_unless(expres == pres, retText, test, expres, pres);

    pres = ruFileExtension("fo/.bar");
    fail_unless(expres == pres, retText, test, expres, pres);

    pres = ruFileExtension("foo.d/bar");
    fail_unless(expres == pres, retText, test, expres, pres);

    pres = ruFileExtension("foo.d");
    ck_assert_str_eq("d", pres);

    pres = ruFileExtension("foo/.bar.x");
    ck_assert_str_eq("x", pres);

    pres = ruFileExtension("bar.x.y");
    ck_assert_str_eq("y", pres);

    pres = ruFileExtension(".a.b.c");
    ck_assert_str_eq("c", pres);

    test = "ruFullPath";
    expres = NULL;
    res = ruFullPath("con");
    fail_if(expres == res, retText, test, expres, res);
    ruFree(res);

#if defined(RUMS)
    expres = "C:\\con";
#elif defined(_WIN32)
    expres = "C:/con";
#else
    expres = "/con";
#endif
    res = ruFullPath("/con");
    fail_if(expres == res, retText, test, expres, res);
    ck_assert_str_eq(res, expres);
    ruFree(res);

#ifdef _WIN32
    expres = "C:\\con";
#endif
    res = ruFullPath("\\con");
    fail_if(expres == res, retText, test, expres, res);
    ruFree(res);

    res = ruFullPath(NULL);
    fail_if(expres == res, retText, test, expres, res);
    ruFree(res);

    test = "ruPathJoin";
    expres = NULL;
    res = ruPathJoin(NULL, NULL);
    fail_unless(expres == res, retText, test, expres, res);

    expres = "base";
    res = ruPathJoin(expres, NULL);
    ck_assert_str_eq(expres, res);
    ruFree(res);

    res = ruPathJoin(NULL, expres);
    ck_assert_str_eq(expres, res);
    ruFree(res);

    res = ruPathJoinNative(expres, expres);
    expres = "base" RU_SLASH_S "base";
    ck_assert_str_eq(expres, res);
    ruFree(res);

    expres = "base";
    res = ruPathJoin(expres, expres);
    expres = "base/base";
    ck_assert_str_eq(expres, res);
    ruFree(res);

    test = "ruPathMultiJoin";
    expres = NULL;
    res = ruPathMultiJoin(-1);
    fail_unless(expres == res, retText, test, expres, res);

    res = ruPathMultiJoin(0);
    fail_unless(expres == res, retText, test, expres, res);

    res = ruPathMultiJoin(1, "/foo/");
    expres = "/foo";
    ck_assert_str_eq(expres, res);
    ruFree(res);

    res = ruPathMultiJoinNative(1, RU_SLASH_S "foo" RU_SLASH_S);
    expres = RU_SLASH_S "foo";
    ck_assert_str_eq(expres, res);
    ruFree(res);

    res = ruPathMultiJoin(1, "foo", "bar");
    expres = "foo";
    ck_assert_str_eq(expres, res);
    ruFree(res);

    res = ruPathMultiJoin(2, "foo", "bar");
    expres = "foo/bar";
    ck_assert_str_eq(expres, res);
    ruFree(res);

    res = ruPathMultiJoinNative(2, "foo", "bar");
    expres = "foo" RU_SLASH_S "bar";
    ck_assert_str_eq(expres, res);
    ruFree(res);

    res = ruPathMultiJoin(2, "/foo/", "/bar/");
    expres = "/foo/bar";
    ck_assert_str_eq(expres, res);
    ruFree(res);

    res = ruPathMultiJoinNative(2, RU_SLASH_S "foo" RU_SLASH_S, RU_SLASH_S "bar" RU_SLASH_S);
    expres = RU_SLASH_S "foo" RU_SLASH_S "bar";
    ck_assert_str_eq(expres, res);
    ruFree(res);

}
END_TEST


START_TEST(fileopen) {
    int ret, exp, fd;
    FILE *fl;
    const char *test = "ruOpenTmp";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";
    const char *retErrText = "%s failed wanted ret '%d' but got '%d' error '%s'";
    char *tmpDir = insureTestFolder(NULL);

    char *tmpFile = ruDupPrintf("%s/tmp-^^^.txt", tmpDir);
    char *fooFile = ruDupPrintf("%s/foo/tmp-^^^.txt", tmpDir);

    exp = RUE_PARAMETER_NOT_SET;
    fd = ruOpenTmp(NULL, 0, 0, &ret);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    fd = ruOpen(NULL, 0, 0, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    fl = ruFOpen(NULL, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    fd = ruOpenTmp(NULL, 0, 0, &ret);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    fl = ruFOpen(tmpFile, NULL, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    fd = ruOpen(NULL, O_WRONLY, 0, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    fl = ruFOpen(NULL, "w", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_FILE_NOT_FOUND;
    fd = ruOpen(tmpFile, 0, 0, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_INVALID_PARAMETER;
    fd = ruOpenTmp(tmpFile, 0, 0, &ret);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    fd = ruOpenTmp(tmpFile, O_WRONLY, 0, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    fd = ruOpenTmp(tmpFile, O_CREAT | O_WRONLY, 0666, &ret);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());
    close(fd);
    ruFileRemove(tmpFile);

    fd = ruOpen(tmpFile, O_CREAT|O_WRONLY, 0666, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    close(fd);
    ruFileRemove(tmpFile);

#ifdef _WIN32
    exp = RUE_CANT_WRITE;
#else
    exp = RUE_FILE_NOT_FOUND;
#endif
    fd = ruOpen(fooFile, O_CREAT|O_WRONLY, 0666, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = RUE_OK;
    fl = ruFOpen(tmpFile, "w", &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fclose(fl);
    ruFileRemove(tmpFile);

    ruFree(tmpFile);
    ruFree(fooFile);

    const char *writeFile = ruDupPrintf("%s/ru-outfile.txt", tmpDir);
    const char *writtenText = "Little mary\r\nhad ä lamb\n\n";
    exp = RUE_PARAMETER_NOT_SET;
    ret = ruFileSetContents(NULL, NULL, 0);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileSetContents(NULL, writtenText, 0);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileGetContents(NULL, NULL, 0);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileGetContents(writeFile, NULL, 0);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileCopy(NULL, NULL);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileCopy(writeFile, NULL);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileCopy(NULL, writeFile);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    exp = RUE_FILE_NOT_FOUND;
    ret = ruFileCopy("/foo", writeFile);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileCopy(writeFile, "/some/path/in/nirvana");
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    exp = RUE_OK;
    ret = ruFileSetContents(writeFile, NULL, 0);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    rusize len = 0;
    len = ruFileSize(writeFile, &ret);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());
    fail_unless(0 == len, retText, test, 0, len);
    ret = ruFileRemove(writeFile);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileSetContents(writeFile, writtenText, 0);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    len = ruFileSize(writeFile, &ret);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());
    fail_unless(0 == len, retText, test, 0, len);
    ret = ruFileRemove(writeFile);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileSetContents(writeFile, writtenText, RU_SIZE_AUTO);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    alloc_chars writeCopy = ruDupPrintf("%s/ru-outcopy.txt", tmpDir);
    ret = ruFileCopy(writeFile, writeCopy);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    exp = RUE_CANT_WRITE;
    ret = ruFileTryRename(writeFile, writeCopy);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    exp = RUE_OK;
    char *readText = NULL;
    len = 0;
    rusize expLen = strlen(writtenText);
    ret = ruFileGetContents(writeCopy, &readText, &len);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());
    fail_unless(expLen == len, retText, test, expLen, len);
    ck_assert_str_eq(writtenText, readText);
    ruFree(readText);

    ret = ruFileRename(writeFile, writeCopy);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    bool want = false, is = ruFileExists(writeFile);
    fail_unless(want == is, retText, test, want, is);

    ret = ruFileTryRename(writeCopy, writeFile);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());
    ruFree(writeCopy);

    expLen = 11;
    ret = ruFileSetContents(writeFile, writtenText, expLen);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruFileGetContents(writeFile, &readText, &len);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(expLen == len, retText, test, expLen, len);
    ck_assert_str_eq("Little mary", readText);
    ruFree(readText);

    test = "ruFileRemove";
    ruFileRemove(writeFile);
    bool exists = ruFileExists(writeFile);
    fail_unless(false == exists, retText, test, false, exists);

    expLen = 11;
    ret = ruFileSetContents(writeFile, writtenText, expLen);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruFolderRemove";
    ruFolderRemove(writeFile);
    exists = ruFileExists(writeFile);
    fail_unless(false == exists, retText, test, false, exists);
    ruFree(writeFile);

    const char *base = ruDupPrintf("%s/töst", tmpDir);
    const char *newFolder = ruDupPrintf("%s/this", base);

    test = "ruMkdir";
    exp = RUE_FILE_NOT_FOUND;
    ret = ruMkdir(newFolder, 0775, false);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(false == ruIsDir(newFolder), retText, test, false, true);

    exp = RUE_OK;
    ret = ruMkdir(newFolder, 0775, true);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(true == ruIsDir(newFolder), retText, test, true, false);

    ruFileRemove(newFolder);
    test = "ruFileExists";
    exists = ruFileExists(newFolder);
    fail_unless(false == exists, retText, test, false, exists);
    ruFree(newFolder);

    // w/trailing slash
    newFolder = ruDupPrintf("%s/that/", base);

    ru_int ecnt = 0, cnt;
    test = "ruFolderEntries";
    cnt = ruFolderEntries(newFolder);
    fail_unless(ecnt == cnt, retText, test, ecnt, cnt);

    test = "ruMkdir";
    ret = ruMkdir(newFolder, 0775, true);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(true == ruIsDir(newFolder), retText, test, true, false);

    test = "ruFolderEntries";
    ecnt = 1;
    cnt = ruFolderEntries(newFolder);
    fail_unless(ecnt == cnt, retText, test, ecnt, cnt);

    ecnt = 2;
    cnt = ruFolderEntries(base);
    fail_unless(ecnt == cnt, retText, test, ecnt, cnt);

    ruFileRemove(newFolder);
    test = "ruFileExists";
    exists = ruFileExists(newFolder);
    fail_unless(false == exists, retText, test, false, exists);

    ruFileRemove(base);
    exists = ruFileExists(base);
    fail_unless(false == exists, retText, test, false, exists);

    ruFree(newFolder);

    // build up a folder structure
    newFolder = ruDupPrintf("%s/some/deep/dirs/", base);
    test = "ruMkdir";
    ret = ruMkdir(newFolder, 0775, true);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(true == ruIsDir(newFolder), retText, test, true, false);
    ruFree(newFolder);

    writeFile = ruDupPrintf("%s/some/file.txt", base);
    expLen = 11;
    ret = ruFileSetContents(writeFile, writtenText, expLen);
    fail_unless(exp == ret, retText, test, exp, ret);
    ruFree(writeFile);

    writeFile = ruDupPrintf("%s/some/deep/dirs/text.txt", base);
    expLen = 11;
    ret = ruFileSetContents(writeFile, writtenText, expLen);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruFileSize";
    len = ruFileSize(writeFile, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(expLen == len, retText, test, expLen, len);

    perm_chars regiboxlog = makePath("CFRunLoop.h");
    expLen = 10002;
    len = ruFileSize(regiboxlog, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(expLen == len, retText, test, expLen, len);

    test = "ruFileSetUtcTime";
    ret = ruFileSetUtcTime(writeFile, 7200);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruFileUtcTime";
    sec_t tm = ruFileUtcTime(writeFile, &ret);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(7200 == tm, retText, test, 7200, tm);

    test = "ruFolderRemove";
    ret = ruFolderRemove(base);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ruFree(writeFile);
    ruFree(base);
    ruFree(tmpDir);
}
END_TEST

typedef struct {
    ruList fileLst;
    trans_chars exclude;
} walkCtx;

static bool filter(trans_chars folderPath, trans_chars fileName, bool isFolder, ptr o) {
    walkCtx* wc = (walkCtx*)o;
    bool filter = false;
    if (ruStrEquals(wc->exclude, fileName)) filter = true;
    return filter;
}

static int32_t lister(trans_chars path, bool isFolder, ptr o) {
    walkCtx* wc = (walkCtx*)o;
    trans_chars mypath = path + ruStrLen(testBase);
    alloc_chars str = NULL;
    if (isFolder) {
        str = ruDupPrintf(" %s:d", mypath);
    } else {
        str = ruDupPrintf(" %s:f", mypath);
    }
    ruListAppend(wc->fileLst, str);
    return RUE_OK;
}

static void walk(trans_chars run, trans_chars file, int flags,
                 trans_chars exclude, trans_chars fileExp, trans_chars filterExp) {
    perm_chars retText = "%s failed wanted ret '%d' but got '%d'";
    perm_chars test = file + ruStrLen(testBase);

    walkCtx wc;
    wc.fileLst = ruListNew(ruTypeStrFree());
    wc.exclude = NULL;
    int exp = RUE_OK;
    int ret = ruFolderWalk(file, flags | RU_WALK_UNIX_SLASHES,
                           lister, &wc);
    fail_unless(exp == ret, retText, test, exp, ret);

    alloc_chars lstr = ruListJoin(wc.fileLst, " ", NULL);
    alloc_chars fileStr = ruDupPrintf("%s %s", run, lstr);
    ck_assert_str_eq(fileExp, fileStr);
    ruFree(lstr);
    ruFree(fileStr);
    wc.fileLst = ruListFree(wc.fileLst);

    wc.fileLst = ruListNew(ruTypeStrFree());
    wc.exclude = exclude;
    ret = ruFilteredFolderWalk(file, flags | RU_WALK_UNIX_SLASHES,
                               filter, lister, &wc);
    fail_unless(exp == ret, retText, test, exp, ret);

    lstr = ruListJoin(wc.fileLst, " ", NULL);
    fileStr = ruDupPrintf("%s %s", run, lstr);
    ck_assert_str_eq(filterExp, fileStr);
    ruFree(lstr);
    ruFree(fileStr);
    wc.fileLst = ruListFree(wc.fileLst);
}

START_TEST(folderwalk) {
    int ret, exp;
    perm_chars test = "ruFolderWalk";
    perm_chars retText = "%s failed wanted ret '%d' but got '%d'";
    perm_chars file = NULL;
    perm_chars fileExp = NULL;
    perm_chars exclude = NULL;
    perm_chars filterExp = NULL;
    ruString fileStr = NULL;

    exp = RUE_PARAMETER_NOT_SET;
    ret = ruFolderWalk(NULL, 0, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruFilteredFolderWalk(NULL, 0, NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    file = makePath("walker/");
    exp = RUE_OK;
    ret = ruFolderWalk(file, 0, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruFilteredFolderWalk(file, 0, NULL, NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    // sorted
    fileExp = "NR  /walker/su1/:d  /walker/su2:f";
    walk("NR", file, RU_WALK_NO_RECURSE, exclude, fileExp, fileExp);

    exclude = "su2";
    fileExp = "NRFF  /walker/:d  /walker/su1/:d  /walker/su2:f";
    filterExp = "NRFF  /walker/:d  /walker/su1/:d";
    walk("NRFF", file, RU_WALK_NO_RECURSE | RU_WALK_FOLDER_FIRST,
         exclude, fileExp, filterExp);

    fileExp = "FF  /walker/:d  /walker/su1/:d  /walker/su1/file1:f  /walker/su2:f";
    filterExp = "FF  /walker/:d  /walker/su1/:d  /walker/su1/file1:f";
    walk("FF", file, RU_WALK_FOLDER_FIRST, exclude, fileExp, filterExp);

    fileExp = "FFFL  /walker/:d  /walker/su1/:d  /walker/su1/file1:f"
              "  /walker/su1/:d  /walker/su2:f  /walker/:d";
    filterExp = "FFFL  /walker/:d  /walker/su1/:d  /walker/su1/file1:f"
              "  /walker/su1/:d  /walker/:d";
    walk("FFFL", file, RU_WALK_FOLDER_FIRST | RU_WALK_FOLDER_LAST,
         exclude, fileExp, filterExp);

    exclude = "su1";
    fileExp = "NSFFFL  /walker/su1/:d  /walker/su1/file1:f  /walker/su1/:d  /walker/su2:f";
    filterExp = "NSFFFL  /walker/su2:f";
    walk("NSFFFL", file, RU_WALK_NO_SELF | RU_WALK_FOLDER_FIRST |
        RU_WALK_FOLDER_LAST, exclude, fileExp, filterExp);

    exclude = "file1";
    fileExp = "NSFF  /walker/su1/:d  /walker/su1/file1:f  /walker/su2:f";
    filterExp = "NSFF  /walker/su1/:d  /walker/su2:f";
    walk("NSFF", file, RU_WALK_NO_SELF | RU_WALK_FOLDER_FIRST,
         exclude, fileExp, filterExp);

    exclude = "su2";
    fileExp = "NRFL  /walker/su1/:d  /walker/su2:f  /walker/:d";
    filterExp = "NRFL  /walker/su1/:d  /walker/:d";
    walk("NRFL", file, RU_WALK_NO_RECURSE | RU_WALK_FOLDER_LAST,
         exclude, fileExp, filterExp);

    fileExp =  "FL  /walker/su1/file1:f  /walker/su1/:d  /walker/su2:f  /walker/:d";
    filterExp =  "FL  /walker/su1/file1:f  /walker/su1/:d  /walker/:d";
    walk("FL", file, RU_WALK_FOLDER_LAST, exclude, fileExp, filterExp);

    fileExp =  "NSFL  /walker/su1/file1:f  /walker/su1/:d  /walker/su2:f";
    filterExp =  "NSFL  /walker/su1/file1:f  /walker/su1/:d";
    walk("NSFL", file, RU_WALK_NO_SELF | RU_WALK_FOLDER_LAST,
         exclude, fileExp, filterExp);

    exclude = NULL;
    fileExp = "0  /walker/su1/file1:f  /walker/su2:f";
    walk("0", file, 0, exclude, fileExp, fileExp);
}
END_TEST

TCase* ioTests ( void ) {
    TCase *tcase = tcase_create ( "io" );
    tcase_add_test(tcase, api);
    tcase_add_test(tcase, filetest);
    tcase_add_test(tcase, fileopen);
    tcase_add_test(tcase, folderwalk);
    return tcase;
}
