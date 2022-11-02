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
 * \file
 * The file regex.h contains... 
 *
 */

#ifndef REGIFY_UTIL_REGEX_H
#define REGIFY_UTIL_REGEX_H

/* Only need to export C interface if used by C++ source code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * \defgroup regex Regular Expressions
 * \brief These functions facilitate regular expression functionality as it is provided
 * by ICU http://site.icu-project.org/
 *
 * Example:
 * ~~~~~{.c}
 * int32_t exp = RUE_OK;
 * ruRegex rr = NULL;
 * ruList matches = NULL;
 * do {
 *   rr = ruRegexNew("^(\\w+)^", RUREGEX_CASE_INSENSITIVE, &ret);
 *   if (ret != RUE_OK) break;
 *   char *orig = "text with a ^tag^ in it.";
 *   matches = ruRegexFindGroups(rr, orig, &ret);
 *   if (ret != RUE_OK) break;
 *   char *entry = ruListIdxData(matches, 1, char*, &ret);
 *   if (ret != RUE_OK) break;
 *   printf("group 1: %s", entry);
 * } while(0);
 * // clean up
 * if (matches) ruListFree(matches);
 * if (rr) ruRegexFree(rr);
 * ~~~~~
 *
 * @{
*/
/**
 * \brief Constants for Regular Expression Match Modes.
 *
 * These are mirrored from ICU to avoid having to pull the entire includes
 * folders when using regify-utils.
 */
typedef enum ruRegexFlag_ {

    /** Forces normalization of pattern and strings.
    Not implemented yet, just a placeholder, hence draft. */
    RUREGEX_CANON_EQ         = 128,
    /**  Enable case insensitive matching. */
    RUREGEX_CASE_INSENSITIVE = 2,

    /**  Allow white space and comments within patterns */
    RUREGEX_COMMENTS         = 4,

    /**  If set, '.' matches line terminators,
     * otherwise '.' matching stops at line end. */
    RUREGEX_DOTALL           = 32,

    /**  If set, treat the entire pattern as a literal string.
      *  Metacharacters or escape sequences in the input sequence will be given
      *  no special meaning.
      *
      *  The flag RUREGEX_CASE_INSENSITIVE retains its impact
      *  on matching when used in conjunction with this flag.
      *  The other flags become superfluous.
      */
    RUREGEX_LITERAL = 16,

    /**   Control behavior of "$" and "^"
      *    If set, recognize line terminators within string,
      *    otherwise, match only at start and end of input string.
      */
    RUREGEX_MULTILINE        = 8,

    /**   Unix-only line endings.
      *   When this mode is enabled, only \\u000a is recognized as a line ending
      *    in the behavior of ., ^, and $.
      */
    RUREGEX_UNIX_LINES = 1,

    /**  Unicode word boundaries.
      *     If set, \b uses the Unicode TR 29 definition of word boundaries.
      *     Warning: Unicode word boundaries are quite different from
      *     traditional regular expression word boundaries.  See
      *     http://unicode.org/reports/tr29/#Word_Boundaries
      */
    RUREGEX_UWORD            = 256,

    /**  Error on Unrecognized backslash escapes.
      *     If set, fail with an error on patterns that contain
      *     backslash-escaped ASCII letters without a known special
      *     meaning.  If this flag is not set, these
      *     escaped letters represent themselves.
      */
    RUREGEX_ERROR_ON_UNKNOWN_ESCAPES = 512

} ruRegexFlag;

/**
 * \brief Opaque pointer to regular expression object.
 */
typedef void* ruRegex;

/**
 * \brief Frees up the resources of the given \ref ruRegex object.
 * @param rr object to free.
 */
RUAPI ruRegex ruRegexFree(ruRegex rr);

/**
 * \brief Creates a new regular expression object to be used with ruRegex(Replace|Match|...)
 * free with \ref ruRegexFree after use.
 * @param pattern The pattern representing the regular expression without delimiters.
 * @param flags Flags influencing the behavior of the expression. These are from
 *              ICU and we provide \ref ruRegexFlag.
 * @param code (Optional) Where the return result of this operation such as
 *             \ref RUE_OK on success will be stored.
 * @return The newly created ruRegex object or NULL on failure.
 */
RUAPI ruRegex ruRegexNew(const char* pattern, ruRegexFlag flags, int32_t* code);

/**
 * \brief Replace the found expression instances in original with the content
 * found in replacement and return the result.
 * @param rr The \ref ruRegex object to use.
 * @param original The source string to replace
 * @param replacement The replacement string for found matches in original.
 * @param code (Optional) Where the return result of this operation such as
 *             \ref RUE_OK on success will be stored.
 * @return A newly allocated string that results out of the replacement.
 *         Free with \ref ruFree.
 */
RUAPI char* ruRegexReplace(ruRegex rr, const char* original,
                           const char* replacement, int32_t* code);

/**
 * \brief Indicates whether the given original matches the entire expression in rr.
 * @param rr The \ref ruRegex object to use.
 * @param original The source string to match.
 * @param code (Optional) Where the return result of this operation such as
 *             \ref RUE_OK on success will be stored.
 * @return true or false depending on whether there was a match.
 */
RUAPI bool ruRegexMatch(ruRegex rr, const char* original, int32_t* code);

/**
 * \brief Indicates whether the given original matches the part of the expression in rr.
 * @param rr The \ref ruRegex object to use.
 * @param original The source string to match.
 * @param code (Optional) Where the return result of this operation such as
 *             \ref RUE_OK on success will be stored.
 * @return true or false depending on whether there was a match.
 */
RUAPI bool ruRegexFind(ruRegex rr, const char* original, int32_t* code);

/**
 * \brief Returns the matches found in the given original if it matched the entire expression in rr.
 * @param rr The \ref ruRegex object to use.
 * @param original The source string to match.
 * @param code (Optional) Where the return result of this operation such as
 *             \ref RUE_OK on success will be stored.
 * @return List of matches or NULL if there was no match. List must be freed after use.
 */
RUAPI ruList ruRegexMatchGroups(ruRegex rr, const char* original, int32_t* code);

/**
 * \brief Returns the matches found in the given original if it matched part of the expression in rr.
 * @param rr The \ref ruRegex object to use.
 * @param original The source string to match.
 * @param code (Optional) Where the return result of this operation such as
 *             \ref RUE_OK on success will be stored.
 * @return List of matches or NULL if there was no match. List must be freed after use.
 */
RUAPI ruList ruRegexFindGroups(ruRegex rr, const char* original, int32_t* code);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_REGEX_H
