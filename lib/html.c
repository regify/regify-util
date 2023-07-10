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
 *
 * The code below is based on the code from the book "Mastering Algorithms
 * with C" by Kyle Loudon, published by O'Reilly & Associates.
 */
#include "lib.h"

//#define html_dbg(fmt, ...)
#define html_dbg(fmt, ...) ruLog_(RU_LOG_DBUG, fmt, __VA_ARGS__)

typedef struct parseCtx_ {
    TidyDoc doc;
    TidyBuffer pass1;
    TidyBuffer txt;
    ruMap excludeTags;
    ruMap excludeAttrs;
    // sanitize flags
    bool didExclude;
    // text flags
    bool lineStarted;
    int indent;
} parseCtx;

perm_chars INDENT = "  ";

#define tAppend(buf, str) tidyBufAppend(&(buf), (ptr)(str), (uint32_t)ruStrLen(str))

#define addKey(set, at) static perm_chars at = #at; \
    ruSetPut(set, at)

static void filterTags(parseCtx* pc, TidyNode tnod) {
    for (TidyNode child = tidyGetChild(tnod); child;
         child = tidyGetNext(child)) {
        perm_chars name = NULL;
        TidyNodeType nt = tidyNodeGetType(child);
        bool opened = false;
        bool hasText = tidyNodeHasText(pc->doc, child);
        if (nt == TidyNode_DocType) {
            tAppend(pc->pass1, "<!DOCTYPE html>\n");

        } else if(nt == TidyNode_Start || nt == TidyNode_End || nt == TidyNode_StartEnd) {
            name = tidyNodeGetName(child);
            if (ruMapHas(pc->excludeTags, name, NULL)) {
                ruDbgLogf("excluding '%s' tag", name);
                pc->didExclude = true;
                continue;
            }
            // if it has a name, then it's an HTML tag ...
            tAppend(pc->pass1, "<");
            tAppend(pc->pass1, name);
            opened = true;
            // walk the attribute list
            for (TidyAttr attr = tidyAttrFirst(child); attr;
                 attr = tidyAttrNext(attr)) {
                perm_chars atName = tidyAttrName(attr);
                if (ruStrEmpty(atName)) continue;
                if (ruMapHas(pc->excludeAttrs, atName, NULL)) {
                    ruDbgLogf("excluding '%s' attribute", atName);
                    pc->didExclude = true;
                } else {
                    tAppend(pc->pass1, " ");
                    tAppend(pc->pass1, atName);
                    if (tidyAttrValue(attr)) {
                        tAppend(pc->pass1, "=\"");
                        tAppend(pc->pass1, tidyAttrValue(attr));
                        tAppend(pc->pass1, "\"");
                    }
                }
            }
            tAppend(pc->pass1, ">\n");
            if (ruStrEquals(name, "head")) {
                // add our charset just in case
                tAppend(pc->pass1, "<meta charset=\"utf-8\" />\n");
            }

        } else if (hasText) {
            // if it does not have a name, then it's probably text, cdata, etc...
            tidyNodeGetText(pc->doc, child, &pc->pass1);
        }
        filterTags(pc, child); // recursive
        if (opened) {
            tAppend(pc->pass1, "</");
            tAppend(pc->pass1, name);
            tAppend(pc->pass1, ">\n");
        }
    }
}

#include "htmlEntities.c"

static void entityDecode(alloc_chars txt, rusize* len) {
    ruMap entities = getEntityMap();
    char* out = txt;
    char* end = txt + *len;
    for (char* in = txt; in < end; in++) {
        if (*in != '&') {
            *(out++) = *in;
            continue;
        }
        // we have in == &
        char* p = in + 1;
        if (*p == '#') {
            p++;
            // p is x or first decimal digit
            int64_t num = -1;
            perm_chars pastNum = NULL;
            if (*p == 'x') {
                ruStrParseInt64(p, &pastNum, 16, &num);
            } else if (*p >= '0' && *p <= '9') {
                ruStrParseInt64(p, &pastNum, 10, &num);
            }
            if (*pastNum != ';' || num < 0 || num > 0xfffd) {
                // not an entity
                // or out of range
                *(out++) = *in++;   // &
                *(out++) = *in;     // #
                continue;
            }
            // parse the numeric entity
            perm_uni wc[] = {(perm_uni) (intptr_t)num, 0};
            alloc_chars utf = ruStrFromUtf16(wc[0]);
            for(char* i = utf; *i; i++) *(out++) = *i;
            ruFree(utf);
            in = (char*)pastNum;   // ;
            continue;
        }
        bool replaced = false;
        for (int i = MAX_ENTITY_LEN; i >= 0; i--, p++) {
            if (*p == ';' && i < MAX_ENTITY_LEN) {
                // end of entity
                char name[MAX_ENTITY_LEN+1];
                // 0   1  2
                // in     p
                // &   a  ;
                rusize namelen = p - in - 1;
                memcpy(&name[0], in+1, namelen);
                name[namelen] = '\0';
                alloc_chars bytes = NULL;
                ruMapGet(entities, &name[0], &bytes);
                if (bytes) {
                    // success
                    while(*bytes) *(out++) = *bytes++;
                    in = p;
                    replaced = true;
                    break;
                }
            } else if ((*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9') ||
                       (*p >= 'A' && *p <= 'Z')) {
                // legal characters
                continue;
            }
        }
        if (!replaced) {
            // i guess just an ampersand
            *(out++) = *in;
        }
    }
    *len = out - txt;
}

static void filterText(parseCtx* pc, TidyNode tnod) {
    static ruSet lfTtags = NULL;
    if (!lfTtags) {
        lfTtags = ruSetNew(ruTypeStrRef());
        addKey(lfTtags, br);
        addKey(lfTtags, div);
        addKey(lfTtags, h1);
        addKey(lfTtags, h2);
        addKey(lfTtags, h3);
        addKey(lfTtags, h4);
        addKey(lfTtags, h5);
        addKey(lfTtags, h6);
        addKey(lfTtags, hr);
        addKey(lfTtags, ol);
        addKey(lfTtags, p);
        addKey(lfTtags, tr);
        addKey(lfTtags, ul);
    }
    for (TidyNode child = tidyGetChild(tnod); child;
         child = tidyGetNext(child)) {
        perm_chars name = NULL;
        TidyNodeType nt = tidyNodeGetType(child);
        bool opened = false;
        bool hasText = tidyNodeHasText(pc->doc, child);
        if(nt == TidyNode_Start || nt == TidyNode_End || nt == TidyNode_StartEnd) {
            name = tidyNodeGetName(child);
            if (ruMapHas(lfTtags, name, NULL)) {
                // if it has a name, then it's an HTML tag ...
                html_dbg("start %s break", name);
                if (pc->lineStarted) tAppend(pc->txt, "\n");
                if (ruStrEquals("ul", name) || ruStrEquals("ol", name)) {
                    pc->indent++;
                }
                pc->lineStarted = false;
                opened = true;
            } else if (ruStrEquals("li", name)) {
                if (!pc->lineStarted) {
                    pc->lineStarted = true;
                    for (int id = pc->indent; id > 0; id--) {
                        tAppend(pc->txt, INDENT);
                    }
                }
                tAppend(pc->txt, "*");
                opened = true;
            }


        } else if (hasText) {
            // if it does not have a name, then it's probably text, cdata, etc...
            TidyBuffer buf;
            tidyBufInit(&buf);
            tidyNodeGetText(pc->doc, child, &buf);
            rusize end = 0;
            perm_chars start = ruStrTrimBounds((trans_chars)buf.bp,
                                               buf.size, &end);
            entityDecode((alloc_chars)start, &end);
            if (end) {
                if (pc->lineStarted) {
                    tAppend(pc->txt, " ");
                } else {
                    pc->lineStarted = true;
                    for (int id = pc->indent; id > 0; id--) tAppend(pc->txt, INDENT);
                }
                tidyBufAppend(&pc->txt, (ptr)start, (uint32_t)end);
            }
            tidyBufFree(&buf);
        }
        filterText(pc, child); // recursive
        if (opened && pc->lineStarted) {
            html_dbg("end %s break", name);
            if (ruStrEquals("ul", name) || ruStrEquals("ol", name)) {
                pc->indent--;
            }
            tAppend(pc->txt, "\n");
            pc->lineStarted = false;
        }
    }
}

static void startFilterText(parseCtx* pc, TidyNode tnod) {
    pc->lineStarted = false;
    pc->indent = -1;
    filterText(pc, tnod);
}

static ruSet excludeTagSet(void) {
    static ruSet tags = NULL;
    if (!tags) {
        tags = ruSetNew(ruTypeStrRef());
        addKey(tags, applet);
        addKey(tags, script);
        addKey(tags, object);
        addKey(tags, iframe);
        addKey(tags, noframes);
        addKey(tags, noscript);
    }
    return tags;
}

static ruSet excludeAttrSet(void) {
    static ruSet attrs = NULL;
    if (!attrs) {
        attrs = ruSetNew(ruTypeStrRef());
        addKey(attrs, onabort);
        addKey(attrs, onblur);
        addKey(attrs, onchange);
        addKey(attrs, onclick);
        addKey(attrs, ondblclick);
        addKey(attrs, onerror);
        addKey(attrs, onfocus);
        addKey(attrs, onkeydown);
        addKey(attrs, onkeypress);
        addKey(attrs, onkeyup);
        addKey(attrs, onload);
        addKey(attrs, onmousedown);
        addKey(attrs, onmousemove);
        addKey(attrs, onmouseout);
        addKey(attrs, onmouseover);
        addKey(attrs, onmouseup);
        addKey(attrs, onreset);
        addKey(attrs, onselect);
        addKey(attrs, onsubmit);
        addKey(attrs, onunload);
        addKey(attrs, javascript);
        addKey(attrs, eval);
        addKey(attrs, script);
    }
    return attrs;
}

RUAPI alloc_chars ruHtmlEncodeText(trans_chars text) {
    static ruCleaner rc = NULL;
    if (!rc) {
        rc = ruCleanNew(0);
        ruCleanAdd(rc, "&", "&amp;");
        ruCleanAdd(rc, "<", "&lt;");
        ruCleanAdd(rc, ">", "&gt;");
        ruCleanAdd(rc, "  ", "&nbsp;&nbsp;");
        ruCleanAdd(rc, "\r\n", "<br/>\n");
        ruCleanAdd(rc, "\n", "<br/>\n");
        ruCleanAdd(rc, "\r", "<br/>\n");
    }
    ruString out = NULL;
    ruCleanToString(rc, text, 0, &out);
    alloc_chars res = ruStringGetCString(out);
    ruStringFree(out, true);
    return res;
}

RUAPI perm_chars ruHtmlSanitizeCustom(perm_chars html,
                                      alloc_chars* htmlCopy, alloc_chars* plainTxt,
                                      ruSet excludeTags, ruSet excludeAttrs) {
    perm_chars out = html;
    if (!html) {
        ruDbgLog("No input no need to clean");
        return out;
    }

    if (!htmlCopy && !plainTxt) {
        ruDbgLog("No output references no need to clean");
        return out;
    }

    ruZeroedStruct(parseCtx, pctx);
    parseCtx* pc = &pctx;
    if (excludeTags) {
        pc->excludeTags = excludeTags;
    } else {
        pc->excludeTags = excludeTagSet();
    }
    if (excludeAttrs) {
        pc->excludeAttrs = excludeAttrs;
    } else {
        pc->excludeAttrs = excludeAttrSet();
    }

    // resources
    TidyBuffer errbuf = {0};
    tidyBufInit(&errbuf);
    TidyBuffer inbuf = {0};
    tidyBufInit(&inbuf);
    pc->doc = tidyCreate();
    tidyBufInit(&pc->pass1);

    tAppend(inbuf, html);

    tidyOptSetBool(pc->doc, TidyForceOutput, yes);
    tidyOptSetBool(pc->doc, TidyXhtmlOut, yes);
    tidyOptSetBool(pc->doc, TidyMuteShow, yes);
    tidyOptSetInt(pc->doc, TidyWrapLen, 255);
    tidySetErrorBuffer(pc->doc, &errbuf);

    html_dbg("checking input html: '%s'", html);
    int err = tidyParseBuffer(pc->doc, &inbuf); /* parse the input */
    //html_dbg("First parse error: %d", err);
    if (err >= 0) {
        err = tidyCleanAndRepair(pc->doc); /* fix any problems */
        if (err >= 0) {
            err = tidyRunDiagnostics(pc->doc); /* load tidy error buffer */
            if (err >= 0) {
                // walk the tree and filter
                filterTags(pc, tidyGetRoot(pc->doc));
                /* show errors */
                html_dbg("First parse errors: '%s'", errbuf.bp);
            }
        }
    }

    // clean-up
    tidyBufFree(&inbuf);
    tidyBufFree(&errbuf);
    tidyRelease(pc->doc);

    if(!pc->didExclude && ruDoesLog(RU_LOG_VERB)) {
        html_dbg("cleaned tidy: '%s'", pc->pass1.bp);
    }

    if (pc->didExclude || plainTxt) {
        // resources
        tidyBufInit(&errbuf);
        pc->doc = tidyCreate();

        // do an output pass
        tidyOptSetBool(pc->doc, TidyForceOutput, yes);
        tidyOptSetBool(pc->doc, TidyXhtmlOut, yes);
        //tidyOptSetValue(pc->doc, TidyMuteReports, "BLANK_TITLE_ELEMENT");
        tidyOptSetInt(pc->doc, TidyIndentContent, 2);
        tidyOptSetInt(pc->doc, TidyWrapLen, 255);
        tidySetErrorBuffer(pc->doc, &errbuf);
        err = tidyParseBuffer(pc->doc, &pc->pass1);
        html_dbg("Second parse error: %d", err);
        if (err >= 0) {
            err = tidyCleanAndRepair(pc->doc); /* fix any problems */
            if (err >= 0) {
                err = tidyRunDiagnostics(pc->doc); /* load tidy error buffer */
                if (err >= 0) {
                    // walk the body tree to extract text
                    if (plainTxt) {
                        ruDbgLog("providing plain text");
                        tidyBufInit(&pc->txt);
                        startFilterText(pc, tidyGetBody(pc->doc));
                        html_dbg("Extracted text: '%s'", pc->txt.bp);
                        *plainTxt = ruStrNDup((alloc_chars)pc->txt.bp, pc->txt.size);
                        tidyBufFree(&pc->txt);
                    }
                    if (pc->didExclude && htmlCopy) {
                        uint outLen = 0;
                        tidySaveString(pc->doc, NULL, &outLen);
                        alloc_chars outbuf = ruMalloc0(outLen+1, char);
                        err = tidySaveString(pc->doc, outbuf, &outLen);
                        if (err < 2) {
                            *(outbuf+outLen) = '\0';
                            html_dbg("Final output[%ld]: '%s'", outLen, outbuf);
                            *htmlCopy = outbuf;
                            out = outbuf;
                            outbuf = NULL;
                        } else {
                            ruCritLogf("Failed saving output: %d", err);
                        }
                        ruFree(outbuf);
                    }
                    /* show errors */
                    html_dbg("Second parse errors: '%s'", errbuf.bp);
                }
            }
        }
        tidyBufFree(&errbuf);
        tidyRelease(pc->doc);
    }
    tidyBufFree(&pc->pass1);
    if (out == html) {
        ruDbgLog("return unchanged");
    } else {
        ruDbgLog("return cleaned html");
    }
    html_dbg("Returning: '%s'", out);
    return out;
}

RUAPI perm_chars ruHtmlSanitize(perm_chars html, alloc_chars* htmlCopy,
                                alloc_chars* plainTxt) {
    return ruHtmlSanitizeCustom(html, htmlCopy, plainTxt,
                                NULL, NULL);
}

RUAPI bool ruHtmlTestFor(trans_chars content) {
    if (!content) return false;

    rusize bodyLen = strlen(content);
    rusize endLen = 20;
    rusize startLen = bodyLen > 200? 200: bodyLen;
    perm_chars end = content + bodyLen - endLen;
    if (end < content) {
        end = content;
        endLen = bodyLen;
    }

    if (ruStrCaseStrLen(content, "<html", startLen) &&
        ruStrCaseStrLen(end, "</html>", endLen)) {
        /* say it is html only, if there is "<html" found in the first
            * 200 chars and </html> in the last 20. */
        return true;
    }
    return false;
}
