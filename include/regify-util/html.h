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
 * \defgroup html HTML Utilities
 * \brief Functions for working with HTML content.
 *
 * This uses https://github.com/htacg/tidy-html5
 *
 * @{
 */

#ifndef REGIFY_UTIL_HTML_H
#define REGIFY_UTIL_HTML_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief Return HTML compatible version of given text without any HTML wrapper tags.
 *
 * This function simply replaces the following:
 *  Character Sequence | Encoding
 *  -------------------|-----------------
 *         &           |  &amp;amp;
 *         <           |  &amp;lt;
 *         >           |  &amp;gt;
 *      2 spaces       |  &amp;nbsp;&amp;nbsp;
 *         \r\\n       |  &lt;br/&gt;\\n
 *         \\n         |  &lt;br/&gt;\\n
 *         \r          |  &lt;br/&gt;\\n
 *
 * @param text Text to HTML encode
 * @return HTML encoded version of given text. Caller must free.
 */
RUAPI alloc_chars ruHtmlEncodeText(trans_chars text);

/**
 * \brief Sanitizes given HTML and optionally extracts plain text.
 *
 * It returns a sanitized HTML copy if excluded items were found and an htmlCopy
 * reference was given. If plainTxt reference was given it will store the
 * extracted plain text there. At least one of htmlCopy or plainTxt must be set.
 * @param html HTML buf to evaluate
 * @param htmlCopy Where a cleaned copy if needed will be stored. Caller must free
 * @param plainTxt Where a plain text copy will be stored. Caller must free.
 * @param excludeTags Optional Set of tags to filter out. Default:
 *      \arg \b applet
 *      \arg \b script
 *      \arg \b object
 *      \arg \b iframe
 *      \arg \b noframes
 *      \arg \b noscript
 * @param excludeAttrs Optional aet of attributes to filter out. Default:
 *      \arg \b onabort
 *      \arg \b onblur
 *      \arg \b onchange
 *      \arg \b onclick
 *      \arg \b ondblclick
 *      \arg \b onerror
 *      \arg \b onfocus
 *      \arg \b onkeydown
 *      \arg \b onkeypress
 *      \arg \b onkeyup
 *      \arg \b onload
 *      \arg \b onmousedown
 *      \arg \b onmousemove
 *      \arg \b onmouseout
 *      \arg \b onmouseover
 *      \arg \b onmouseup
 *      \arg \b onreset
 *      \arg \b onselect
 *      \arg \b onsubmit
 *      \arg \b onunload
 *      \arg \b javascript
 *      \arg \b eval
 *      \arg \b script
 * @return Either cleaned up html or original.
 */
RUAPI perm_chars ruHtmlSanitizeCustom(perm_chars html,
                                      alloc_chars* htmlCopy, alloc_chars* plainTxt,
                                      ruSet excludeTags, ruSet excludeAttrs);

/**
 * \brief Sanitizes given HTML and optionally extracts plain text.
 *
 * This runs \ref ruHtmlSanitizeCustom with the default sets.
 * @param html HTML buf to evaluate
 * @param htmlCopy Where a cleaned copy if needed will be stored. Caller must free
 * @param plainTxt Where a plain text copy will be stored. Caller must free.
 * @return Either cleaned up html or original.
 */
RUAPI perm_chars ruHtmlSanitize(perm_chars html, alloc_chars* htmlCopy,
                                alloc_chars* plainTxt);

/**
 * \brief Checks whether given buffer is HTML
 *
 * This function checks the first 200 characters for &lt;html and the last 20 for
 * &lt;/html>.
 * \param content Buffer to check
 * \return false if body is NULL or doesn't have html tags in it.
 */
RUAPI bool ruHtmlTestFor(trans_chars content);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_HTML_H
