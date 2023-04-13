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
 * Original Source is
 * inih -- simple .INI file parser
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (C) 2009-2020, Ben Hoyt
 *
 * inih is released under the New BSD license (see LICENSE.txt). Go to the project
 * home page for more info:
 *
 * https://github.com/benhoyt/inih
 *
*/
#include "lib.h"

ruMakeTypeGetter(Ini, MagicIni)

/* Chars that begin a start-of-line comment. Per Python configparser, allow
   both ; and # comments at the start of a line by default. */
#define INI_START_COMMENT_PREFIXES ";#"

/* Nonzero to allow inline comments (with valid inline comment characters
   specified by INI_INLINE_COMMENT_PREFIXES). Set to 0 to turn off and match
   Python 3.2+ configparser behaviour. */
#define INI_INLINE_COMMENT_PREFIXES ";"

/* Initial size in bytes for heap line buffer. */
#define INI_INITIAL_ALLOC 8192

/* Stop parsing on first error (default is to keep parsing). */
#define INI_STOP_ON_FIRST_ERROR 0

#define MAX_SECTION 50

/* Typedef for prototype of fgets-style reader function. */
typedef char* (*ini_reader)(char* str, int num, void* stream);

/* Used by ini_parse_string() to keep track of string parsing state. */
typedef struct {
    const char* ptr;
    size_t num_left;
} ini_parse_string_ctx;

/* Return pointer to first non-whitespace char in given string. */
static char* lskip(const char* s) {
    while (*s && isspace((unsigned char) (*s))) s++;
    return (char*) s;
}

/* Return pointer to first char (of chars) or inline comment in given string,
   or pointer to NUL at end of string if neither found. Inline comment must
   be prefixed by a whitespace character to register as a comment. */
static char* find_chars_or_comment(perm_chars s, trans_chars chars) {
    int was_space = 0;
    while (*s && (!chars || !strchr(chars, *s)) &&
           !(was_space && strchr(INI_INLINE_COMMENT_PREFIXES, *s))) {
        was_space = isspace((unsigned char) (*s));
        s++;
    }
    return (char*) s;
}

static int ini_parse_stream(ini_reader reader, void* stream, ruIniCallback handler,
                     void* user) {
    /* Uses a fair bit of stack (use heap instead if you need to) */
    size_t max_line = INI_INITIAL_ALLOC;
    char* section = NULL;
    char* prev_name = NULL;

    char* start;
    char* end;
    char* name;
    char* value;
    int lineno = 0;
    int errLineNo = 0;

    char* line = ruMalloc0(INI_INITIAL_ALLOC, char);

    /* Scan through stream line by line */
    while (reader(line, (int) max_line, stream) != NULL) {
        lineno++;
        start = line;
        if (lineno == 1) {
            if ((uint8_t) start[0] == 0xEF && (uint8_t) start[1] == 0xBB
                    && (uint8_t) start[2] == 0xBF) {
                // cope with UTF-8 BOM
                start += 3;
            }
        }
        start = lskip(ruStrTrim(start));

        if (strchr(INI_START_COMMENT_PREFIXES, *start)) {
            /* Start-of-line comment */
            continue;
        }

        if (prev_name && *start && start > line) {
            //#if INI_ALLOW_INLINE_COMMENTS
            end = find_chars_or_comment(start, NULL);
            if (*end) *end = '\0';
            ruStrTrim(start);
            //#endif
            /* Non-blank line with leading whitespace, treat as continuation
               of previous name's value (as per Python configparser). */
            if (!handler(user, section, prev_name, start, lineno) && !errLineNo) errLineNo = lineno;

        } else if (*start == '[') {
            /* A "[section]" line */
            end = find_chars_or_comment(start + 1, "]");
            if (*end == ']') {
                *end = '\0';
                ruFree(section);
                section = ruStrDup(start + 1);
                //*prev_name = '\0';
                ruFree(prev_name);
                if (!handler(user, section, NULL, NULL, lineno) && !errLineNo) errLineNo = lineno;
            } else if (!errLineNo) {
                /* No ']' found on section line */
                errLineNo = lineno;
            }

        } else if (*start) {
            /* Not a comment, must be a name[=:]value pair */
            end = find_chars_or_comment(start, "=:");
            if (*end == '=' || *end == ':') {
                *end = '\0';
                name = ruStrTrim(start);
                value = end + 1;
                //#if INI_ALLOW_INLINE_COMMENTS
                end = find_chars_or_comment(value, NULL);
                if (*end) *end = '\0';
                //#endif
                value = lskip(value);
                ruStrTrim(value);

                /* Valid name[=:]value pair found, call handler */
                ruFree(prev_name);
                prev_name = ruStrDup(name);
                //strncpy0(prev_name, name, sizeof(prev_name));
                if (!handler(user, section, name, value, lineno) && !errLineNo) errLineNo = lineno;
            } else if (!errLineNo) {
                /* No '=' or ':' found on name[=:]value line */
                *end = '\0';
                name = ruStrTrim(start);
                if (!handler(user, section, name, NULL, lineno)) errLineNo = lineno;
            }
        }

#if INI_STOP_ON_FIRST_ERROR
        if (error)
            break;
#endif
    }

    ruFree(line);
    ruFree(section);
    ruFree(prev_name);

    return errLineNo;
}

/* Same as ini_parse(), but takes a FILE* instead of filename. This doesn't
   close the file when it's finished -- the caller must do that. */
static int ini_parse_file(FILE* file, ruIniCallback handler, void* user) {
    return ini_parse_stream((ini_reader) fgets, file, handler, user);
}

/* An ini_reader function to read the next line from a string buffer. This
   is the fgets() equivalent used by ini_parse_string(). */
static char* ini_reader_string(char* str, int num, void* stream) {
    ini_parse_string_ctx* ctx = (ini_parse_string_ctx*) stream;
    const char* ctx_ptr = ctx->ptr;
    size_t ctx_num_left = ctx->num_left;
    char* strp = str;
    char c;

    if (ctx_num_left == 0 || num < 2) return NULL;

    while (num > 1 && ctx_num_left != 0) {
        c = *ctx_ptr++;
        ctx_num_left--;
        *strp++ = c;
        if (c == '\n') break;
        num--;
    }

    *strp = '\0';
    ctx->ptr = ctx_ptr;
    ctx->num_left = ctx_num_left;
    return str;
}

int32_t iniParseString(trans_chars string, ruIniCallback handler, void* user) {
    ruClearError();
    ini_parse_string_ctx ctx;
    ctx.ptr = string;
    ctx.num_left = strlen(string);
    int errLineNo = ini_parse_stream((ini_reader) ini_reader_string,
                                     &ctx, handler, user);
    if (errLineNo) {
        ruSetError("Failed parsing ini file content, error on line %d",
                   errLineNo);
        return RUE_GENERAL;
    }
    return RUE_OK;
}

static ruMap getIniMap(Ini* ini, trans_chars section) {
    ruMap map = ini->keys;
    if (section) {
        if (ruMapHas(ini->sections, section, NULL)) {
            ruMapGet(ini->sections, section, &map);
        } else {
            map = ruMapNewString(free, free);
            ruMapPut(ini->sections, ruStrDup(section), map);
        }
    }
    return map;
}

static bool iniParseCb(void* user, trans_chars section, trans_chars name,
                       trans_chars value, int lineno) {
    Ini* ctx = IniGet(user, NULL);
    if (!ctx) return true;

    ruMap map = getIniMap(ctx, section);
    if (name) {
        char* val = NULL;
        if (ruMapHas(map, name, NULL)) {
            ruMapGet(map, name, &val);
            if (val || value) {
                // append new value
                char *newval = ruDupPrintf("%s%s", val?val:"", value?value:"");
                ruMapPut(map, ruStrDup(name), newval);
            }
        } else {
            ruMapPut(map, ruStrDup(name), ruStrDup(value));
        }
    }
    return true;
}

Ini* iniNew(void) {
    Ini* i = ruMalloc0(1, Ini);
    i->type = MagicIni;
    i->keys = ruMapNewString(free, free);
    i->sections = ruMapNewString(free, (ruFreeFunc)ruMapFree);
    return i;
}

RUAPI ruIni ruIniNew(void) {
    return (ruIni)iniNew();
}

Ini* iniFree(Ini* i) {
    if (!i) return NULL;
    i->keys = ruMapFree(i->keys);
    i->sections = ruMapFree(i->sections);
    i->type = 0;
    ruFree(i);
    return NULL;
}

RUAPI ruIni ruIniFree(ruIni ini) {
    return (ruIni) iniFree(IniGet(ini, NULL));
}

static bool dumpMap(FILE* file, ruMap map) {
    bool used = false;
    int32_t ret;
    char* key, * val;
    for (ret = ruMapFirst(map, &key, &val); ret == RUE_OK;
         ret = ruMapNext(map, &key, &val)) {
        if (val) {
            fprintf(file, "%s = %s\n", key, val);
        } else {
            // should never happen since NULL means remove key
            fprintf(file, "%s\n", key);
        }
        used = true;
    }
    return used;
}

RUAPI int32_t ruIniWrite(ruIni iniOb, trans_chars filename) {
    int32_t ret;
    Ini* ini = IniGet(iniOb, &ret);
    if (!ini) return ret;
    FILE* file = ruFOpen(filename, "w", &ret);
    if (!file) return ret;
    bool used = dumpMap(file, ini->keys);
    char *section;
    ruMap keys;
    for (ret = ruMapFirst(ini->sections, &section, &keys); ret == RUE_OK;
        ret = ruMapNext(ini->sections, &section, &keys)) {
        if (used) fprintf(file, "\n");
        fprintf(file, "[%s]\n", section);
        used |= dumpMap(file, keys);
    }
    if (ret == RUE_FILE_NOT_FOUND) ret = RUE_OK;
    fclose(file);
    return ret;
}

RUAPI int32_t ruIniRead(trans_chars filename, ruIni* iniOb) {
//    ruClearError(); // already done by ruFOpen
    if (!filename || !iniOb) return RUE_PARAMETER_NOT_SET;
    int32_t code = RUE_OK;
    FILE* file = ruFOpen(filename, "r", &code);
    if (!file) return code;
    Ini* ini = iniNew();
    int errLineNo = ini_parse_file(file, iniParseCb, ini);
    fclose(file);
    if (errLineNo) {
        ruSetError("Failed parsing ini file '%s' error on line %d",
                   filename, errLineNo);
        code = RUE_GENERAL;
        ini = iniFree(ini);
    }
    *iniOb = ini;
    return code;
}

RUAPI int32_t ruIniKeys(ruIni iniOb, trans_chars section, ruList* keys) {
    int32_t ret;
    Ini* ini = IniGet(iniOb, &ret);
    if (!ini) return ret;
    if (!keys) return RUE_PARAMETER_NOT_SET;

    ruMap map = getIniMap(ini, section);
    return ruMapKeyList(map, keys, (ruCloneFunc) ruStrDup, free);
}

RUAPI int32_t ruIniSections(ruIni iniOb, ruList* sections) {
    int32_t ret;
    Ini* ini = IniGet(iniOb, &ret);
    if (!ini) return ret;
    if (!sections) return RUE_PARAMETER_NOT_SET;

    ruMap map = ini->sections;
    return ruMapKeyList(map, sections, (ruCloneFunc) ruStrDup, free);
}

RUAPI perm_chars ruIniGetDef(ruIni iniOb, trans_chars section, trans_chars key,
                             perm_chars def, int32_t* code) {
    int32_t ret;
    Ini* ini = IniGet(iniOb, &ret);

    if (!ini) ruRetWithCode(code, ret, NULL);
    if (!key) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL);

    ruMap map = ini->keys;
    if (section) {
        ret = ruMapGet(ini->sections, section, &map);
        if (ret != RUE_OK && !def) ruRetWithCode(code, ret, NULL);
    }
    const char* value = NULL;
    ret = ruMapGet(map, key, &value);
    if (def) {
        if (!value || ruStrCmp(value, "") == 0) {
            value = def;
            ret = RUE_OK;
        }
    }
    ruRetWithCode(code, ret, value);
}

RUAPI int32_t ruIniGet(ruIni iniOb, trans_chars section, trans_chars key, perm_chars* value) {
    if (!value) return RUE_PARAMETER_NOT_SET;
    int32_t ret;
    *value = ruIniGetDef(iniOb, section, key, NULL, &ret);
    return ret;
}

RUAPI int32_t ruIniSet(ruIni iniOb, trans_chars section, trans_chars key, trans_chars value) {
    int32_t ret;
    Ini* ini = IniGet(iniOb, &ret);
    if (!ini) return ret;
    if (!section && !key) return RUE_PARAMETER_NOT_SET;

    ruMap map = getIniMap(ini, section);
    if (key) {
        if (value) {
            ret = ruMapPut(map, ruStrDup(key), ruStrDup(value));
        } else {
            ret = ruMapRemove(map, key, NULL);
        }
    }
    return ret;
}
