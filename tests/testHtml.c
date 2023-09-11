/**
 * SMTP unit tests
 *
 * @copyright 2023 regify
 *
 * kate: mode C;
 */
#include "tests.h"

START_TEST (api) {
    perm_chars retText = "%s failed wanted ret 0x%p but got 0x%p";
    perm_chars test = "ruHtmlSanitizeCustom";
    perm_chars body = "foo";
    perm_chars res;
    alloc_chars cleaned = NULL;
    alloc_chars txt = NULL;

    res = ruHtmlSanitizeCustom(NULL, NULL, NULL,
                               NULL, NULL);
    fail_unless(NULL == res, retText, test, NULL, res);

    res = ruHtmlSanitizeCustom(body, NULL, NULL,
                               NULL, NULL);
    fail_unless(body == res, retText, test, body, res);

    res = ruHtmlSanitizeCustom(body, &cleaned, NULL,
                               NULL, NULL);
    fail_unless(body == res, retText, test, body, res);

    res = ruHtmlSanitizeCustom(NULL, NULL, &txt,
                               NULL, NULL);
    fail_unless(NULL == res, retText, test, NULL, res);

    res = ruHtmlSanitize(NULL, &cleaned, &txt);
    fail_unless(NULL == res, retText, test, NULL, res);
}
END_TEST

#ifdef _WIN32
#define LF "\r\n"
#define TIDY "Windows version 5.8.0"
#else
#define LF "\n"
#ifdef MAC_OS_X
#define TIDY "Apple macOS version 5.8.0"
#else
#define TIDY "Linux version 5.8.0"
#endif
#endif

START_TEST (run) {
    perm_chars retText = "%s failed wanted ret 0x%p but got 0x%p";
    perm_chars test = "ruHtmlSanitizeCustom";
    perm_chars body = "<p>foo<p>bar";
    perm_chars expstr = NULL;
    perm_chars res;
    alloc_chars cleaned = NULL;
    alloc_chars txt = NULL;

    res = ruHtmlSanitize(body, &cleaned, &txt);
    fail_unless(body == res, retText, test, body, res);
    fail_unless(NULL == cleaned, retText, test, NULL, cleaned);
    fail_if(NULL == txt, retText, test, NULL, txt);
    ruDbgLogf("plain text: '%s'", txt);
    expstr = "foo\n"
             "bar\n";
    ck_assert_str_eq(expstr, txt);
    ruFree(txt);

    body = "<!DOCTYPE html>\n"
           "<html>\n"
           "<head>\n"
           "<meta name=\"generator\" content=\"HTML Tidy for HTML5 for Linux version 5.6.0\" />\n"
           "<script>alert(\"hi\");</script>\n"
           "<meta charset=utf8>\n"
           "<title>Da title is here</title>\n"
           "</head>\n"
           "<body onsubmit=>\n"
           "<p onclick='huh'>foo &Ascr;   \n"
           "  <b>  is  </b>\n\n "
           "  the</p>\n"
           "<"
           "object"
           " onblur='s'/>"
           "<br onchange>"
           "<p>bar <i>man</p>\n"
           "<ol>\n"
           "<li>the&nbsp;&#xa0; &nbsp;first &#35;item </li>\n"
           "<ul>\n"
           "<li> &AMP item &AMP;2\n"
           "</body>\n"
           "</html>";
    expstr = "<!DOCTYPE html>" LF
             "<html xmlns=\"http://www.w3.org/1999/xhtml\">" LF
             "<head>" LF
             "  <meta charset=\"utf-8\" />" LF
             "  <meta name=\"generator\" content=\"HTML Tidy for HTML5 for " TIDY "\" />" LF
             "  <title>Da title is here</title>" LF
             "</head>" LF
             "<body>" LF
             "  <p>foo \xf0\x9d\x92\x9c <b>is</b> the</p><br />" LF
             "  <p>bar <i>man</i></p>" LF
             "  <ol>" LF
             "    <li>" LF
             "      <i>the&nbsp;&nbsp; &nbsp;first #item</i>" LF
             "      <ul>" LF
             "        <li><i>&amp; item &amp;2</i></li>" LF
             "      </ul>" LF
             "    </li>" LF
             "  </ol>" LF
             "</body>" LF
             "</html>" LF;
    res = ruHtmlSanitize(body, &cleaned, NULL);
    fail_if(body == res, retText, test, body, res);
    ruDbgLogf("cleaned html: '%s'", cleaned);
    ck_assert_str_eq(expstr, cleaned);
    ruFree(cleaned);

    res = ruHtmlSanitize(body, NULL, &txt);
    fail_unless(body == res, retText, test, body, res);
    ruDbgLogf("plain text: '%s'", txt);
    expstr = "foo \xf0\x9d\x92\x9c is the\n"
             "bar man\n"
             "* the    first #item\n"
             "  * & item &2\n";
    ck_assert_str_eq(expstr, txt);
    ruFree(txt);

    body = "<html>\n"
           "<body>\n"
           "<object/>\n"
           "<h1>Title 1</h1>\n"
           "<h2>Title 2</h2>\n"
           "<h6>Title 6</h6>\n"
           "<h7>Bogus Title</h7>\n"
           "<p>bar <b>man</b>\n"
           "</p>\n"
           "<li> the  first &#35;item </li>\n"
           "</body>\n"
           "</html>";
    res = ruHtmlSanitize(body, &cleaned, &txt);
    fail_if(body == res, retText, test, body, res);
    ruDbgLogf("cleaned html: '%s'", cleaned);
    expstr = "<!DOCTYPE html>" LF
             "<html xmlns=\"http://www.w3.org/1999/xhtml\">" LF
             "<head>" LF
             "  <meta charset=\"utf-8\" />" LF
             "  <meta name=\"generator\" content=\"HTML Tidy for HTML5 for " TIDY "\" />" LF
             "  <title></title>" LF
             "</head>" LF
             "<body>" LF
             "  <h1>Title 1</h1>" LF
             "  <h2>Title 2</h2>" LF
             "  <h6>Title 6</h6>Bogus Title" LF
             "  <p>bar <b>man</b></p>" LF
             "  <li>the first #item</li>" LF
             "</body>" LF
             "</html>" LF;
    ck_assert_str_eq(expstr, cleaned);
    ruDbgLogf("plain text: '%s'", txt);
    expstr = "Title 1\n"
             "Title 2\n"
             "Title 6\n"
             "Bogus Title\n"
             "bar man\n"
             "* the first #item\n";
    ck_assert_str_eq(expstr, txt);
    ruFree(cleaned);
    ruFree(txt);
}
END_TEST

START_TEST (encode) {
    perm_chars text;
    perm_chars expstr;
    alloc_chars html = NULL;
    alloc_chars ehtm = NULL;

    text = "testing<123>";
    html = ruHtmlEncodeText(text);
    expstr = "testing&lt;123&gt;";
    ruDbgLogf("encoded: '%s'", html);
    ck_assert_str_eq(expstr, html);
    ruFree(html);

    text = "| |\n|  |\r\n|   |\r|    |\n\r|     |";
    html = ruHtmlEncodeText(text);
    expstr = "| |<br/>\n"
             "|&nbsp;&nbsp;|<br/>\n"
             "|&nbsp;&nbsp; |<br/>\n"
             "|&nbsp;&nbsp;&nbsp;&nbsp;|<br/>\n"
             "<br/>\n"
             "|&nbsp;&nbsp;&nbsp;&nbsp; |";
    ruDbgLogf("encoded: '%s'", html);
    ck_assert_str_eq(expstr, html);
    ruFree(html);

    text = "test ing>1&2  3<";
    html = ruHtmlEncodeText(text);
    expstr = "test ing&gt;1&amp;2&nbsp;&nbsp;3&lt;";
    ruDbgLogf("encoded: '%s'", html);
    ck_assert_str_eq(expstr, html);
    ruFree(html);
}
END_TEST

TCase* htmlTests(void) {
    TCase *tcase = tcase_create("smtp");
    tcase_add_test(tcase, api);
    tcase_add_test(tcase, run);
    tcase_add_test(tcase, encode);
    return tcase;
}

