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

START_TEST ( api ) {
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
    reti = ruStat(NULL, NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    reti = ruStat("/tmp", NULL);
    fail_unless(exp == ret, retText, test, exp, ret);

    test = "ruBaseName";
    char *expres = NULL;
    char *res = ruBaseName(NULL);
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

START_TEST ( filetest ) {

    const char*file = makePath("foess");
    bool ret, exp;
    const char *test = "ruFileExists";
    const char *retText = "%s failed wanted ret '%d' but got '%d'";

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

    ret = ruIsExecutable(file);
    fail_unless(exp == ret, retText, test, exp, ret);

#ifndef _WIN32
    ret = ruIsSymlink(file);
    fail_unless(exp == ret, retText, test, exp, ret);
#endif

    file = makePath("föss");

    exp = true;
    ret = ruFileExists(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIsFile(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = false;
    ret = ruIsDir(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIsExecutable(file);
    fail_unless(exp == ret, retText, test, exp, ret);

#ifndef _WIN32
    ret = ruIsSymlink(file);
    fail_unless(exp == ret, retText, test, exp, ret);
#endif

    file = makePath("föss.bat");

    exp = true;
    ret = ruFileExists(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIsFile(file);
    fail_unless(exp == ret, retText, test, exp, ret);

#ifndef DO_IOS // borked on ios
    ret = ruIsExecutable(file);
    fail_unless(exp == ret, retText, test, exp, ret);
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
    file = makePath("löss");

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
    ret = ruIsExecutable(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIsDir(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    file = makePath("loeh");

    exp = true;
    ret = ruFileExists(file);
    fail_unless(exp == ret, retText, test, exp, ret);

#ifndef DO_IOS // borked on ios
    ret = ruIsSymlink(file);
    fail_unless(exp == ret, retText, test, exp, ret);
#endif

    ret = ruIsDir(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    ret = ruIsExecutable(file);
    fail_unless(exp == ret, retText, test, exp, ret);

    exp = false;
    ret = ruIsFile(file);
    fail_unless(exp == ret, retText, test, exp, ret);

#endif

    file = "/bar";
    char *expres = "bar";
    char *res = ruBaseName(file);
    ck_assert_str_eq(expres, res);

    expres = "/";
    res = ruDirName(file);
    ck_assert_str_eq(expres, res);
    ruFree(res);

    file = "/bar/";
    expres = "";
    res = ruBaseName(file);
    ck_assert_str_eq(expres, res);

    expres = "/bar/";
    res = ruDirName(file);
    ck_assert_str_eq(expres, res);
    ruFree(res);

    file = "bar";
    expres = "bar";
    res = ruBaseName(file);
    ck_assert_str_eq(expres, res);

    expres = NULL;
    res = ruDirName(file);
    fail_unless(expres == res, retText, test, expres, res);

    file = "/";
    expres = "";
    res = ruBaseName(file);
    ck_assert_str_eq(expres, res);

    expres = "/";
    res = ruDirName(file);
    ck_assert_str_eq(expres, res);
    ruFree(res);

    file = "";
    expres = "";
    res = ruBaseName(file);
    ck_assert_str_eq(expres, res);

    expres = NULL;
    res = ruDirName(file);
    fail_unless(expres == res, retText, test, expres, res);

    test = "ruFullPath";
    expres = NULL;
    res = ruFullPath("con");
    fail_if(expres == res, retText, test, expres, res);
    ruFree(res);

    res = ruFullPath(NULL);
    fail_if(expres == res, retText, test, expres, res);
    ruFree(res);

}
END_TEST


START_TEST ( fileopen ) {
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
    const char *writtenText = "Little mary had ä lamb";
    exp = RUE_PARAMETER_NOT_SET;
    ret = ruFileSetContents(NULL, NULL, 0);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileSetContents(writeFile, NULL, 0);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileSetContents(NULL, writtenText, 0);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileSetContents(writeFile, writtenText, 0);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileGetContents(NULL, NULL, 0);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ret = ruFileGetContents(writeFile, NULL, 0);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    exp = RUE_OK;
    ret = ruFileSetContents(writeFile, writtenText, -1);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    char *readText = NULL;
    rusize len = 0;
    rusize expLen = strlen(writtenText);
    ret = ruFileGetContents(writeFile, &readText, &len);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());
    fail_unless(expLen == len, retText, test, expLen, len);
    ck_assert_str_eq(writtenText, readText);
    ruFree(readText);

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
    test = "ruMkdir";
    ret = ruMkdir(newFolder, 0775, true);
    fail_unless(exp == ret, retText, test, exp, ret);
    fail_unless(true == ruIsDir(newFolder), retText, test, true, false);

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
    ruFree(writeFile);

    ret = ruFolderRemove(base);
    fail_unless(exp == ret, retErrText, test, exp, ret, ruLastError());

    ruFree(base);
    ruFree(tmpDir);
}
END_TEST


TCase* ioTests ( void ) {
    TCase *tcase = tcase_create ( "io" );
    tcase_add_test ( tcase, api );
    tcase_add_test ( tcase, filetest );
    tcase_add_test ( tcase, fileopen );
    return tcase;
}
