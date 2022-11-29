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
 * \defgroup string String Functions
 * \brief This section contains utilities relating to string manipulation.
 *
 * @{
 */

#ifndef REGIFY_UTIL_STRING_H
#define REGIFY_UTIL_STRING_H
/* Only need to export C interface if used by C++ source code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**
 * \defgroup ruString String Object
 * \brief A String buffer implementation.
 * This implementation allows for dynamically creating string and appending to
 * them in a reasonably efficient matter.
 * @{
 */
/**
 * \brief An opaque data type representing a regify-util string object.
 */
typedef void* ruString;

/**
 * \brief Creates a new String object from a string.
 * @param instr String to start object with.
 * @return The newly created \ref ruString object to be freed with \ref
 *         ruStringFree or NULL on error. Check \ref ruLastError in case of NULL;
 */
RUAPI ruString ruStringNew(const char* instr);

/**
 * \brief Creates a new String object of initial size from a string.
 * @param instr String to start object with.
 * @param size Initial buffer to allocate.
 * @return The newly created \ref ruString object to be freed with \ref
 *         ruStringFree or NULL on error. Check \ref ruLastError in case of NULL;
 */
RUAPI ruString ruStringNewn(const char* instr, rusize size);

/**
 * \brief Creates a new String object from a formatted string.
 * @param format A general printf format string to format the remaining arguments.
 * @param ... Any remaining arguments to add to the format string.
 * @return The newly created \ref ruString object to be freed with \ref
 *         ruStringFree or NULL on error. Check \ref ruLastError in case of NULL;
 */
RUAPI ruString ruStringNewf(const char* format, ...);

/**
 * \brief Frees the given \ref ruString object.
 * @param rs String to free
 * @param keepBuffer Whether to also free the underlying char* buffer.
 */
RUAPI ruString ruStringFree(ruString rs, bool keepBuffer);

/**
 * \brief Append given string to \ref ruString.
 * @param rs String buffer to append string to.
 * @param str String to append.
 * @return \ref RUE_OK on success else a regify error code.
 */
RUAPI int32_t ruStringAppend(ruString rs, const char* str);

/**
 * \brief Append given string URI encoded to \ref ruString.
 * @param rs String buffer to append string to.
 * @param instr String to append.
 * @return \ref RUE_OK on success else a regify error code.
 */
RUAPI int32_t ruStringAppendUriEncoded(ruString rs, const char* instr);

/**
 * \brief Append given string to \ref ruString.
 * @param rs String buffer to append string to.
 * @param instr String to append.
 * @param len Number of bytes of string to append.
 * @return \ref RUE_OK on success else a regify error code.
 */
RUAPI int32_t ruStringAppendn(ruString rs, const char* instr, rusize len);

/**
 * \brief Appends formatted string to \ref ruString.
 * @param rs String buffer to append string to.
 * @param format A general printf format string to format the remaining arguments.
 * @param ... Any remaining arguments to add to the format string.
 * @return \ref RUE_OK on success else a regify error code.
 */
RUAPI int32_t ruStringAppendf(ruString rs, const char* format, ...);

/**
 * \brief Returns the underlying char* of the given \ref ruString.
 * @param rs String buffer to return buffer of.
 * @return Underlying char* or NULL in case rs was invalid.
 */
RUAPI char* ruStringGetCString(ruString rs);

/**
 * \brief Returns the current length of the \ref ruString.
 * @param rs String buffer to return the length of.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return The length of the string or 0 in case of an error.
 */
RUAPI rusize ruStringLen(ruString rs, int32_t *code);

/**
 * \brief Whether \ref ruString ends with given suffix.
 * @param rs String buffer to check.
 * @param suffix Suffix to test for.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Whether string ends with suffix.
 */
RUAPI bool ruStringEndsWith(ruString rs, const char *suffix, int32_t *code);
/**
 * @}
 */

/**
 * \brief Normalizer Mode for NFC normalization for \ref ruUtf8CaseNormalize.
 */
#define ruUtf8Nfc 1

/**
 * \brief Normalizer Mode for NFD normalization for \ref ruUtf8CaseNormalize.
 */
#define ruUtf8Nfd 2
// these seem to be generally missing ATM
//#define ruUtf8Nfkc 3
//#define ruUtf8Nfkd 4

/**
 * \brief Casemode constant to lower case text for \ref ruUtf8CaseNormalize.
 */
#define ruUtf8Lower 1

/**
 * \brief Casemode constant to lower case text for \ref ruUtf8CaseNormalize.
 */
#define ruUtf8Upper 2

/**
 * \brief Returns the given string normalized and/or case switched as requested.
 * @param instr The string to convert.
 * @param normMode Normalization mode of \ref ruUtf8Nfc, \ref ruUtf8Nfd or 0
 *                 for untouched.
 * @param caseMode Case toggling mode of \ref ruUtf8Lower, \ref ruUtf8Upper or
 *                 0 for untouched.
 * @return A copy of the given string with the requested tranformations made or
 *         NULL incase of error. In that case call \ref ruLastError for details.
 */
RUAPI char* ruUtf8CaseNormalize(const char *instr, int32_t normMode, int32_t caseMode);

/**
 * \brief Replaces any occurrence of search in string with replace.
 *
 * This function works byte level not character level.
 * NOTE: Calling this function with strings on the stack will segfault.
 *
 * @param string String to process
 * @param search Character to replace
 * @param replace Character to replace search with
 */
RUAPI void ruStrByteReplace(char* string, char search, char replace);

/**
 * \brief Returns what libc strcmp would return but in a manner that gracefully
 * handles NULL imputs.
 * @param str1 The first string to check.
 * @param str2 The second string to compare to the first.
 * @return -1, 1 or 0 in case of them being equal or both NULL.
 */
RUAPI int32_t ruStrcmp(trans_chars str1, trans_chars str2);

/**
 * \brief Returns true when libc strcmp would return 0 but in a manner that gracefully
 * handles NULL imputs.
 * @param str1 The first string to check.
 * @param str2 The second string to compare to the first.
 * @return true if both strings are the same or NULL
 */
RUAPI bool ruStrEquals(trans_chars str1, trans_chars str2);

/**
 * \brief Returns what libc strcasecmp would return but in a manner that gracefully
 * handles NULL imputs.
 * @param str1 The first string to check.
 * @param str2 The second string to compare to the first.
 * @return -1, 1 or 0 in case of them being equal or both NULL.
 */
RUAPI int32_t ruStrcasecmp(trans_chars str1, trans_chars str2);

/**
 * \brief Whether given string starts with given prefix.
 * @param str String to check.
 * @param prefix Prefix to test for.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Whether string starts with the prefix.
 */
RUAPI bool ruStrStartsWith(trans_chars str, trans_chars prefix, int32_t *code);

/**
 * \brief Whether given string ends with given suffix.
 * @param str String to check.
 * @param suffix Suffix to test for.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Whether string ends with the suffix.
 */
RUAPI bool ruStrEndsWith(trans_chars str, trans_chars suffix, int32_t *code);

/**
 * \brief Whether given string ends with given suffix ignoring case.
 * @param str String to check.
 * @param suffix Suffix to test for.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Whether string ends with the suffix.
 */
RUAPI bool ruStrEndsCaseWith(trans_chars str, trans_chars suffix, int32_t *code);

/**
 * \brief Searches the first instance of needle in haystack up to len bytes.
 * @param haystack String to search needle in.
 * @param needle String to find in haystack.
 * @param len Maximum number of bytes in haystack to consider.
 * @return Start of the first found instance of haystack or NULL if not found.
 */
RUAPI char* ruStrStrLen(const char* haystack, const char* needle, rusize len);

/**
 * \brief Searches the first instance of needle in haystack.
 * @param haystack String to search needle in.
 * @param needle String to find in haystack.
 * @return Start of the first found instance of haystack or NULL if not found.
 */
RUAPI char* ruStrStr(const char* haystack, const char* needle);

/**
 * \brief Searches the last instance of needle in haystack up to len bytes.
 * @param haystack String to search needle in.
 * @param needle String to find in haystack.
 * @param len Maximum number of bytes in haystack to consider.
 * @return Start of the last found instance of haystack or NULL if not found.
 */
RUAPI char* ruLastSubstrLen(const char* haystack, const char* needle, rusize len);

/**
 * \brief Searches the last instance of needle in haystack.
 * @param haystack String to search needle in.
 * @param needle String to find in haystack.
 * @return Start of the last found instance of haystack or NULL if not found.
 */
RUAPI char* ruLastSubstr(const char* haystack, const char* needle);

/**
 * \brief split given instr with delim into a \ref ruList.
 * @param instr String to split.
 * @param delim Delimiter to split string on.
 * @param maxCnt Maximum number of pieces to return or 0 for no limit. The
 *        remainder will be unsplit in the last piece.
 * @return The list of pieces  to be free by caller with \ref ruListFree or
 *         NULL in case of a parameter error.
 */
RUAPI ruList ruStrsplit (const char *instr, const char *delim, int32_t maxCnt);

/**
 * \brief Returns lowercase representation of given ASCII character.
 * @param in Character to lowercase.
 * @return The lowercase version of given character or the same if not uppercase
 *         ASCII.
 */
RUAPI char ruAsciiCharToLower(char in);

/**
 * \brief Returns a lowercase copy of given ASCII string.
 * @param instr String to lowercase.
 * @return Lowercase representation of given string. Free with \ref ruFree.
 */
RUAPI char* ruAsciiToLower(char *instr);

/**
 * \brief Returns a lowercase copy of len bytes of given ASCII string.
 * @param instr String to lowercase.
 * @param len The number of bytes to consider
 * @return Lowercase representation of given string. Free with \ref ruFree.
 */
RUAPI char* ruAsciiNToLower(char *instr, rusize len);

/**
 * \brief Returns uppercase representation of given ASCII character.
 * @param in Character to lowercase.
 * @return The uppercase version of given character or the same if not lowercase
 *         ASCII.
 */
RUAPI char ruAsciiCharToUpper(char in);

/**
 * \brief Returns a uppercase copy of given ASCII string.
 * @param instr String to uppercase.
 * @return Uppercase representation of given string. Free with \ref ruFree.
 */
RUAPI char* ruAsciiToUpper(char *instr);

/**
 * \brief Returns a uppercase copy of len bytes of given ASCII string.
 * @param instr String to uppercase.
 * @param len The number of bytes to consider
 * @return Uppercase representation of given string. Free with \ref ruFree.
 */
RUAPI char* ruAsciiNToUpper(char *instr, rusize len);

/**
 * \brief Returns a lowercase copy of given UTF8 string.
 * @param instr String to lowercase.
 * @return Lowercase representation of given string. Free with \ref ruFree.
 */
RUAPI char* ruUtf8ToLower(const char *instr);

/**
 * \brief Returns a uppercase copy of given UTF8 string.
 * @param instr String to uppercase.
 * @return Uppercase representation of given string. Free with \ref ruFree.
 */
RUAPI alloc_chars ruUtf8ToUpper(trans_chars instr);

/**
 * \brief Returns a copy of given string
 * @param str String to copy
 * @return Copy of given string or NULL if NULL was given. Free with \ref ruFree.
 */
RUAPI char* ruStrdup(const char* str);

/**
 * \brief Returns a copy of len bytes of given string
 * @param str String to copy
 * @param len Number of bytes to consider.
 * @return Copy of given string or NULL if NULL was given. Free with \ref ruFree.
 */
RUAPI char* ruStrndup(const char* str, rusize len);

/**
 * \brief Returns an allocated string from given vprintf call.
 * @param format Printf style format string
 * @param arglist a va_start initialized argument list
 * @return Allocated string copy of formatted string. Free with \ref ruFree.
 */
RUAPI char* ruDupvPrintf(const char* format, va_list arglist);

/**
 * \brief Returns an allocated string from given printf call.
 * @param format Printf style format string
 * @param ... Remaining arguments for the format string.
 * @return Allocated string copy of formatted string. Free with \ref ruFree.
 */
RUAPI char* ruDupPrintf(const char* format, ...);

/**
 * Parses long out of given string in given base assuming and ignoring comma
 * thousand separators. The decimal point is a period.
 * @param instr String to parse.
 * @param endptr (Optional) Points to the first character that did not parse properly
 * @param base a number between 2 and 36
 * @return The number result.
 */
RUAPI int64_t ruStrParseInt64(const char *instr, char **endptr, int base);

/**
 * \brief Like \ref ruStrParseInt64 but ignores overflow and trailing garbage
 * @param numstr String to contain numerals to parse
 * @return whatever strtoll returns
 */
RUAPI int64_t ruStrToInt64(const char* numstr);

/**
 * \brief Returns long from parsed string.
 * Overflow or trailing non numeric characters invalidate the result.
 *
 * @param numstr String to contain numerals to parse
 * @return The number on success or 0 with errno set.
 */
RUAPI long ruStrParseLong(const char* numstr);

/**
 * \brief Like \ref ruStrParseLong but ignores overflow and trailing garbage
 * @param numstr String to contain numerals to parse
 * @return whatever strtol returns
 */
RUAPI long ruStrToLong(const char* numstr);

/**
 * \brief Alias for \ref ruStrParseInt64
 */
#define ruStrToll ruStrParseInt64

/**
 * \brief Removes trailing whitespace from string in place.
 *
 * The given string should be on the heap, else a segfault may occur when trying
 * to modify something on the stack.
 * @param instr The string to trim
 * @return instr
 */
RUAPI char* ruStrTrim(char* instr);

/**
 * \brief Checks whether given string has any content other than whitespace
 * @param str String to inspect
 * @return true if NULL empty or only white space
 */
RUAPI bool ruStrEmpty(trans_chars str);

/**
 * \brief Removes the characters in chars from the string instr.
 * The given string should be on the heap, else a segfault may occur when trying
 * to modify something on the stack.
 * @param instr Input string to be stripped in place
 * @param chars Characters to remove from instr.
 */
RUAPI void ruStripChars(char *instr, const char* chars);

/**
 * @}
 */

/**
 * \defgroup buffer Buffer Functions
 * \brief This section contains utilities relating to buffer management.
 *
 * @{
 */

/**
 * \defgroup ruBuffer Buffer Object
 * \brief A data buffer implementation.
 * This implementation allows for dynamically creating a buffer and appending to
 * it in a reasonably efficient matter. It is like \ref ruString but is not NULL
 * terminated. Functions of both may be used interchangeably.
 */
/**
 * \brief An opaque data type representing a regify-util string object that is
 * not NULL terminated.
 */
typedef void* ruBuffer;

/**
 * \brief Creates a new buffer object of given initial size.
 * A \ref ruBuffer is essentially a \ref ruString that is not NULL terminated
 * and therefore has a length parameter. \ref ruString and \ref ruBuffer may be
 * used interchangeably.
 * @param initialSize Initial buffer to allocate.
 * @return The newly created \ref ruBuffer object to be freed with \ref
 *         ruBufferFree or NULL on error. Check \ref ruLastError in case of NULL;
 */
#define ruBufferNew(initialSize) (ruBuffer) ruStringNewn(NULL, initialSize)

/**
 * \brief Append given data to \ref ruBuffer.
 * @param rb Data buffer to append data to.
 * @param buf data to append.
 * @param len Number of bytes of data to append.
 * @return \ref RUE_OK on success else a regify error code.
 */
#define ruBufferAppend(rb, buf, len) ruStringAppendn(rb, buf, len)

/**
 * \brief Append given string URI encoded to \ref ruString.
 * @param rs Data buffer to append data to.
 * @param buf Data to append.
 * @param len Number of bytes of data to append.
 * @return \ref RUE_OK on success else a regify error code.
 */
RUAPI int32_t ruBufferAppendUriEncoded(ruString rs, const char* buf, rusize len);

/**
 * \brief Returns the underlying char* of the given \ref ruBuffer.
 * @param rb String buffer to return buffer of.
 * @return Underlying char* or NULL in case rs was invalid.
 */
#define ruBufferGetData(rb) ruStringGetCString(rb)

/**
 * \brief Returns the current length of the \ref ruBuffer.
 * @param rb Data buffer to return the length of.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return The length of the string or 0 in case of an error.
 */
#define ruBufferLen(rb, code) ruStringLen(rb, code)

/**
 * \brief Frees the given \ref ruString object.
 * @param rb String to free
 * @param keep Whether to also free the underlying char* buffer.
 */
#define ruBufferFree(rb, keep) ruStringFree(rb, keep);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_STRING_H
