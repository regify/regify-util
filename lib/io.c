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
#include "lib.h"

// generally a 4GB chunk, but settable for tests
unsigned int ruIntChunk = (unsigned int)-1;
int ruIoChunk = 0x7ffff000;

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define open _open
#define read _read
#define close _close

wchar_t * getWPath(const char *filepath) {
    if (!filepath) return NULL;
    char *pathFixed = fixPath(filepath);
    wchar_t *wpath = charToUni(pathFixed);
    ruFree (pathFixed);
    return wpath;
}

char* getMbPath(const wchar_t *filepath) {
    if (!filepath) return NULL;
    char *path = uniToChar((UChar*)filepath);
    char *pathFixed = fixPath(path);
    ruFree (path);
    return pathFixed;
}

static int getAttrs(const char* filename) {
    int attributes;
    wchar_t *wfilename = getWPath(filename);
    if (!wfilename) return INVALID_FILE_ATTRIBUTES;
    attributes = GetFileAttributesW (wfilename);
    ruFree (wfilename);
    return attributes;
}

static int getWAttrs(const wchar_t *wfilename) {
    int attributes;
    if (!wfilename) return INVALID_FILE_ATTRIBUTES;
    attributes = GetFileAttributesW (wfilename);
    return attributes;
}

RUAPI bool ruFileExists(const char* filename) {
    if (!filename) return FALSE;
    int attributes = getAttrs(filename);
    if (attributes == INVALID_FILE_ATTRIBUTES) return FALSE;
    return TRUE;
}

RUAPI bool ruIsFile(const char* filename) {
    if (!filename) return FALSE;
    int attributes = getAttrs(filename);
    if (attributes == INVALID_FILE_ATTRIBUTES) return FALSE;
    if ((attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE)) == 0) return TRUE;
    return FALSE;
}

bool ruWIsDir(const wchar_t* filename) {
    if (!filename) return FALSE;
    int attributes = getWAttrs(filename);
    if (attributes == INVALID_FILE_ATTRIBUTES) return FALSE;
    if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) return TRUE;
    return FALSE;
}

RUAPI bool ruIsDir(const char* filename) {
    if (!filename) return FALSE;
    int attributes = getAttrs(filename);
    if (attributes == INVALID_FILE_ATTRIBUTES) return FALSE;
    if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0) return TRUE;
    return FALSE;
}

RUAPI bool ruIsExecutable(const char* filename) {
    if (!filename) return FALSE;
    // be sure the file actually exists before looking at the name
    if (!ruIsFile(filename)) return FALSE;

    const char *lastdot = strrchr (filename, '.');
    const char *pathext = NULL, *p;
    rusize extlen;

    if (lastdot == NULL) return FALSE;

    if (_stricmp (lastdot, ".exe") == 0 ||
        _stricmp (lastdot, ".cmd") == 0 ||
        _stricmp (lastdot, ".bat") == 0 ||
        _stricmp (lastdot, ".com") == 0)
        return TRUE;

    /* Check if it is one of the types listed in %PATHEXT% */

    pathext = ruGetenv("PATHEXT");
    if (pathext == NULL) return FALSE;

    pathext = ruUtf8ToLower(pathext);
    lastdot = ruUtf8ToLower(lastdot);
    extlen = strlen (lastdot);

    p = pathext;
    while (TRUE)
    {
        const char *q = strchr (p, ';');
        if (q == NULL) q = p + strlen (p);
        if (extlen == (rusize)(q - p) && memcmp (lastdot, p, extlen) == 0) {
            ruFree(pathext);
            ruFree(lastdot);
            return TRUE;
        }
        if (*q) p = q + 1;
        else break;
    }

    ruFree(pathext);
    ruFree(lastdot);
    return FALSE;
}

int ruOpen(const char *filepath, int flags, int mode, int32_t* code) {
    if (!filepath) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, 0);
    if (flags == O_RDONLY) {
        wchar_t *wpath = getWPath(filepath);
        int fd = _wopen((const wchar_t*)wpath, flags);
        ruFree (wpath);
        if (fd == -1) ruRetWithCode(code, RUE_FILE_NOT_FOUND, fd);
        ruRetWithCode(code, RUE_OK, fd);
    }
    if (!(flags & O_WRONLY) && !(flags & O_RDWR)) {
        ruRetWithCode(code, RUE_INVALID_PARAMETER, 0);
    }
    wchar_t *wpath = getWPath(filepath);
    int fd = _wopen((const wchar_t*)wpath, flags, mode);
    ruFree (wpath);
    if (fd == -1) ruRetWithCode(code, RUE_CANT_WRITE, fd);
    ruRetWithCode(code, RUE_OK, fd);
}

RUAPI FILE* ruFOpen(const char *filepath, const char *mode, int32_t* code) {
    if (!filepath) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, 0);
    if (!mode) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, 0);
    wchar_t *wpath = getWPath(filepath);
    wchar_t *wmode = charToUni(mode);
    FILE *fd = _wfopen((const wchar_t*)wpath, wmode);
    ruFree (wpath);
    ruFree (wmode);
    if (!fd) ruRetWithCode(code, RUE_CANT_OPEN_FILE, fd);
    ruRetWithCode(code, RUE_OK, fd);
}

RUAPI int32_t ruDiskFree(trans_chars path, uint64_t* total, uint64_t* avail) {
    alloc_chars dirPath = NULL;
    if (ruIsFile(path)) {
        dirPath = ruDirName(path);
        path = dirPath;
    }
    wchar_t* wpath = getWPath(path);
    if (!wpath || (!total && !avail)) return RUE_PARAMETER_NOT_SET;
    int32_t ret;
    ULARGE_INTEGER lfree;
    ULARGE_INTEGER ltot;
    if (GetDiskFreeSpaceExW(wpath, &lfree,
                            &ltot, NULL)) {
        if (total) *total = (uint64_t)ltot.QuadPart;
        if (avail) *avail = (uint64_t)lfree.QuadPart;
        ret = RUE_OK;
    } else {
        if (total) *total = 0;
        if (avail) *avail = 0;
        ret = errno2rfec(errno);
    }
    ruFree(dirPath);
    ruFree(wpath);
    return ret;
}

RUAPI rusize ruFileSize(trans_chars filePath, int32_t* code) {
    wchar_t* wfilename = getWPath(filePath);
    rusize out = 0;
    if (!wfilename) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, out);

    ruZeroedStruct(struct __stat64, buf);
    if (0 == _wstat64(wfilename, &buf)) {
        out = (rusize)buf.st_size;
        if (code) *code = RUE_OK;
    } else {
        if (code) *code = errno2rfec(errno);
    }
    ruFree (wfilename);
    return out;
}

RUAPI sec_t ruFileUtcTime(trans_chars filePath, int32_t* code) {
    wchar_t* wfilename = getWPath(filePath);
    sec_t out = 0;
    if (!wfilename) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, out);

    ruZeroedStruct(struct __stat64, buf);
    if (0 == _wstat64(wfilename, &buf)) {
        out = (sec_t)buf.st_mtime;
        if (code) *code = RUE_OK;
    } else {
        if (code) *code = errno2rfec(errno);
    }
    ruFree (wfilename);
    return out;
}

RUAPI int32_t ruFileSetUtcTime(trans_chars filePath, sec_t date) {
    if (!filePath) return RUE_PARAMETER_NOT_SET;
    if (date < 0) return RUE_INVALID_PARAMETER;

    wchar_t* wfilename = getWPath(filePath);
    struct _utimbuf tb;
    tb.actime = date;
    tb.modtime = date;
    int32_t ret = RUE_OK;
    if(_wutime(wfilename, &tb)) {
        ret = errno2rfec(errno);
    }
    ruFree (wfilename);
    return ret;
}

#else

//#ifndef _WIN32 /* mingw doesn't do symlinks */
RUAPI bool ruIsSymlink(const char* filename) {
    ruClearError();
    struct stat s;
    if (lstat(filename, &s) == 0 && S_ISLNK (s.st_mode)) return true;
    return false;
}
//#endif

RUAPI bool ruFileExists(const char* filename) {
    ruClearError();
    struct stat s;
    if (stat(filename, &s) == 0) return true;
    return false;
}

RUAPI bool ruIsFile(const char* filename) {
    ruClearError();
    struct stat s;
    if (stat(filename, &s) == 0 && S_ISREG (s.st_mode)) return true;
    return false;
}

RUAPI bool ruIsDir(const char* filename) {
    ruClearError();
    struct stat s;
    if (stat (filename, &s) == 0 && S_ISDIR (s.st_mode)) return true;
    return false;
}

RUAPI bool ruIsExecutable(const char* filename) {
    ruClearError();
    struct stat s;
    if (stat(filename, &s) != 0) return false;
    if((s.st_mode & S_IXOTH) || (s.st_mode & S_IXUSR) || (s.st_mode & S_IXGRP)) {
        return true;
    }
    return false;
}

RUAPI int ruOpen(const char *filepath, int flags, int mode, int32_t* code) {
    ruClearError();
    if (!filepath) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, 0);
    if (flags == O_RDONLY) {
        int fd = open(filepath, flags);
        if (fd < 0) {
            int32_t ret = errno2rfec(errno);
            ruRetWithCode(code, ret, fd);
        }
        ruRetWithCode(code, RUE_OK, fd);
    }
    if (!(flags & O_WRONLY) && !(flags & O_RDWR)) {
        ruRetWithCode(code, RUE_INVALID_PARAMETER, 0);
    }
    if (!mode) ruRetWithCode(code, RUE_INVALID_PARAMETER, 0);
#ifdef ITS_OSX
    alloc_chars nfdpath = ruStrToNfd(filepath);
    int fd = open(nfdpath, flags, mode);
    ruFree(nfdpath);
#else
    int fd = open(filepath, flags, mode);
#endif
    if (fd < 0) {
        int32_t ret = errno2rfec(errno);
        ruRetWithCode(code, ret, fd);
    }
    ruRetWithCode(code, RUE_OK, fd);
}

RUAPI FILE* ruFOpen(const char *filepath, const char *mode, int32_t* code) {
    ruClearError();
    if (!filepath) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, 0);
    if (!mode) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, 0);
#ifdef ITS_OSX
    alloc_chars nfdpath = ruStrToNfd(filepath);
    FILE* fd = fopen(nfdpath, mode);
    ruFree(nfdpath);
#else
    FILE* fd = fopen(filepath, mode);
#endif
    if (!fd) {
        int32_t ret = errno2rfec(errno);
        ruRetWithCode(code, ret, fd);
    }
    ruRetWithCode(code, RUE_OK, fd);
}

RUAPI int ruDiskFree(trans_chars path, uint64_t* total, uint64_t* avail) {
#ifndef __EMSCRIPTEN__
    if (!path || (!total && !avail)) return RUE_PARAMETER_NOT_SET;
#ifdef __linux__
    ruZeroedStruct(struct statfs64, sf);
    int ret = statfs64(path, &sf);
#else
    // darwin
    ruZeroedStruct(struct statvfs, sf);
    int ret = statvfs(path, &sf);
#endif
    if (!ret) {
        if (total) *total = sf.f_blocks * sf.f_frsize;
        if (avail) *avail = sf.f_bavail * sf.f_frsize;
        return  RUE_OK;
    }
    if (total) *total = 0;
    if (avail) *avail = 0;
#endif
    return RUE_CANT_OPEN_FILE;
}

RUAPI rusize ruFileSize(trans_chars filePath, int32_t* code) {
    ruStat_t st;
    int32_t ret = ruStat(filePath, &st);
    if(ret != RUE_OK) ruRetWithCode(code, ret, 0);
    ruRetWithCode(code, ret, st.st_size);
}

RUAPI sec_t ruFileUtcTime(trans_chars filePath, int32_t* code) {
    ruStat_t st;
    int32_t ret = ruStat(filePath, &st);
    if(ret != RUE_OK) ruRetWithCode(code, ret, 0);
    ruRetWithCode(code, ret, st.st_mtime);
}

RUAPI int32_t ruFileSetUtcTime(trans_chars filePath, sec_t date) {
    if (!filePath) return RUE_PARAMETER_NOT_SET;
    if (date < 0) return RUE_INVALID_PARAMETER;
    struct utimbuf tb;
    tb.actime = date;
    tb.modtime = date;
    if(utime(filePath, &tb)) {
        return errno2rfec(errno);
    }
    return RUE_OK;
}

#endif

int32_t errno2rfec(int err) {
    // from Linux errno-base.h
    //#define	EPERM		 1	/* Operation not permitted */
    //#define	ENOENT		 2	/* No such file or directory */
    //#define	ESRCH		 3	/* No such process */
    //#define	EINTR		 4	/* Interrupted system call */
    //#define	EIO		     5	/* I/O error */
    //#define	ENXIO		 6	/* No such device or address */
    //#define	E2BIG		 7	/* Argument list too long */
    //#define	ENOEXEC		 8	/* Exec format error */
    //#define	EBADF		 9	/* Bad file number */
    //#define	ECHILD		10	/* No child processes */
    //#define	EAGAIN		11	/* Try again */
    //#define	ENOMEM		12	/* Out of memory */
    //#define	EACCES		13	/* Permission denied */
    //#define	EFAULT		14	/* Bad address */
    //#define	ENOTBLK		15	/* Block device required */
    //#define	EBUSY		16	/* Device or resource busy */
    //#define	EEXIST		17	/* File exists */
    //#define	EXDEV		18	/* Cross-device link */
    //#define	ENODEV		19	/* No such device */
    //#define	ENOTDIR		20	/* Not a directory */
    //#define	EISDIR		21	/* Is a directory */
    //#define	EINVAL		22	/* Invalid argument */
    //#define	ENFILE		23	/* File table overflow */
    //#define	EMFILE		24	/* Too many open files */
    //#define	ENOTTY		25	/* Not a typewriter */
    //#define	ETXTBSY		26	/* Text file busy */
    //#define	EFBIG		27	/* File too large */
    //#define	ENOSPC		28	/* No space left on device */
    //#define	ESPIPE		29	/* Illegal seek */
    //#define	EROFS		30	/* Read-only file system */
    //#define	EMLINK		31	/* Too many links */
    //#define	EPIPE		32	/* Broken pipe */
    //#define	EDOM		33	/* Math argument out of domain of func */
    //#define	ERANGE		34	/* Math result not representable */
    switch (err){
        case ENOENT:
        case ENOTDIR:
            return RUE_FILE_NOT_FOUND;
        case EINVAL:
            return RUE_INVALID_PARAMETER;
        case EACCES:
        case EPERM:
        case EMFILE:
            return RUE_CANT_OPEN_FILE;
        case EROFS:
            return RUE_CANT_WRITE;
        case EEXIST:
            return RUE_FILE_EXISTS;
        default:
            return RUE_GENERAL;
    }
}

RUAPI int ruStat(const char*filepath, ruStat_t *dest) {
    ruClearError();
    if (!filepath || !dest) return RUE_PARAMETER_NOT_SET;
    if (!stat(filepath, dest)) return RUE_OK;
    if(errno == ENOENT) return RUE_FILE_NOT_FOUND;
    ruSetError("failed accessing '%s' errno: %d - %s",
               filepath, errno, strerror(errno));
    return RUE_CANT_OPEN_FILE;
}

RUAPI int ruOpenTmp(char *pathTemplate, int flags, int mode, int32_t *code) {
    ruClearError();
    static const char pool[] = "abcdefghijklmnopqrstuvwxyz";
    static const int poolSize = sizeof (pool) - 1;
    static int threadcounter = 0;

    if (!pathTemplate) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, 0);
    if (!(flags & O_WRONLY) && !(flags & O_RDWR)) ruRetWithCode(code, RUE_INVALID_PARAMETER, 0);
    if (!mode) ruRetWithCode(code, RUE_INVALID_PARAMETER, 0);

    ruTimeVal tv;
    ruGetTimeVal(&tv);

    char *xl8 = (char*) ruLastSubStr(pathTemplate, "^^^");
    if (!xl8) ruRetWithCode(code, RUE_INVALID_PARAMETER, 0);

    long value = (long)((tv.usec ^ tv.sec ) + threadcounter++);
    int ret = RUE_OK, fd = -1;
    for (int tries = 0; tries < 100; value += 123, tries++) {
        long v = value;

        xl8[0] = pool[v % poolSize];
        v /= poolSize;
        xl8[1] = pool[v % poolSize];
        v /= poolSize;
        xl8[2] = pool[v % poolSize];

        fd = ruOpen((const char*)pathTemplate, flags, mode, &ret);
        int myerr = errno;
        if (fd >= 0) ruRetWithCode(code, ret, fd);

        if (myerr != EEXIST) ruRetWithCode(code, ret, fd);
    }
    ruRetWithCode(code, ret, fd);
}

RUAPI int32_t ruFileSetContents(trans_chars filename, trans_chars contents,
                              rusize length) {
    ruClearError();
    if (!filename) return RUE_PARAMETER_NOT_SET;

    int32_t ret = RUE_OK;
    if (length == RU_SIZE_AUTO && contents) length = strlen (contents);
    char *tmpName = ruDupPrintf("%s.^^^", filename);
    int oh = ruOpenTmp(tmpName, O_CREAT | O_RDWR, 0666, &ret);
    do {
        if (ret != RUE_OK) {
            ruSetError("could not open '%s' ec: %d", tmpName, ret);
            break;
        }
        if (!contents) length = 0;
        while (length > 0) {
            rusize_s s = ruWrite(oh, contents, length);

            if (s < 0) {
                int saved_errno = errno;
                if (saved_errno == EINTR) continue;

                ruSetError("Failed to write file '%s' errno: %d - %s",
                           tmpName, saved_errno, strerror(saved_errno));
                ret = RUE_CANT_WRITE;
                break;
            }
            contents += s;
            length -= s;
        }

    } while(0);

    // clean up
    if (oh >= 0) {
        close(oh);
#ifdef ITS_OSX
        alloc_chars nfdpath = ruStrToNfd(filename);
        ret = ruFileRename(tmpName, nfdpath);
        ruFree(nfdpath);
#else
        ret = ruFileRename(tmpName, filename);
#endif
        if (ret != RUE_OK) {
            ruFileRemove(tmpName);
        }
    }

    ruFree(tmpName);
    return ret;
}

RUAPI int ruFileGetContents(trans_chars filename, alloc_chars* contents, rusize* length) {
    ruClearError();
    if (!filename || !contents) return RUE_PARAMETER_NOT_SET;
    *contents = NULL;
    if (length) *length = 0;

    if (!ruFileExists(filename)) {
        return RUE_FILE_NOT_FOUND;
    }

    if (!ruIsFile(filename)) {
        ruSetError("File '%s' is not a regular file to read");
        return RUE_INVALID_PARAMETER;
    }

    int fd = open(filename, O_RDONLY);

    if (fd < 0) {
        ruSetError("Failed to open file '%s' errno: %d - %s",
                   filename, errno, strerror(errno));
        return RUE_CANT_OPEN_FILE;
    }

    char *buf;
    rusize bytes_read = 0, size, alloc_size;
    struct stat stat_buf;
    if (fstat (fd, &stat_buf) < 0) {
        ruSetError("Failed to get attributes of file '%s': errno: %d - %s",
                 filename, errno, strerror(errno));
        close(fd);
        return RUE_CANT_OPEN_FILE;
    }

    int ret = RUE_OK;
    do {
        size = stat_buf.st_size;

        alloc_size = size + 1;
        buf = malloc(alloc_size);

        if (buf == NULL) {
            ruSetError("Could not allocate %lu bytes to read file “%s”",
                     alloc_size, filename);
            ret = RUE_OUT_OF_MEMORY;
            break;
        }

        uint32_t chunk = ruIntChunk;
        while (bytes_read < size) {
            // yep ,M$
            rusize left = size - bytes_read;
            if (left < (rusize)chunk) chunk = (uint32_t)left;
            int32_t rc = read(fd, buf + bytes_read, chunk);
            if (rc == 0)  break; // done
            if (rc < 0) { // no write
                if (errno == EINTR) continue; // next chunk
                ruSetError("Failed to read from file '%s' errno: %d - %s",
                        filename, errno, strerror(errno));
                ret = RUE_CANT_OPEN_FILE;
                break;
            }
            bytes_read += rc; // written
        }
    } while(0);

    if(ret == RUE_OK) {
        buf[bytes_read] = '\0';
        if(length) *length = bytes_read;

        *contents = buf;
        buf = NULL;
    }
    ruFree(buf);
    close(fd);

    return ret;
}

#define CP_BUF_SZ 4096
RU_THREAD_LOCAL char cpbuf[CP_BUF_SZ];

RUAPI int ruFileCopy(trans_chars srcpath, trans_chars destpath) {
    ruClearError();
    if (!srcpath || !destpath) return RUE_PARAMETER_NOT_SET;

    int32_t ret = RUE_OK;
    if (!ruFileExists(srcpath)) {
        return RUE_FILE_NOT_FOUND;
    }

    if (!ruIsFile(srcpath)) {
        ruSetError("File '%s' is not a regular file to read");
        return RUE_INVALID_PARAMETER;
    }
    int ih = open(srcpath, O_RDONLY);
    if (ih < 0) {
        ruSetError("Failed to open file '%s' errno: %d - %s",
                   srcpath, errno, strerror(errno));
        return RUE_CANT_OPEN_FILE;
    }

    alloc_chars tmpName = ruDupPrintf("%s.^^^", destpath);
    ret = RUE_CANT_OPEN_FILE;
    int oh = ruOpenTmp(tmpName, O_CREAT | O_RDWR, 0666, &ret);
    while (oh >= 0) {
        // copy the data
        int in = read(ih, cpbuf, CP_BUF_SZ);
        if (in <= 0) break;
        if (ruWrite(oh, cpbuf, in) <= 0) break;
    }

    if (ih) close(ih);
    if (oh >= 0) {
        close(oh);
        ret = ruFileRename(tmpName, destpath);
        if (ret != RUE_OK) {
            ruFileRemove(tmpName);
        }
    }
    ruFree(tmpName);

    return ret;
}

static int fileRename(const char* oldName, const char* newName, bool force) {
    ruClearError();
    int ret = RUE_OK;
#ifdef _WIN32
    char *oldNameFixed = fixPath(oldName);
    char *newNameFixed = fixPath(newName);
    wchar_t *old = charToUni(oldNameFixed);
    wchar_t *new = charToUni(newNameFixed);
    DWORD flags = force? MOVEFILE_REPLACE_EXISTING : 0;
    if (!MoveFileExW (old, new, flags)) {
        DWORD err = GetLastError();
        ruSetError("Failed to rename file '%s' to '%s' errno: %d",
                 oldName, newName, err);
        ret = RUE_CANT_WRITE;
    }
    ruFree(oldNameFixed);
    ruFree(newNameFixed);
    ruFree(old);
    ruFree(new);
#else
#if defined(__linux__) || defined(__EMSCRIPTEN__)
    // not sure about android
#ifdef RENAME_NOREPLACE
    uint flags = force? 0 : RENAME_NOREPLACE;
    if(renameat2(AT_FDCWD, oldName,
                 AT_FDCWD, newName, flags)) {
#else
    // this was needed on Centos 7 due to the old kernel
    if(rename(oldName, newName)) {
#endif
#else
    // means darwin ATM nopt sure about iOS
    uint flags = force? 0 : RENAME_EXCL;
#ifdef ITS_OSX
    alloc_chars nfdpath = ruStrToNfd(newName);
    newName = nfdpath;
#endif
    if (renamex_np(oldName, newName, flags)) {
#endif
        ruSetError("Failed to rename file '%s' to '%s' errno: %d - %s",
                   oldName, newName, errno, strerror(errno));
        ret = RUE_CANT_WRITE;
    }
#endif
#ifdef ITS_OSX
    ruFree(nfdpath);
#endif
    return ret;
}

RUAPI int ruFileRename(const char* oldName, const char* newName) {
    return fileRename(oldName, newName, true);
}

RUAPI int ruFileTryRename(const char* oldName, const char* newName) {
    return fileRename(oldName, newName, false);
}

rusize_s ruWrite(int oh, const void* contents, rusize length) {
#ifdef _WIN32
    if (length > (rusize)ruIoChunk) {
        uint32_t sz = ruIoChunk;
        length = sz; // that's all we'll write
        const char *p = contents;
        while (length) {
            int wrt = _write(oh, p, sz);
            if (wrt < 0) {
                return wrt;
            }
            p += sz;
            length -= sz;
            if ((rusize)sz > length) sz = (uint32_t) length;
        }
        return (rusize_s) length;
    } else {
        return (rusize_s)_write(oh, contents, (unsigned int) length);
    }
#else
    return write(oh, contents, length);
#endif
}

static int remover(trans_chars fullPath, bool isDir, ptr ctx) {
    if (strlen(fullPath) == 0) return RUE_INVALID_PARAMETER;
    if (strcmp("/", fullPath) == 0) return RUE_INVALID_PARAMETER;
    if (!ruFileExists(fullPath)) return RUE_OK;
    if(ruFileRemove(fullPath)) {
        ruSetError("failed removing '%s' error: %d - %s",
                 fullPath, errno, strerror(errno));
        return RUE_GENERAL;
    }
    return RUE_OK;
}

static int32_t folderWalk(trans_chars folder, uint32_t flags,
                          entryFilter filter, entryMgr actor, ptr ctx) {
    // sanity checks
    if (!folder) return RUE_PARAMETER_NOT_SET;
    int32_t ret = RUE_OK;
    if (!ruFileExists(folder)) return ret;
    bool isFolder = ruIsDir(folder);

    alloc_chars dirname = NULL;
    char* basename = NULL;
    if (filter) {
        dirname = ruDirName(folder);
#ifdef ITS_OSX
        basename = ruStrToNfd(ruBaseName(folder));
#else
        basename = (char*)ruBaseName(folder);
#endif
    }

    if (isFolder) {
        if (flags & RU_WALK_FOLDER_FIRST) {
            if (!filter || !filter(dirname, basename, isFolder, ctx)) {
                if (actor) {
                    ret = actor(folder, isFolder, ctx);
                    if (ret != RUE_OK) goto cleanup;
                }
            }
        }
#ifdef _WIN32
        WIN32_FIND_DATAW ffd;
        WCHAR szDir[MAX_PATH];
        HANDLE hFind = INVALID_HANDLE_VALUE;

        // Check that the input path plus 3 is not longer than MAX_PATH.
        // Three characters are for the "\*" plus NULL appended below.
        if (strlen(folder) > (MAX_PATH - 3)) {
            ruSetError("given folder name '%s' is longer than allowed %d bytes",
                    folder, MAX_PATH);
            ret = RUE_INVALID_PARAMETER;
            goto cleanup;
        }

        WCHAR *fld = charToUni(folder);
        // Prepare string for use with FindFile functions.  First, copy the
        // string to a buffer, then append '\*' to the directory name.
        _snwprintf(szDir, MAX_PATH, L"%s\\*", fld);
        ruFree(fld);

        // Find the first file in the directory.
        hFind = FindFirstFileW(szDir, &ffd);
        if (INVALID_HANDLE_VALUE == hFind) {
            ruSetError("failed opening '%s' windows error: %d",
                    szDir, GetLastError());
            ret = RUE_FILE_NOT_FOUND;
            goto cleanup;
        }

        char *path = NULL, *fileName = NULL;
        // List all the files in the directory with some info about them.
        do {
            ruFree(fileName);
            fileName = uniToChar(ffd.cFileName);
            if (ruStrCmp("..", fileName) == 0 ||
                ruStrCmp(".", fileName) == 0)
                continue;

            ruFree(path);
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                path = ruDupPrintf("%s%s%c", folder, fileName, RU_SLASH);
            } else {
                path = ruDupPrintf("%s%s", folder, fileName);
            }
            if ((flags & RU_WALK_NO_RECURSE) ||
                !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                if (actor) {
                    ret = actor(path, false, ctx);
                    if (ret != RUE_OK) break;
                }
                continue;
            }
            ret = folderWalk(path, flags, filter, actor, ctx);
            if (ret != RUE_OK) break;

        } while (FindNextFileW(hFind, &ffd) != 0);

        ruFree(fileName);
        ruFree(path);
        FindClose(hFind);

#else
        DIR *d = opendir(folder);
        if (!d) {
            ruSetError("failed opening '%s' errno: %d - %s",
                       folder, errno, strerror(errno));
            ret = RUE_FILE_NOT_FOUND;
            goto cleanup;
        }

        struct dirent *dir;
        char* path = NULL;
        char* name = NULL;

        while ((dir = readdir(d)) != NULL) {
            if (ruStrEquals("..", dir->d_name) ||
                ruStrEquals(".", dir->d_name)) continue;
#ifdef ITS_OSX
            ruReplace(name, ruStrToNfd(dir->d_name));
#else
            name = dir->d_name;
#endif

            if (filter) {
                if (filter(folder, name,
                           dir->d_type == DT_DIR, ctx)) {
                    continue;
                }
            }
            ruFree(path);
            if (dir->d_type == DT_DIR) {
                path = ruDupPrintf("%s%s%c", folder, name, RU_SLASH);
            } else {
                path = ruDupPrintf("%s%s", folder, name);
            }
            if ((flags & RU_WALK_NO_RECURSE) || dir->d_type != DT_DIR) {
                if (actor) {
                    ret = actor(path, false, ctx);
                    if (ret != RUE_OK) break;
                }
                continue;
            }
            ret = folderWalk(path, flags, filter, actor, ctx);
            if (ret != RUE_OK) break;
        }
#ifdef ITS_OSX
        ruFree(name);
#endif
        ruFree(path);
        closedir(d);
#endif
    }
    if ((flags & RU_WALK_FOLDER_LAST) || !isFolder) {
        if (ret == RUE_OK) {
            if (!filter || !filter(dirname, basename, isFolder, ctx)) {
                if (actor) ret = actor(folder, isFolder, ctx);
            }
        }
    }

cleanup:
#ifdef ITS_OSX
    ruFree(basename);
#endif
    ruFree(dirname);
    return ret;
}

/**
 * Fixes path slashes to be proper and to contain a trailing slash. Result to be
 * freed will be returned and given file pointer will be updated to returned path
 * as needed.
 *
 * @param filePath path to examine, will be set to returned path if it was created
 * @return new created path to free after use or NULL
 */
alloc_chars fixSlashes(perm_chars* filePath) {
    alloc_chars path = NULL;
#ifdef RUMS
    path = fixPath(*filePath);
    if (!ruStrEndsWith(path, "\\", NULL)) {
        alloc_chars p2 = ruDupPrintf("%s\\", path);
        ruFree(path);
        path = p2;
    }
    *filePath = path;
#else
    if (!ruStrEndsWith(*filePath, "/", NULL)) {
        path = ruDupPrintf("%s/", *filePath);
        *filePath = path;
    }
#ifdef ITS_OSX
    alloc_chars nfdpath = ruStrToNfd(*filePath);
    ruReplace(path, nfdpath);
    *filePath = path;
#endif
#endif
    return path;
}

RUAPI int32_t ruFilteredFolderWalk(trans_chars folder, uint32_t flags,
                                   entryFilter filter, entryMgr actor, ptr ctx) {
    alloc_chars path = fixSlashes(&folder);
    int32_t ret = folderWalk(folder, flags, filter, actor, ctx);
    ruFree(path);
    return ret;
}

RUAPI int32_t ruFolderWalk(trans_chars folder, uint32_t flags, entryMgr actor, ptr ctx) {
    alloc_chars path = fixSlashes(&folder);
    int32_t ret = folderWalk(folder, flags, NULL, actor, ctx);
    ruFree(path);
    return ret;
}

static int32_t cntLst(trans_chars fullPath, bool isFolder, ptr o) {
    ru_int* cnt = (ru_int*)o;
    (*cnt)++;
    return RUE_OK;
}

RUAPI ru_int ruFolderEntries(trans_chars folder) {
    ru_int cnt = 0;
    alloc_chars path = fixSlashes(&folder);
    folderWalk(folder, RU_WALK_NO_RECURSE | RU_WALK_FOLDER_FIRST,
               NULL, cntLst, &cnt);
    ruFree(path);
    return cnt;
}

RUAPI int ruFolderRemove(trans_chars folder) {
    ruClearError();
    if (!folder) return RUE_PARAMETER_NOT_SET;
    if (strlen(folder) == 0) return RUE_INVALID_PARAMETER;
    if (strcmp("/", folder) == 0) return RUE_INVALID_PARAMETER;
#ifdef _WIN32
    if (strcmp("\\", folder) == 0) return RUE_INVALID_PARAMETER;
#endif
    return ruFolderWalk(folder, RU_WALK_FOLDER_LAST, remover, NULL);
}

RUAPI int ruFileRemove(const char* filename) {
    ruClearError();
#ifdef _WIN32
    wchar_t *wpath = getWPath(filename);
    int ret = 0, waDir = 0;
    if (ruWIsDir(wpath)) {
        ret = _wrmdir(wpath);
        waDir = 1;
    }
    if (!waDir) {
        ret = _wunlink(wpath);
    }
    ruFree (wpath);
    return ret;
#else
    if (!ruIsSymlink(filename) && ruIsDir(filename)) {
        return rmdir(filename);
    }
    return unlink(filename);
#endif
}

char* getDirNameTerminator(trans_chars filePath) {
    if (!filePath) return NULL;
    char *pfile = (char*)filePath + strlen(filePath)-1;
    for (; pfile >= filePath; pfile--) {
#ifdef _WIN32
        if (*pfile == '/' || *pfile == '\\' ) {
#else
        if (*pfile == '/') {
#endif
            return pfile;
        }
    }
    // at root level
    return NULL;
}

int ruMkdirRecurse(char *pathname, int mode, bool deep) {
    if (!pathname) return RUE_PARAMETER_NOT_SET;
    if (ruIsDir(pathname)) return RUE_OK; // already there
    if (ruFileExists(pathname)) return RUE_FILE_EXISTS; // there but not folder

    char *ptr = getDirNameTerminator(pathname);
    if (!ptr) return RUE_GENERAL;
    char oldVal = *ptr; // remember that character
    *ptr = '\0'; // set the terminator
    if (strlen(pathname) && !ruIsDir(pathname)) {
        if (deep) {
            int ret = ruMkdirRecurse(pathname, mode, deep);
            if (ret != RUE_OK) return ret;
        } else {
            return RUE_FILE_NOT_FOUND;
        }
    }
    *ptr = oldVal; // remove the terminator again
#ifdef _WIN32
    wchar_t *wpath = getWPath(pathname);
    if (_wmkdir(wpath)) {
        ruFree(wpath);
        return RUE_CANT_WRITE;
    }
    ruFree(wpath);
#else
    if (mkdir(pathname, mode)) {
        return RUE_CANT_WRITE;
    }
#endif
    return RUE_OK;
}

RUAPI int ruMkdir(const char *pathname, int mode, bool deep) {
    ruClearError();
    if (!pathname || !strlen(pathname)) return RUE_PARAMETER_NOT_SET;
    if (ruIsDir(pathname)) return RUE_OK; // already there
#ifndef _WIN32
    if (!mode) return RUE_INVALID_PARAMETER;
#endif
    int ret;
#ifdef ITS_OSX
    char *path = ruStrToNfd(pathname);
#else
    char *path = ruStrDup(pathname);
#endif
    do {
        char *p = path + strlen(path)-1;
#ifdef _WIN32
        if (*p == '\\' || *p == '/') *p = '\0';
        p = path;
        /* Recognize drive letter on native Windows */
        char c = p[0] | 0x20;
        if (c >= 'a' && c <= 'z' && p[1] == ':' &&
            (p[2] == '\\' || p[2] == '/')) {
            p += 2;
        } else if ((p[0] == '\\' && p[1] == '\\') ||
                   (p[0] == '/' && p[1] == '/')) {
            ret = RUE_INVALID_PARAMETER;
            break;
        }
#else
        if (*p == '/') *p = '\0';
        p = path;
#endif
        ret = ruMkdirRecurse(p, mode, deep);
    } while(0);

    ruFree(path);
    return ret;
}

RUAPI alloc_chars ruDirName(trans_chars filePath) {
    ruClearError();
    if (!filePath) return NULL;
    char *fileTerm = getDirNameTerminator(filePath);
    if (!fileTerm) return NULL;
    rusize len = fileTerm - filePath;
    return ruStrNDup(filePath, len);
}
// 01/
RUAPI perm_chars ruBaseName(perm_chars filePath) {
    ruClearError();
    if (!filePath) return NULL;
    char *pfile = (char*)filePath + strlen(filePath)-1;
    for (; pfile >= filePath; pfile--) {
#ifdef _WIN32
        if (*pfile == '/' || *pfile == '\\' ) {
#else
        if (*pfile == '/') {
#endif
            pfile++;
            break;
        }
    }
    if (pfile < filePath) return (char*)filePath;
    return pfile;
}

RUAPI perm_chars ruFileExtension(perm_chars filePath) {
    ruClearError();
    if (!filePath) return NULL;
    perm_chars pfile = filePath + strlen(filePath)-1;
    for (; pfile >= filePath; pfile--) {
#ifdef _WIN32
        if (*pfile == '/' || *pfile == '\\' ) {
#else
        if (*pfile == '/') {
#endif
            // hit folder boundary
            return NULL;
        }
        if (*pfile == '.') {
            // we do not want dot files al la .foo
            if (pfile == filePath) return NULL;
            // or /.foo
            if (*(pfile - 1) == '/') return NULL;
#ifdef _WIN32
            // or c:\\.foo
            if (*(pfile - 1) == '\\' ) return NULL;
#endif
            pfile++;
            break;
        }
    }
    // dot at the end
    if (!*pfile) return NULL;
    // no dot at all
    if (pfile == filePath) return NULL;
    return pfile;
}

RUAPI alloc_chars ruFullPath(trans_chars filePath) {
    ruClearError();
    alloc_chars res = NULL;
#ifdef _WIN32
    char *fixedPath = NULL;
    if (filePath) {
        fixedPath = fixPath(filePath);
        if (*fixedPath == '/' || *fixedPath == '\\' ||
            *(fixedPath + 1) == ':') {
            // path is already absolute
            return fixedPath;
        }
    }
    char* dir = NULL;
    UChar fullPath[_MAX_PATH];
    if (_wgetcwd(fullPath, _MAX_PATH)) {
        dir = getMbPath(fullPath);
        if (fixedPath) {
            res = ruDupPrintf("%s%c%s", dir, RU_SLASH, fixedPath);
        } else {
            res = dir;
            dir = NULL;
        }
    } else {
        ruSetError("no current directory");
    }
    ruFree(fixedPath);
    ruFree(dir);
#else
    if (filePath && *filePath == '/') {
        // path is already absolute
        return ruStrDup(filePath);
    }
    char fullPath[MAXPATHLEN];
    fullPath[0] = '\0';
    if (getcwd(fullPath, MAXPATHLEN) && fullPath[0] == '/') {
        if (filePath) {
            res = ruDupPrintf("%s%c%s", fullPath, RU_SLASH, filePath);
        } else {
            res = ruStrDup(fullPath);
        }
    } else {
        ruSetError("invalid current directory '%s'", fullPath);
    }
#endif
    return res;
}

RUAPI alloc_chars ruPathJoin(trans_chars base, trans_chars file) {
    if (!base && !file) return NULL;
    if (!file) return ruStrDup(base);
    if (!base) return ruStrDup(file);
    if (ruStrEndsWith(base, RU_SLASH_S, NULL)) {
        return ruDupPrintf("%s%s", base, file);
    }
    return ruDupPrintf("%s%s%s", base, RU_SLASH_S, file);
}

RUAPI alloc_chars ruPathMultiJoin(int parts, ...) {
    if (parts < 1) return NULL;
    va_list args;
    va_start (args, parts);
    ruString str = NULL;
    for (int i = 0; i < parts; i++) {
        trans_chars piece = va_arg(args, trans_chars);
        if (str && ruStrStartsWith(piece, RU_SLASH_S, NULL)) piece++;
        rusize len = strlen(piece);
        if (ruStrEndsWith(piece, RU_SLASH_S, NULL)) {
            len--;
        }
        if (!str) {
            str = ruStringNewn(NULL, len * parts);
            ruStringAppendn(str, piece, len);
        } else {
            ruStringAppend(str, RU_SLASH_S);
            ruStringAppendn(str, piece, len);
        }
    }
    va_end (args);
    alloc_chars out = ruStringGetCString(str);
    ruStringFree(str, true);
    return out;
}
