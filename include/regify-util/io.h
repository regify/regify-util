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
/* Only need to export C interface if used by C++ source code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Checks whether a file or folder exists.
 * @param filename Name of file, folder or other filesystem entity.
 * @return Return true if the path exists else false.
 */
RUAPI bool ruFileExists(const char* filename);

/**
 * Returns true if the given path exists and is a regular file.
 * On Unix if the underlying path is a symlink it is dereferenced and the status
 * of the target is returned.
 * @param filename File path to test
 * @return Returns true if it is a regular file.
 */
RUAPI bool ruIsFile(const char* filename);

/**
 * Returns true if the given path exists and is a folder.
 * On Unix if the underlying path is a symlink it is dereferenced and the status
 * of the target is returned.
 * @param filename File path to test
 * @return Returns true if it is a folder.
 */
RUAPI bool ruIsDir(const char* filename);

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
RUAPI bool ruIsExecutable(const char* filename);

#ifndef _WIN32
/**
 * Returns true if the given path exists and is a symbolic link (Unix only).
 * @param filename File path to test
 * @return Returns true if it is a symlink.
 */
RUAPI bool ruIsSymlink(const char* filename);
#endif

/**
 * An abstracted version of struct stat.
 */
typedef struct stat ruStat_t;

/**
 * Populates the given \ref ruStat_t with the data of the filepath if it exists.
 * @param filepath File path to test
 * @param dest Pointer to \ref ruStat_t to populate.
 * @return Returns \ref RUE_OK on success else \ref RUE_PARAMETER_NOT_SET if
 *         filename was NULL or \ref RUE_FILE_NOT_FOUND if file didn't exists
 *         or \ref RUE_CANT_OPEN_FILE if it was another error. Check errno or
 *         call \ref ruLastError for details.
 */
RUAPI int ruStat(const char *filepath, ruStat_t *dest);

/**
 * Abstracted version of the Posix open call.
 * @param filepath Path to the file to open
 * @param flags File access flags such as O_RDONLY, O_WRONLY or O_RDWR.
 * @param mode Desired file permissions if this file is to be created. Ignored
 *             on Windows.
 * @param code Parameter validation error or \ref RUE_OK if parameters are good.
 * @return A valid file handle or -1 in case of error. Then check errno.
 */
RUAPI int ruOpen(const char *filepath, int flags, int mode, int32_t* code);

/**
 * Abstracted version of the Posix fopen call.
 * @param filepath Path to the file to open
 * @param mode File access mode such as r, r+, w, w+, a or a+.
 * @param code Parameter validation error or \ref RUE_OK if parameters are good.
 * @return A valid file handle or -1 in case of error. Then check errno.
 */
RUAPI FILE* ruFOpen(const char *filepath, const char *mode, int32_t* code);

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
 * Write the string stored in contents into a new file at filename. Any old file
 * at that location will be overwritten by the new file.
 * @param filename Path to the file to create.
 * @param contents Contents to write into the file. This may be a NULL
 *                 terminated encoding irrelevant string or binary data.
 * @param length Length of the content. May be -1 for a NULL terminated string.
 * @return Return code of the operation or \ref RUE_OK on success.
 */
RUAPI int32_t ruFileSetContents(const char *filename, const char *contents, rusize_s length);

/**
 * Reads the contents out of the given filepath into the variable and sets the
 * length.
 * @param filename Path to the file to read.
 * @param contents Where to store the read content. Caller must use \ref ruFree
 *                 when done with it.
 * @param length Number of bytes in contents.
 * @return Return code of the operation or \ref RUE_OK on success.
 */
RUAPI int ruFileGetContents(const char *filename, char **contents, rusize *length);

/**
 * Renames a given file. This is equivalent to moving a file. Any previous file
 * at the destination will be overwritten.
 * @param oldName Old file path.
 * @param newName The new file path.
 * @return Return code of the operation or \ref RUE_OK on success.
 */
RUAPI int ruFileRename(const char* oldName, const char* newName);

/**
 * Delete a given file or empty folder.
 * @param filename File or folder to delete.
 * @return 0 on success or Posix remove error codes.
 */
RUAPI int ruFileRemove(const char* filename);

/**
 * \brief A function that is called with each entry of a \ref ruFolderWalk.
 * @param fullPath The full file path to the entry in question.
 * @param isFolder whether this is a folder.
 * @param ctx a user definable context.
 */
typedef int (*entryMgr) (const char *fullPath, bool isFolder, void* ctx);

/**
 * \brief Decends into giver folder depth first and calls actor on each entry.
 * Folder entries . and .. are not passed to actor.
 * @param folder Folder to start decending into.
 * @param actor Function to call with each entry
 * @param ctx a user definable context to be passed to the \ref entryMgr function.
 * @return
 */
RUAPI int32_t ruFolderWalk(const char* folder, entryMgr actor, void *ctx);

/**
 * \brief Recursively delete given folder and all it's sub contents.
 * @param folder Name of folder to delete. May not be "", "/" or "\\"
 * @return \ref RUE_OK on success else \ref RUE_GENERAL and call \ref
 *         ruLastError for details.
 */
RUAPI int ruFolderRemove(const char* folder);

/**
 * Creates the requested folder with the requested permissions.
 * @param pathname Folder to create
 * @param mode Permissions to set (Unix only).
 * @param deep Whether to create any upper level folders.
 * @return \ref RUE_OK on success, else a regify error code.
 */
RUAPI int ruMkdir(const char *pathname, int mode, bool deep);

/**
 * \brief Returns the dirname part of a full path.
 * @param filePath File path to return the dirname of.
 * @return Returns the directory name part of the given path or NULL if no
 *         slashes were found. Caller needs to free the result with \ref ruFree
 *         after use.
 */
RUAPI char* ruDirName(const char *filePath);

/**
 * \brief Returns the filename of a path with the preceeding path removed
 * @param filePath File path to return the basename of.
 * @return Returns the start of the basename within the given filePath so do not
 *         free this reference and copy it as needed.
 */
RUAPI char* ruBaseName(const char *filePath);

/**
 * \brief Returns an absolute filepath leading to the given relative path.
 *
 * This function merely appends the given path to CWD if it's relative. It does
 * not validate or process the given path in any other way.
 * @param filePath File path to append the current working directory to if it is relative.
 * @return Returns an absolute filepath leading to the given relative path or
 *         the given path if it was absolute. Path must be freed by the caller.
 */
RUAPI char* ruFullPath(const char* filePath);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_IO_H
