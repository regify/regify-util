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
/**
 * \defgroup io File I/O
 * \brief This section contains abstracted file system and I/O utilities.
 *
 * @{
 */

#ifndef REGIFY_UTIL_IO_H
#define REGIFY_UTIL_IO_H
#include <sys/types.h>
#include <sys/stat.h>

/* Only need to export C interface if used by C++ source code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Checks whether a file or folder exists.
 * @param filename Name of file, folder or other filesystem entity.
 * @return Return true if the path exists else false.
 */
RUAPI bool ruFileExists(trans_chars filename);

/**
 * Returns true if the given path exists and is a regular file.
 * On Unix if the underlying path is a symlink it is dereferenced and the status
 * of the target is returned.
 * @param filename File path to test
 * @return Returns true if it is a regular file.
 */
RUAPI bool ruIsFile(trans_chars filename);

/**
 * Returns true if the given path exists and is a folder.
 * On Unix if the underlying path is a symlink it is dereferenced and the status
 * of the target is returned.
 * @param filename File path to test
 * @return Returns true if it is a folder.
 */
RUAPI bool ruIsDir(trans_chars filename);

/**
 * Returns true if the given path exists and is executable.
 * On Unix if the underlying path is a symlink it is dereferenced and the status
 * of the target is returned.
 * On Unix the file permissions are test for -x.
 * On Windows the file extension is check for .com, .cmd, .bat, .exe and
 * whatever extension is set in the PATHEXT environment variable.
 * @param filename File path to test
 * @return Returns true if it is executable.
 */
RUAPI bool ruIsExecutable(trans_chars filename);

#ifndef _WIN32
/**
 * Returns true if the given path exists and is a symbolic link (Unix only).
 * @param filename File path to test
 * @return Returns true if it is a symlink.
 */
RUAPI bool ruIsSymlink(trans_chars filename);
#endif

/**
 * Returns the total and avail size of the partition where path resides on.
 *
 * @param path Path whose partition to examine.
 * @param total Optional, Where total partition size will be stored in bytes
 * @param avail Optional, Where left over user partition size will be stored in bytes.
 * @return RUE_OK on success else RUE_CANT_OPEN_FILE. Then check errno for details.
 */
RUAPI int32_t ruDiskFree(trans_chars path, uint64_t* total, uint64_t* avail);

/**
 * An abstracted version of struct stat.
 */
typedef struct stat ruStat_t;

/**
 * Populates the given \ref ruStat_t with the data of the filepath if it exists.
 * @param filepath File path to test
 * @param dest Pointer to \ref ruStat_t to populate.
 * @return Returns
 *        \ref RUE_OK on success else
 *        \ref RUE_PARAMETER_NOT_SET if filename was NULL or
 *        \ref RUE_FILE_NOT_FOUND if file didn't exists or
 *        \ref RUE_CANT_OPEN_FILE if it was another error.
 *        Check errno or call \ref ruLastError for details.
 */
RUAPI int32_t ruStat(trans_chars filepath, ruStat_t *dest);

/**
 * \brief Return the size of the given file if it exists
 * @param filePath File path to get size of
 * @param code Where the status of the operation will be stored.
 *        \ref RUE_OK on success else
 *        \ref RUE_PARAMETER_NOT_SET if filename was NULL or
 *        \ref RUE_FILE_NOT_FOUND if file didn't exists or
 *        \ref RUE_CANT_OPEN_FILE if it was another error.
 *        Check errno for details.
 * @return file size or 0 on error
 */
RUAPI rusize ruFileSize(trans_chars filePath, int32_t* code);

/**
 * \brief Return the UTC modification date of the given file if it exists
 * @param filePath File path to get date of
 * @param code Where the status of the operation will be stored.
 *        \ref RUE_OK on success else
 *        \ref RUE_PARAMETER_NOT_SET if filename was NULL or
 *        \ref RUE_FILE_NOT_FOUND if file didn't exists or
 *        \ref RUE_CANT_OPEN_FILE if it was another error.
 *        Check errno for details.
 * @return UTC modification date or 0 on error
 */
RUAPI sec_t ruFileUtcTime(trans_chars filePath, int32_t* code);

/**
 * \brief Set UTC access and mod time of given filePath to given date
 * @param filePath file to set time on
 * @param date UTC time stamp to set file time to
 * @return Parameter validation error or \ref RUE_OK if parameters are good.
 */
RUAPI int32_t ruFileSetUtcTime(trans_chars filePath, sec_t date);

/**
 * Abstracted version of the Posix open call.
 * @param filepath Path to the file to open
 * @param flags File access flags such as O_RDONLY, O_WRONLY or O_RDWR.
 * @param mode Desired file permissions if this file is to be created. Ignored
 *             on Windows.
 * @param code Parameter validation error or \ref RUE_OK if parameters are good.
 * @return A valid file handle or -1 in case of error. Then check errno.
 */
RUAPI int ruOpen(trans_chars filepath, int flags, int mode, int32_t* code);

/**
 * Abstracted version of the Posix fopen call.
 * @param filepath Path to the file to open
 * @param mode File access mode such as r, r+, w, w+, a or a+.
 * @param code Parameter validation error or \ref RUE_OK if parameters are good.
 * @return A valid file handle or -1 in case of error. Then check errno.
 */
RUAPI FILE* ruFOpen(trans_chars filepath, trans_chars mode, int32_t* code);

/**
 * Creates and returns the file handle to a new temporary file.
 * @param pathTemplate The filepath to where the file should be written. The
 *                     folders where the file is to be created must already exist
 *                     and be writable, and the pattern ^^^ will be replaced by
 *                     random unique characters between a-z. The pathTemplate
 *                     variable is actually modified and will point to the path
 *                     corresponding to the returned file handle.
 * @param flags File access flags such as O_RDONLY, O_WRONLY or O_RDWR.
 * @param mode Desired file permissions if this file is to be created. Ignored
 *             on Windows.
 * @param code Parameter validation error or \ref RUE_OK if parameters are good.
 * @return A valid file handle or -1 in case of error. Then check errno.
 */
RUAPI int ruOpenTmp(char *pathTemplate, int flags, int mode, int32_t *code);

/**
 * Abstracted version of the standard write function.
 * @param oh A handle returned by \ref ruOpenTmp or \ref ruOpen
 * @param contents buffer to write
 * @param length number of bytes to write
 * @return number of bytes written or < 0 on error then check errno
 */
RUAPI rusize_s ruWrite(int oh, trans_ptr contents, rusize length);

/**
 * Write the string stored in contents into a new file at filename. Any old file
 * at that location will be overwritten by the new file.
 * @param filename Path to the file to create.
 * @param contents Contents to write into the file. This may be a NULL
 *                 terminated encoding irrelevant string or binary data.
 *                 If contents is NULL an empty file will be created.
 * @param length Length of the content. May be \ref RU_SIZE_AUTO for a NULL
 *               terminated string.
 * @return Return code of the operation or \ref RUE_OK on success.
 */
RUAPI int32_t ruFileSetContents(trans_chars filename, trans_chars contents, rusize length);

/**
 * Reads the contents out of the given filepath into the variable and sets the
 * length.
 * @param filename Path to the file to read.
 * @param contents Where to store the read content. Caller must use \ref ruFree
 *                 when done with it.
 * @param length Number of bytes in contents.
 * @return Return code of the operation or \ref RUE_OK on success.
 */
RUAPI int ruFileGetContents(trans_chars filename, alloc_chars* contents, rusize* length);

/**
 * \brief Copies srcpath to destpath
 * @param srcpath file to copy
 * @param destpath path to copy srcpath to
 * @return Return code of the operation or \ref RUE_OK on success.
 */
RUAPI int ruFileCopy(trans_chars srcpath, trans_chars destpath);

/**
 * Renames a given file. This is equivalent to moving a file. Any previous file
 * at the destination will be overwritten.
 * @param oldName Old file path.
 * @param newName The new file path.
 * @return Return code of the operation or \ref RUE_OK on success.
 */
RUAPI int ruFileRename(trans_chars oldName, trans_chars newName);

/**
 * \brief Renames a given file like \ref ruFileRename, but fails when newName
 * already exists instead of overwriting it.
 * @param oldName Old file path.
 * @param newName The new file path.
 * @return Return code of the operation or \ref RUE_OK on success.
 */
RUAPI int ruFileTryRename(trans_chars oldName, trans_chars newName);

/**
 * Delete a given file or empty folder.
 * @param filename File or folder to delete.
 * @return Return code of the operation or \ref RUE_OK on success.
 */
RUAPI int ruFileRemove(trans_chars filename);

/**
 * Used by \ref ruFolderWalk to specify that the folder be handled before its contents.
 */
#define RU_WALK_FOLDER_FIRST 0x1
/**
 * Used by \ref ruFolderWalk to specify that the folder be handled after its contents.
 */
#define RU_WALK_FOLDER_LAST 0x2
/**
 * Used by \ref ruFolderWalk to specify whether to not recurse into subfolders.
 */
#define RU_WALK_NO_RECURSE 0x4
/**
 * Used by \ref ruFolderWalk to specify not to process the top level folder.
 */
#define RU_WALK_NO_SELF 0x8
/**
 * Used by \ref ruFolderWalk to specify passing paths with unix style directory
 * slashes to the callbacks. Ignored on Unix platforms.
 */
#define RU_WALK_UNIX_SLASHES 0x10


/**
 * \brief A function that is called with each entry of a \ref ruFolderWalk to
 * decide whether that entry should be processed or not.
 * @param folderPath The full file path to the folder under which fileName
 *        resides. Must be copied to persist. Folder may or may not contain a
 *        trailing slash.
 * @param fileName The file base name of the current entry. Must be copied
 *        to persist.
 * @param isFolder whether this is a folder.
 * @param ctx a user definable context.
 * @return true to filter the entry or false to continue processing it and its
 *         potential sub foilders.
 */
typedef bool (*entryFilter) (trans_chars folderPath, trans_chars fileName, bool isFolder, ptr ctx);

/**
 * \brief A function that is called with each entry of a \ref ruFolderWalk.
 * @param fullPath The full file path to the entry in question. Must be copied
 *        to persist. Folder may or may not contain a trailing slash.
 * @param isFolder whether this is a folder.
 * @param ctx a user definable context.
 * @return \ref RUE_OK to continue, else a regify error code.
 */
typedef int32_t (*entryMgr) (trans_chars fullPath, bool isFolder, ptr ctx);

/**
 * \brief Like \ref ruFolderWalk with additional filtering capability.
 * Folder entries . and .. are not passed to filter.
 * @param folder Folder to start descending into. Does not accept / or \\
 * @param flags Flags of type RU_WALK_* to specify walking behavior. All flags
 *        may be specified.
 * @param filter Function to call with each entry to decide filtering
 * @param actor Function to call with each entry
 * @param ctx a user definable context to be passed to the \ref entryFilter and
 *            \ref entryMgr functions.
 * @return
 */
RUAPI int32_t ruFilteredFolderWalk(trans_chars folder, uint32_t flags,
                                   entryFilter filter, entryMgr actor, ptr ctx);

/**
 * \brief Decends into given folder depth first and calls actor on each entry.
 * Folder entries . and .. are not passed to actor.
 * @param folder Folder to start descending into. Does not accept / or \\
 * @param flags Flags of type RU_WALK_* to specify walking behavior. All flags
 *        may be specified.
 * @param actor Function to call with each entry
 * @param ctx a user definable context to be passed to the \ref entryMgr function.
 * @return
 */
RUAPI int32_t ruFolderWalk(trans_chars folder, uint32_t flags, entryMgr actor, ptr ctx);

/**
 * \brief Returns the number of folder entries including itself
 * @param folder Folder to count entries of
 * @return 0 if folder is not readable,
 *         1 if folder is empty or
 *         the number of entries it has - 1 for itself.
 */
RUAPI ru_int ruFolderEntries(trans_chars folder);

/**
 * \brief Recursively delete given folder and all it's sub contents.
 * If given path is a file or symlink, it will also be removed.
 * @param folder Name of folder to delete. May not be "", "/" or "\\"
 * @return \ref RUE_OK on success else and error call \ref
 *         ruLastError on \ref RUE_GENERAL for details.
 */
RUAPI int32_t ruFolderRemove(trans_chars folder);

/**
 * Creates the requested folder with the requested permissions.
 * @param pathname Folder to create
 * @param mode Permissions to set (Unix only).
 * @param deep Whether to create any upper level folders.
 * @return \ref RUE_OK on success, else a regify error code.
 */
RUAPI int32_t ruMkdir(trans_chars pathname, int mode, bool deep);

/**
 * \brief Returns the dirname part of a full path without the trailing slash.
 *
 * @param filePath File path to return the dirname of.
 * @return Returns the directory name part of the given path excluding its
 *         trailing slash or NULL if no slashes were found. Caller needs to free
 *         the result with \ref ruFree after use.
 */
RUAPI alloc_chars ruDirName(trans_chars filePath);

/**
 * \brief Returns the filename of a path with the preceding path removed
 * @param filePath File path to return the basename of.
 * @return Returns the start of the basename within the given filePath so do not
 *         free this reference and copy it as needed.
 */
RUAPI perm_chars ruBaseName(perm_chars filePath);

/**
 * \brief Returns the extension part of the given file name if there is one
 * @param filePath File path to get extension from
 * @return NULL if there's no extension otherwise pointer to where it starts in
 *         filePath.
 */
RUAPI perm_chars ruFileExtension(perm_chars filePath);

/**
 * \brief Returns an absolute filepath leading to the given relative path.
 *
 * This function merely appends the given path to CWD if it's relative. It does
 * not validate or process the given path in any other way.
 * @param filePath File path to append the current working directory to if it is relative.
 * @return Returns an absolute filepath leading to the given relative path or
 *         the given path if it was absolute. Path must be freed by the caller.
 */
RUAPI alloc_chars ruFullPath(trans_chars filePath);

/**
 * \brief Returns base and file joined with the filesystem slash in a new string
 * @param base folder with or without trailing slash
 * @param file file anme to append
 * @return Returns a new filepath to be freed by the caller after use.
 */
RUAPI alloc_chars ruPathJoinNative(trans_chars base, trans_chars file);

/**
 * \brief Returns base and file joined with the a unix filesystem slash in a new string
 * @param base folder with or without trailing slash
 * @param file file anme to append
 * @return Returns a new filepath to be freed by the caller after use.
 */
RUAPI alloc_chars ruPathJoin(trans_chars base, trans_chars file);

/**
 * \brief Return a path by concatenating the given parts number of parameters
 * using a unix style directory slash.
 *
 * The returned path will have the leading slash supplied with the first folder
 * argument. There will be no trailing slash returned.
 *
 * @param parts Number of folder name that follow
 * @param ... parts number of folder names to concatenate. Trailing slashes
 *            and leading slashes after the second folder are accounted for.
 * @return Allocated path to be freed by the caller. Or NULL if parts was < 1.
 */
RUAPI alloc_chars ruPathMultiJoin(int parts, ...);

/**
 * \brief Return a path by concatenating the given parts number of parameters
 * using the platform native directory slash.
 *
 * The returned path will have the leading slash supplied with the first folder
 * argument. There will be no trailing slash returned.
 *
 * @param parts Number of folder name that follow
 * @param ... parts number of folder names to concatenate. Trailing slashes
 *            and leading slashes after the second folder are accounted for.
 * @return Allocated path to be freed by the caller. Or NULL if parts was < 1.
 */
RUAPI alloc_chars ruPathMultiJoinNative(int parts, ...);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_IO_H
