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
 * \brief Empties the string for reuse without freeing the buffer.
 * @param rs String to reset
 * @return \ref RUE_OK on success else a regify error code.
 */
RUAPI int32_t ruStringReset(ruString rs);

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
 * @param len Number of bytes of string to append or \ref RU_SIZE_AUTO to use
 *        strlen to determine the length
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
 * \brief Casemode constant to leave case for \ref ruUtf8CaseNormalize intact.
 */
#define ruUtf8NoCase 0

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
 *                 \ref ruUtf8NoCase for untouched.
 * @return A copy of the given string with the requested tranformations made or
 *         NULL incase of error. In that case call \ref ruLastError for details.
 */
RUAPI char* ruUtf8CaseNormalize(const char *instr, int32_t normMode, int32_t caseMode);

/**
 * \brief Returns the given decomposed string in precomposed form.
 *
 * \see https://en.wikipedia.org/wiki/Unicode_equivalence#Normal_forms
 * @param instr Decomposed UTF8 string.
 * @return Standard precomposed UTF8 string.
 */
RUAPI alloc_chars ruStrFromNfd(trans_chars instr);

/**
 * \brief Returns the given string in decomposed form.
 *
 * \see https://en.wikipedia.org/wiki/Unicode_equivalence#Normal_forms
 * @param instr Standard precomposed UTF8 string
 * @return Decomposed UTF8 string.
 */
RUAPI alloc_chars ruStrToNfd(trans_chars instr);

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
 * \brief Replaces any occurrence of search in instr with replace.
 *
 * @param instr String to process
 * @param search String to replace
 * @param replace String to replace search with
 * @return A new string with the replacements made, caller must free.
 */
RUAPI alloc_chars ruStrReplace(trans_chars instr, trans_chars search,
                               trans_chars replace);

/**
 * \brief Returns what libc strcmp would return but in a manner that gracefully
 * handles NULL imputs.
 * @param str1 The first string to check.
 * @param str2 The second string to compare to the first.
 * @return -1, 1 or 0 in case of them being equal or both NULL.
 */
RUAPI int32_t ruStrCmp(trans_chars str1, trans_chars str2);

/**
 * \brief Returns true when libc strcmp would return 0 but in a manner that gracefully
 * handles NULL imputs.
 * @param str1 The first string to check.
 * @param str2 The second string to compare to the first.
 * @return true if both strings are the same or NULL
 */
RUAPI bool ruStrEquals(trans_chars str1, trans_chars str2);

/**
 * \brief Returns true when str2 is equal to the first len number of bytes in
 * str1 or both str1 and str2 are NULL or if len is NULL.
 * @param str1 The first string to check.
 * @param s1len Number of bytes in str1 to consider
 * @param str2 The second string to compare to the first.
 * @return true if both strings are the same or NULL
 */
RUAPI bool ruStrNEquals(trans_chars str1, rusize s1len, trans_chars str2);

/**
 * \brief Returns what libc strcasecmp would return but in a manner that gracefully
 * handles NULL imputs.
 * @param str1 The first string to check.
 * @param str2 The second string to compare to the first.
 * @return -1, 1 or 0 in case of them being equal or both NULL.
 */
RUAPI int32_t ruStrCaseCmp(trans_chars str1, trans_chars str2);

/**
 * \brief Returns true when libc strcasecmp would return 0 but in a manner that gracefully
 * handles NULL imputs.
 * @param str1 The first string to check.
 * @param str2 The second string to compare to the first.
 * @return true if both strings are the same or NULL
 */
RUAPI bool ruStrCaseEquals(trans_chars str1, trans_chars str2);

/**
 * \brief Whether given string starts with given prefix.
 * @param str String to check.
 * @param prefix Prefix to test for.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Whether string starts with the prefix.
 */
RUAPI bool ruStrStartsWith(trans_chars str, trans_chars prefix, int32_t *code);

/**
 * \brief Whether given string starts with given prefix ignoring case.
 * @param str String to check.
 * @param prefix Prefix to test for.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Whether string starts with the prefix.
 */
RUAPI bool ruStrCaseStartsWith(trans_chars str, trans_chars prefix, int32_t *code);

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
RUAPI bool ruStrCaseEndsWith(trans_chars str, trans_chars suffix, int32_t *code);

/**
 * \brief Searches the first instance of lowercase needle in lowercase haystack
 * up to len bytes. Lowercasing is only ASCII compatible.
 * @param haystack String to search needle in.
 * @param needle String to find in haystack.
 * @param len Maximum number of bytes in haystack to consider.
 * @return Start of the first found instance of haystack or NULL if not found.
 */
RUAPI trans_chars ruStrCaseStrLen(trans_chars haystack, trans_chars needle, rusize len);

/**
 * \brief Searches the first instance of needle in haystack up to len bytes.
 * @param haystack String to search needle in.
 * @param needle String to find in haystack.
 * @param len Maximum number of bytes in haystack to consider.
 * @return Start of the first found instance of haystack or NULL if not found.
 */
RUAPI trans_chars ruStrStrLen(trans_chars haystack, trans_chars needle, rusize len);

/**
 * \brief Searches the first instance of needle in haystack.
 * @param haystack String to search needle in.
 * @param needle String to find in haystack.
 * @return Start of the first found instance of haystack or NULL if not found.
 */
RUAPI trans_chars ruStrStr(trans_chars haystack, trans_chars needle);

/**
 * \brief Searches the last instance of needle in haystack up to len bytes.
 * @param haystack String to search needle in.
 * @param needle String to find in haystack.
 * @param len Maximum number of bytes in haystack to consider.
 * @return Start of the last found instance of haystack or NULL if not found.
 */
RUAPI trans_chars ruLastSubstrLen(trans_chars haystack, trans_chars needle, rusize len);

/**
 * \brief Searches the last instance of needle in haystack.
 * @param haystack String to search needle in.
 * @param needle String to find in haystack.
 * @return Start of the last found instance of haystack or NULL if not found.
 */
RUAPI trans_chars ruLastSubstr(trans_chars haystack, trans_chars needle);

/**
 * \brief Split given instr up to inlen bytes with delim into a \ref ruList.
 * @param instr String to split.
 * @param inlen Optional length delimiter. Use \ref RU_SIZE_AUTO to omit.
 * @param delim Delimiter to split string on.
 * @param maxCnt Maximum number of pieces to return or 0 for no limit. The
 *        remainder will be unsplit in the last piece.
 * @return The list of pieces  to be free by caller with \ref ruListFree or
 *         NULL in case of a parameter error.
 */
RUAPI ruList ruStrNSplit(trans_chars instr, rusize inlen, trans_chars delim, int32_t maxCnt);

/**
 * \brief split given instr with delim into a \ref ruList.
 * @param instr String to split.
 * @param delim Delimiter to split string on.
 * @param maxCnt Maximum number of pieces to return or 0 for no limit. The
 *        remainder will be unsplit in the last piece.
 * @return The list of pieces  to be free by caller with \ref ruListFree or
 *         NULL in case of a parameter error.
 */
RUAPI ruList ruStrSplit(trans_chars instr, trans_chars delim, int32_t maxCnt);

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
RUAPI alloc_chars ruAsciiToLower(trans_chars instr);

/**
 * \brief Returns a lowercase copy of len bytes of given ASCII string.
 * @param instr String to lowercase.
 * @param len The number of bytes to consider
 * @return Lowercase representation of given string. Free with \ref ruFree.
 */
RUAPI alloc_chars ruAsciiNToLower(trans_chars instr, rusize len);

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
RUAPI alloc_chars ruAsciiToUpper(trans_chars instr);

/**
 * \brief Returns a uppercase copy of len bytes of given ASCII string.
 * @param instr String to uppercase.
 * @param len The number of bytes to consider
 * @return Uppercase representation of given string. Free with \ref ruFree.
 */
RUAPI alloc_chars ruAsciiNToUpper(trans_chars instr, rusize len);

/**
 * \brief Returns a lowercase copy of given UTF8 string.
 * @param instr String to lowercase.
 * @return Lowercase representation of given string. Free with \ref ruFree.
 */
RUAPI alloc_chars ruUtf8ToLower(trans_chars instr);

/**
 * \brief Returns a uppercase copy of given UTF8 string.
 * @param instr String to uppercase.
 * @return Uppercase representation of given string. Free with \ref ruFree.
 */
RUAPI alloc_chars ruUtf8ToUpper(trans_chars instr);

/**
 * \brief Converts given UTF16 wide character sequence to UTF8
 * @param unistr UTF16 string to convert.
 * @param bytelen Length of unistr without terminating bytes or -1 to detect length
 * @return UTF8 representation of unistr. Caller must free.
 */
RUAPI alloc_chars ruStrFromNUtf16(trans_uni unistr, int32_t bytelen);

/**
 * \brief Converts given UTF16 wide character sequence to UTF8
 * @param unistr UTF16 string to convert.
 * @return UTF8 representation of unistr. Caller must free.
 */
RUAPI alloc_chars ruStrFromUtf16(trans_uni unistr);

/**
 * \brief Converts given UTF8 string to UTF16 as it is used on Windows
 * @param str UTF8 string to convert
 * @param bytelen Length of str without terminating byte or -1 to detect length.
 * @return UTF16 wide character string. Caller must free.
 */
RUAPI alloc_uni ruStrNToUtf16(trans_chars str, int32_t bytelen);

/**
 * \brief Converts given UTF8 string to UTF16 as it is used on Windows
 * @param str UTF8 string to convert
 * @return UTF16 wide character string. Caller must free.
 */
RUAPI alloc_uni ruStrToUtf16(trans_chars str);

/**
 * \brief Returns a copy of given string
 * @param str String to copy
 * @return Copy of given string or NULL if NULL was given. Free with \ref ruFree.
 */
RUAPI char* ruStrDup(const char* str);

/**
 * \brief behaves like strlen but returns 0 if NULL is passed in
 * @param str string to get length of
 * @return length of given string or 0 if string was NULL.
 */
RUAPI rusize ruStrLen(trans_chars str);

/**
 * \brief Returns a copy of len bytes of given string
 * @param str String to copy
 * @param len Number of bytes to consider.
 * @return Copy of given string or NULL if NULL was given. Free with \ref ruFree.
 */
RUAPI char* ruStrNDup(const char* str, rusize len);

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
 * Parses the number out of the given string in given base assuming and ignoring
 * comma thousand separators. The decimal point is a period.
 *
 * @param str  String to parse. Leading white space will be skipped.
 * @param endptr  (Optional) Points to the first character that did not parse properly
 * @param base a number between 2 and 36 or 0 for detecting hex with prefix 0x,
 *             octal with prefix 0 and else assuming decimal.
 * @param num Where the number result will be stored or 0 on error.
 * @return \ref RUE_OK on success \ref RUE_OVERFLOW when the number did not fit
 *         the type or \ref RUE_PARAMETER_NOT_SET and \ref RUE_INVALID_PARAMETER
 *         for not given or invalid input.
 */
RUAPI int32_t ruStrParseInt64(trans_chars str, perm_chars* endptr, uint32_t base, int64_t* num);

/**
 * \brief Like \ref ruStrParseInt64 but ignores overflow and trailing garbage
 * @param str String to contain numerals to parse
 * @return whatever strtoll returns
 */
RUAPI int64_t ruStrToInt64(trans_chars str);

/**
 * Parses the number out of the given string in given base assuming and ignoring
 * comma thousand separators. The decimal point is a period.
 *
 * @param str  String to parse. Leading white space will be skipped.
 * @param endptr  (Optional) Points to the first character that did not parse properly
 * @param base a number between 2 and 36 or 0 for detecting hex with prefix 0x,
 *             octal with prefix 0 and else assuming decimal.
 * @param num Where the number result will be stored or 0 on error.
 * @return \ref RUE_OK on success \ref RUE_OVERFLOW when the number did not fit
 *         the type or \ref RUE_PARAMETER_NOT_SET and \ref RUE_INVALID_PARAMETER
 *         for not given or invalid input.
 */
RUAPI int32_t ruStrParseLong(trans_chars str, perm_chars* endptr, uint32_t base, long* num);

/**
 * \brief Like \ref ruStrParseLong but ignores overflow and trailing garbage
 * @param str String to contain numerals to parse
 * @return the number
 */
RUAPI long ruStrToLong(trans_chars str);

/**
 * Parses the number out of the given string in given base assuming and ignoring
 * comma thousand separators. The decimal point is a period.
 *
 * @param str  String to parse. Leading white space will be skipped.
 * @param endptr  (Optional) Points to the first character that did not parse properly
 * @param base a number between 2 and 36 or 0 for detecting hex with prefix 0x,
 *             octal with prefix 0 and else assuming decimal.
 * @param num Where the number result will be stored or 0 on error.
 * @return \ref RUE_OK on success \ref RUE_OVERFLOW when the number did not fit
 *         the type or \ref RUE_PARAMETER_NOT_SET and \ref RUE_INVALID_PARAMETER
 *         for not given or invalid input.
 */
RUAPI int32_t ruStrParseInt(trans_chars str, perm_chars* endptr, uint32_t base, int32_t* num);

/**
 * \brief Like \ref ruStrParseInt but ignores overflow and trailing garbage
 * @param str String to contain numerals to parse
 * @return the number
 */
RUAPI int32_t ruStrToInt(trans_chars str);

/**
 * Returns true if given haystack contains needle character
 * @param haystack string to search for needle
 * @param needle character to search in haystack
 * @return whether it is found
 */
RUAPI bool ruStrHasChar(trans_chars haystack, char needle);

/**
 * \brief Removes unwanted characters from instr.
 *
 * @param instr Input string to remove unwanted characters from.
 * @param unwanted Characters to remove or NULL for white space.
 * @param newstr Where a potential new copy of the given string will be stored.
 *               Caller must free.
 * @return instr if unmodified or *newstr if unwanted characters where found.
 */
RUAPI perm_chars ruStrStrip(perm_chars instr, trans_chars unwanted,
                            alloc_chars* newstr);

/**
 * Constants used by \ref ruStrTrim to indicate the trimming sides.
 */
enum ruTrimSide {
/**
 * Trim character off the beginning and the end of the given string.
 */
ruTrimBoth = (uint32_t)0,

/**
 * Trim character off the beginning of the given string.
 */
ruTrimStart = 1,

/**
 * Trim character off the end of the given string.
 */
ruTrimEnd = 2

};

/**
 * \brief Checks given string for leading / trailing unwanted characters and
 * returns trimmed copy if needed.
 *
 * @param instr String to check for trimming
 * @param unwanted String containing all unwanted character bytes or NULL to
 *                 trim whitespace.
 * @param ends Indicates which sides to trim
 * @param newstr Optional. Where the new string will be stored if it was created.
 *               If no trimming took place this will be NULL.
 *               This string should be freed by the caller.
 * @return instr if instr was NULL or did not need trimming,
 *         instr + trimmed start character if only the beginning needed trimming
 *         else the trimmed new string also set in newstr.
 *         newstr is to be freed by the caller.
 *
 * Example:
 * ~~~~~{.c}
    alloc_chars newstr = NULL;
    perm_chars trimstr = ruStrTrim(mystring, NULL, ruTrimBoth, &newstr);
    // do whatever with trimstr
    ruFree(newstr);
 * ~~~~~
 *
 */
RUAPI perm_chars ruStrTrim(perm_chars instr, trans_chars unwanted,
                           enum ruTrimSide ends, alloc_chars* newstr);

/**
 * \brief Returns the white space trimmed bounds of given string without modifying it.
 * @param inStart Start address of string to examine
 * @param inLen Number of bytes to examine or \ref RU_SIZE_AUTO
 * @param outLen Optional, where the length of the returned start address will be stored.
 * @return Start address of first non whitespace character or NULL
 */
RUAPI trans_chars ruStrTrimBounds(trans_chars inStart, rusize inLen, rusize* outLen);

/**
 * \brief Parses whitespace trimmed key / value regions based on delim out of
 * given input string.
 *
 * @param inStart Start of the string to parse
 * @param inLen Number of bytes to evaluate or \ref RU_SIZE_AUTO.
 * @param delim Delimiter sequence to split key / value on
 * @param keyStart Optional, where the start of the key region will be stored
 * @param keyLen Optional, where the length of the key region will be stored
 * @param valStart Optional, where the start of the value region will be stored
 * @param valLen Optional, where the length of the value region will be stored
 * @return true if delimiter was found. When false only the key parameters will
 *          be set.
 */
RUAPI bool ruStrFindKeyVal(trans_chars inStart, rusize inLen, trans_chars delim,
                           trans_chars* keyStart, rusize* keyLen,
                           trans_chars* valStart, rusize* valLen);

/**
 * \brief Checks whether given string has any content other than whitespace
 * @param str String to inspect
 * @return true if NULL empty or only white space
 */
RUAPI bool ruStrEmpty(trans_chars str);

/** @deprecated Use \ref ruStrCmp */
#define ruStrcmp ruStrCmp;

/** @deprecated Use \ref ruStrCaseCmp */
#define ruStrcasecmp ruStrCaseCmp

/** @deprecated Use \ref ruStrDup */
#define ruStrdup ruStrDup

/** @deprecated Use \ref ruStrNDup */
#define ruStrndup ruStrNDup
/** @deprecated Use \ref ruStrSplit */
#define ruStrsplit ruStrSplit

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
