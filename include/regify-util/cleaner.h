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
 * \defgroup cleaner Data Cleaning Utility
 * \brief This section contains a text replacement implementation that is fast
 * and scalable.
 *
 * It is meant to be used to clean credentials out of large files in a minimum
 * amount of time.
 *
 * Example:
 * ~~~~~{.c}
 * #include <stdio.h>
 * #include <regify-util.h>
 *
 * rusize_s reader(const void* in, void* data, rusize len) {
 *     return (rusize_s) fread(data, len, 1, (FILE*)in);
 * }
 *
 * rusize_s writer(const void* out, const void* data, rusize len) {
 *     return (rusize_s) fwrite(data, len, 1, (FILE*)out);
 * }
 *
 * void clean() {
 *     int32_t ret;
 *     FILE *in = 0, *out = 0;
 *     ruCleaner rc = ruCleanNew(0);
 *     const char* repl = "^^^PASSWORD^^^";
 *     do {
 *         ret = ruCleanAdd(rc, "mysecret", repl);
 *         if (ret != RUE_OK) {
 *             printf("Failed adding secret: %d", ret);
 *             break;
 *         }
 *         ret = ruCleanAdd(rc, "myothersecret", repl);
 *         if (ret != RUE_OK) {
 *             printf("Failed adding other secret: %d", ret);
 *             break;
 *         }
 *         in = ruFOpen("debug.log", "r", &ret);
 *         if (ret != RUE_OK) {
 *             printf("Failed opening debug.log: %d", ret);
 *             break;
 *         }
 *         out = ruFOpen("clean.log", "w", &ret);
 *         if (ret != RUE_OK) {
 *             printf("Failed opening clean.log: %d", ret);
 *             break;
 *         }
 *         ret = ruCleanIo(rc, &reader, in, &writer, out);
 *         if (ret != RUE_OK) {
 *             printf("Failed setting I/O context: %d", ret);
 *             break;
 *         }
 *         ret = ruCleanNow(rc);
 *         if (ret != RUE_OK) {
 *             printf("Failed cleaning log: %d", ret);
 *             break;
 *         }
 *     } while(0);
 *
 *     if (in) fclose(in);
 *     if (out) fclose(out);
 *     ruCleanFree(rc);
 * }
 * ~~~~~
 * @{
 */
#ifndef REGIFY_UTIL_CLEANER_H
#define REGIFY_UTIL_CLEANER_H

/* Only need to export C interface if used by C++ source code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*****************************************************************************
*                                                                            *
*  ---------------------- Public Cleaner Interface ------------------------  *
*                                                                            *
*****************************************************************************/

/**
 * The type function for the write operation when cleaning a data stream.
 */
typedef rusize_s (*rcWriteFn) (perm_ptr ctx, trans_ptr buf, rusize len);

/**
 * The type function for the read operation when cleaning a data stream.
 */
typedef rusize_s (*rcReadFn) (perm_ptr ctx, ptr buf, rusize len);

/**
 * \brief Creates a new ruCleaner object. To be freed with \ref ruCleanFree.
 *
 * Unless the ruCleaner comes from a pwcleaner library which lacks the remaining
 * regify-util functionality, it is made thread safe by the internal use of a
 * mutex. pwcleaner users need to do their own locking in multi threaded use cases.
 *
 * @param chunkSize Size of chunk to process at a time. Will be allocated twice.
 *                  Will be increased to the largest item to clean if that is
 *                  bigger. Defaults to 1M if set to 0.
 * @return Guaranteed to return new ruCleaner object, or process abort.
 */
RUAPI ruCleaner ruCleanNew(rusize chunkSize);

/**
 * Frees the given ruCleaner object.
 * @param rc list to free.
 * @return NULL
 */
RUAPI ruCleaner ruCleanFree(ruCleaner rc);

/**
 * A callback for the \ref ruCleanDump function. The given key and subst parameters
 * belong to the cleaner and must not be freed.
 * @param user_data The optional context that was given to \ref ruCleanDump.
 * @param key The secret key to clean.
 * @param subst The placeholder to substitute the secret with.
 */
typedef void (*ruCleanerCb) (perm_ptr user_data, trans_chars key, trans_chars subst);

/**
 * Iterates over the cleaner database and calls given \ref ruCleanerCb with
 * each secret and substitute. This function is mainly used for debugging.
 * @param cp The relevant ruCleaner object
 * @param lf The \ref ruCleanerCb to call with each entry
 * @param user_data An optional context that will be given to the callbacks user_data parameter.
 * @return \ref RUE_OK on success else an error code.
 */
RUAPI int32_t ruCleanDump(ruCleaner cp, ruCleanerCb lf, perm_ptr user_data);

/**
 * Adds a new string to be replaced.
 * @param rc The relevant ruCleaner object
 * @param instr The string to be replaced. Will be copied.
 * @param substitute The string to replace instr with. Will be copied.
 * @return \ref RUE_OK on success else an error code.
 */
RUAPI int32_t ruCleanAdd(ruCleaner rc, trans_chars instr, trans_chars substitute);

/**
 * Removes a string entry from the database.
 * @param rc The relevant ruCleaner object
 * @param instr The string to be removed
 * @return \ref RUE_OK on success else an error code.
 */
RUAPI int32_t ruCleanRemove(ruCleaner rc, trans_chars instr);

/**
 * \brief Does replacements using the given I/O functions.
 *
 * This can be an opened file with the corresponding file handle as context.
 * This function may be called multiple times and interchangeably with the
 * ruCleanTo* functions.
 * @param rc The relevant ruCleaner object
 * @param reader The read function to be called for data input.
 * @param readCtx The context to be passed to the read function.
 * @param writer The write function to be called for data output.
 * @param writeCtx The context to be passed to the write function.
 * @return \ref RUE_OK on success else an error code.
 */
RUAPI int32_t ruCleanIo(ruCleaner rc, rcReadFn reader, perm_ptr readCtx,
                        rcWriteFn writer, perm_ptr writeCtx);

/**
 * \brief Does replacements on given string using the given Output function.
 *
 * This can be an opened file with the corresponding file handle as context.
 * This function may be called multiple times and interchangeably with the
 * ruCleanTo*, ruCleanIo functions.
 * This function is thread safe but locks allow only one thread at time.
 * @param rc The relevant ruCleaner object
 * @param in String to replace tokens in.
 * @param len Length of the string or 0 to use the null terminator for length
 *            detection.
 * @param writer The write function to be called for data output.
 * @param writeCtx The context to be passed to the write function.
 * @return \ref RUE_OK on success else an error code.
 */
RUAPI int32_t ruCleanToWriter(ruCleaner rc, trans_chars in, rusize len,
                              rcWriteFn writer, perm_ptr writeCtx);

#ifndef CLEANER_ONLY
/**
 * \brief Does replacements in given string and returns out in given \ref ruString.
 *
 * This function may be called multiple times and interchangeably with the
 * ruCleanTo*, ruCleanIo functions.
 * This function is thread safe but locks allow only one thread at time.
 * @param rc The relevant ruCleaner object
 * @param in String to replace tokens in.
 * @param len Length of the string or 0 to use the null terminator for length
 *            detection.
 * @param out Where the \ref ruString containing the result will be stored.
 *            Caller must free with \ref ruStringFree after use.
 * @return \ref RUE_OK on success else an error code.
 */
RUAPI int32_t ruCleanToString(ruCleaner rc, trans_chars in, rusize len, ruString *out);
#endif

/**
 * @}
 */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_CLEANER_H
