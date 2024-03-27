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

//<editor-fold desc="internal">
ruMakeTypeGetter(json, MagicJson)

// differs between jsonGet and jsonSet
static json* jsonGetWhat(ptr o, int32_t* code, bool setter) {
    int32_t ret = RUE_GENERAL;
    json* js = jsonGet(o, &ret);
    if (ret == RUE_OK) {
        if (setter) {
            if (!js->g) ret = RUE_INVALID_PARAMETER;
        } else {
            if (!js->node) ret = RUE_INVALID_PARAMETER;
        }
    }
    if (code) *code = ret;
    return js;
}

static json* jsonGetGet(ptr o, int32_t* code) {
    return jsonGetWhat(o, code, false);
}

static json* jsonGetSet(ptr o, int32_t* code) {
    return jsonGetWhat(o, code, true);
}

static yajl_val getYajlVal(ruJson rj, int32_t* status) {
    yajl_val node = NULL;
    if (!rj) ruRetWithCode(status, RUE_PARAMETER_NOT_SET, node);
    json* j = jsonGetGet(rj, status);
    if (j) {
        // top level node
        node = j->node;
    } else {
        // something returned by ruJsonKeyArray
        node = (yajl_val)rj;
    }
    return node;
}

static int64_t parseNum(yajl_val v, int32_t* status) {
    int64_t out = 0;
    if (YAJL_IS_NUMBER(v)) {
        out = YAJL_GET_INTEGER(v);
        ruVerbLogf("parsed %" PRId64 , out);
        ruRetWithCode(status, RUE_OK, out);
    } else if(YAJL_IS_STRING(v)) {
        perm_chars str = YAJL_GET_STRING(v);
        out = ruStrToInt64(str);
        ruVerbLogf("parsed '%s' to %" PRId64, str, out);
        ruRetWithCode(status, RUE_OK, out);
    } else {
        ruVerbLogf("got invalid type %d", v->type);
        ruRetWithCode(status, RUE_FILE_NOT_FOUND, out);
    }
}

static alloc_chars toString(yajl_val v, int32_t* status) {
    alloc_chars out = NULL;
    if(YAJL_IS_INTEGER(v)) {
        out = ruDupPrintf("%ld", YAJL_GET_INTEGER(v));
        ruVerbLogf("returning integer '%s'", out);
    } else if (YAJL_IS_DOUBLE(v)) {
        out = ruDupPrintf("%f", YAJL_GET_DOUBLE(v));
        ruVerbLogf("returning float '%s'", out);
    } else if (YAJL_IS_TRUE(v)) {
        out = ruStrDup("true");
        ruVerbLogf("returning boolean '%s'", out);
    } else if (YAJL_IS_FALSE(v)) {
        out = ruStrDup("false");
        ruVerbLogf("returning boolean '%s'", out);
    } else if(YAJL_IS_STRING(v)) {
        out = ruStrDup(YAJL_GET_STRING(v));
        ruVerbLogf("returning string '%s'", out);
    } else if (YAJL_IS_NULL(v)) {
        ruVerbLog("returning NULL");
    } else {
        ruVerbLog("returning nothing for complex types");
        ruRetWithCode(status, RUE_INVALID_PARAMETER, out);
    }
    ruRetWithCode(status, RUE_OK, out);
}

static yajl_val jsonKey(ruJson rj, trans_chars key, yajl_type type, int32_t* status) {
    yajl_val node = getYajlVal(rj, status);
    if (!node) return NULL;
    if (!key) ruRetWithCode(status, RUE_PARAMETER_NOT_SET, NULL);
    const char * path[] = { key, (const char *) 0 };
    yajl_val v = yajl_tree_get(node, path, type);
    if (v) {
        ruVerbLogf("returning key '%s'", key);
        ruRetWithCode(status, RUE_OK, v);
    }

    ruVerbLogf("key '%s' not specified or wrong type", key);
    ruRetWithCode(status, RUE_FILE_NOT_FOUND, NULL);
}

static yajl_val jsonIdx(ruJson rj, rusize index, yajl_type type, int32_t* status) {
    yajl_val node = getYajlVal(rj, status);
    if (!node) return NULL;
    if (!YAJL_IS_ARRAY(node)) {
        ruRetWithCode(status, RUE_INVALID_PARAMETER, NULL);
    }
    if (index >= node->u.array.len) {
        ruRetWithCode(status, RUE_WRONG_PARAMETER_LENGTH, NULL);
    }
    yajl_val v = node->u.array.values[index];
    if (type == yajl_t_any || v->type == type) {
        ruRetWithCode(status, RUE_OK, v);
    }
    ruVerbLogf("index %ld is of invalid type", index, v->type);
    ruRetWithCode(status, RUE_FILE_NOT_FOUND, NULL);
}

static perm_chars nodeStr(yajl_val node, int32_t* status) {
    perm_chars out = NULL;
    if (!node) ruRetWithCode(status, RUE_FILE_NOT_FOUND, out);
    if (!YAJL_IS_STRING(node)) {
        ruRetWithCode(status, RUE_INVALID_PARAMETER, out);
    }
    out = YAJL_GET_STRING(node);
    ruVerbLogf("returning '%s'" , out);
    ruRetWithCode(status, RUE_OK, out);
}

bool nodeBool(yajl_val node, int32_t* status) {
    bool out = false;
    if (!node) ruRetWithCode(status, RUE_FILE_NOT_FOUND, out);
    if (YAJL_IS_TRUE(node)) {
        out = true;
        ruRetWithCode(status, RUE_OK, out);
    }
    if (YAJL_IS_FALSE(node)) {
        ruRetWithCode(status, RUE_OK, out);
    }
    ruRetWithCode(status, RUE_INVALID_PARAMETER, out);
}

int64_t nodeInt(yajl_val node, int32_t* status) {
    int64_t out = 0;
    if (!node) ruRetWithCode(status, RUE_FILE_NOT_FOUND, out);
    if (!YAJL_IS_INTEGER(node)) {
        ruRetWithCode(status, RUE_INVALID_PARAMETER, out);
    }
    out = YAJL_GET_INTEGER(node);
    ruVerbLogf("returning %" PRId64, out);
    ruRetWithCode(status, RUE_OK, out);
}

double nodeDouble(yajl_val v, int32_t* status) {
    double out = 0;
    if (!v) ruRetWithCode(status, RUE_FILE_NOT_FOUND, out);
    if (!YAJL_IS_DOUBLE(v)) {
        ruRetWithCode(status, RUE_INVALID_PARAMETER, out);
    }
    out = YAJL_GET_DOUBLE(v);
    ruVerbLogf("returning %0.6f", out);
    ruRetWithCode(status, RUE_OK, out);
}

ruJson nodeMap(yajl_val v, int32_t* status) {
    ruJson out = NULL;
    if (!v) ruRetWithCode(status, RUE_FILE_NOT_FOUND, out);
    if (!YAJL_IS_OBJECT(v)) {
        ruRetWithCode(status, RUE_INVALID_PARAMETER, out);
    }
    ruRetWithCode(status, RUE_OK, v);
}

ruJson nodeArray(yajl_val v, int32_t* status) {
    ruJson out = NULL;
    if (!v) ruRetWithCode(status, RUE_FILE_NOT_FOUND, out);
    if (!YAJL_IS_ARRAY(v)) {
        ruRetWithCode(status, RUE_INVALID_PARAMETER, out);
    }
    ruRetWithCode(status, RUE_OK, v);
}

//</editor-fold>

//<editor-fold desc="common">
RUAPI ruJson ruJsonFree(ruJson rj) {
    if (!rj) return NULL;
    json* j = jsonGet(rj, NULL);
    if (j->g) yajl_gen_free(j->g);
    if (j->node) yajl_tree_free(j->node);
    ruFree(j);
    return NULL;
}

//</editor-fold>

//<editor-fold desc="setter">
RUAPI ruJson ruJsonNew(void) {
    json* j = ruMalloc0(1, json);
    j->type = MagicJson;
    j->g = yajl_gen_alloc(NULL);

    if (!j->g) {
        ruCritLogf("Failed to allocate yail generator: ec:%d", RUE_OUT_OF_MEMORY);
        return ruJsonFree(j);
    }
    if (!yajl_gen_config(j->g, yajl_gen_validate_utf8, 1)) {
        ruCritLog("Failed to configure yail generator");
        return ruJsonFree(j);
    }
    return (ruJson)j;
}

RUAPI ruJson ruJsonStart(bool map) {
    json* j = ruJsonNew();
    if (!j) return NULL;
    yajl_gen_status yst;
    if (map) {
        yst = yajl_gen_map_open(j->g);
        j->open = yajl_t_object;
    } else {
        yst = yajl_gen_array_open(j->g);
        j->open = yajl_t_array;
    }
    if (yst != yajl_gen_status_ok) {
        ruCritLogf("Failed to open yail container json ec: %d", yst);
        return ruJsonFree(j);
    }
    return (ruJson)j;
}

RUAPI int32_t ruJsonStartMap(ruJson rj) {
    int32_t ret = RUE_GENERAL;
    json* j = jsonGetSet(rj, &ret);
    if (!j) return ret;
    do {
        ret = RUE_INVALID_PARAMETER;
        yajl_gen_status yst = yajl_gen_map_open(j->g);
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate array json ec: %d", yst);
            break;
        }
        ret = RUE_OK;
    } while(0);
    return ret;
}

RUAPI int32_t ruJsonEndMap(ruJson rj) {
    int32_t ret = RUE_GENERAL;
    json* j = jsonGetSet(rj, &ret);
    if (!j) return ret;
    do {
        ret = RUE_INVALID_PARAMETER;
        yajl_gen_status yst = yajl_gen_map_close(j->g);
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate array json ec: %d", yst);
            break;
        }
        ret = RUE_OK;
    } while(0);
    return ret;
}

RUAPI int32_t ruJsonStartArray(ruJson rj) {
    int32_t ret = RUE_GENERAL;
    json* j = jsonGetSet(rj, &ret);
    if (!j) return ret;
    do {
        ret = RUE_INVALID_PARAMETER;
        yajl_gen_status yst = yajl_gen_array_open(j->g);
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate array json ec: %d", yst);
            break;
        }
        ret = RUE_OK;
    } while(0);
    return ret;
}

RUAPI int32_t ruJsonEndArray(ruJson rj) {
    int32_t ret = RUE_GENERAL;
    json* j = jsonGetSet(rj, &ret);
    if (!j) return ret;
    do {
        ret = RUE_INVALID_PARAMETER;
        yajl_gen_status yst = yajl_gen_array_close(j->g);
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate array json ec: %d", yst);
            break;
        }
        ret = RUE_OK;
    } while(0);
    return ret;
}

RUAPI int32_t ruJsonSetInt(ruJson rj, int64_t val) {
    int32_t ret = RUE_GENERAL;
    json* j = jsonGetSet(rj, &ret);
    if (!j) return ret;
    do {
        ret = RUE_INVALID_PARAMETER;
        yajl_gen_status yst = yajl_gen_integer(j->g, val);
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate value [%" PRId64 "] json ec: %d",
                       val, yst);
            break;
        }
        ret = RUE_OK;
    } while(0);
    return ret;
}

RUAPI int32_t ruJsonSetDouble(ruJson rj, double val) {
    int32_t ret = RUE_GENERAL;
    json* j = jsonGetSet(rj, &ret);
    if (!j) return ret;
    do {
        ret = RUE_INVALID_PARAMETER;
        yajl_gen_status yst = yajl_gen_double(j->g, val);
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate value [%" PRId64 "] json ec: %d",
                       val, yst);
            break;
        }
        ret = RUE_OK;
    } while(0);
    return ret;
}

RUAPI int32_t ruJsonSetStr(ruJson rj, trans_chars val) {
    int32_t ret = RUE_GENERAL;
    json* j = jsonGetSet(rj, &ret);
    if (!j) return ret;
    do {
        ret = RUE_INVALID_PARAMETER;
        yajl_gen_status yst;
        if (val) {
            yst = yajl_gen_string(j->g, (trans_bytes) val,
                                   ruStrLen((char*) val));
        } else {
            yst = yajl_gen_null(j->g);
        }
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate value [%s] json ec: %d", val, yst);
            break;
        }
        ret = RUE_OK;
    } while(0);
    return ret;
}

RUAPI int32_t ruJsonSetKeyStr(ruJson rj, trans_chars key, trans_chars val) {
    int32_t ret = RUE_GENERAL;
    json* j = jsonGetSet(rj, &ret);
    if (!j) return ret;
    if (!key) return RUE_PARAMETER_NOT_SET;
    do {
        ret = RUE_INVALID_PARAMETER;
        yajl_gen_status yst = yajl_gen_string(j->g,
                                               (trans_bytes)key,
                                               ruStrLen((char*)key));
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate key [%s] json ec: %d", key, yst);
            break;
        }
        if (val) {
            yst = yajl_gen_string(j->g, (trans_bytes) val,
                                   ruStrLen((char*) val));
        } else {
            yst = yajl_gen_null(j->g);
        }
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate value [%s] for key [%s] json ec: %d", val, key, yst);
            break;
        }
        ret = RUE_OK;
    } while(0);
    return ret;
}

RUAPI int32_t ruJsonSetKeyInt(ruJson rj, trans_chars key, int64_t val) {
    int32_t ret = RUE_GENERAL;
    json* j = jsonGetSet(rj, &ret);
    if (!j) return ret;
    if (!key) return RUE_PARAMETER_NOT_SET;
    do {
        ret = RUE_INVALID_PARAMETER;
        yajl_gen_status yst = yajl_gen_string(j->g,
                                               (trans_bytes)key,
                                               ruStrLen(key));
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate key [%s] json ec: %d", key, yst);
            break;
        }
        yst = yajl_gen_integer(j->g, val);
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate value [%" PRId64 "] for key [%s] json ec: %d",
                       val, key, yst);
            break;
        }
        ret = RUE_OK;
    } while(0);
    return ret;
}

RUAPI int32_t ruJsonSetKeyDouble(ruJson rj, trans_chars key, double val) {
    int32_t ret = RUE_GENERAL;
    json* j = jsonGetSet(rj, &ret);
    if (!j) return ret;
    if (!key) return RUE_PARAMETER_NOT_SET;
    do {
        ret = RUE_INVALID_PARAMETER;
        yajl_gen_status yst = yajl_gen_string(j->g,
                                               (trans_bytes)key,
                                               ruStrLen(key));
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate key [%s] json ec: %d", key, yst);
            break;
        }
        yst = yajl_gen_double(j->g, val);
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate value [%f] for key [%s] json ec: %d",
                       val, key, yst);
            break;
        }
        ret = RUE_OK;
    } while(0);
    return ret;
}

RUAPI int32_t ruJsonStartKeyMap(ruJson rj, trans_chars key) {
    int32_t ret = RUE_GENERAL;
    json* j = jsonGetSet(rj, &ret);
    if (!j) return ret;
    if (!key) return RUE_PARAMETER_NOT_SET;
    do {
        ret = RUE_INVALID_PARAMETER;
        yajl_gen_status yst = yajl_gen_string(j->g,
                                               (trans_bytes)key,
                                               ruStrLen((char*)key));
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate key [%s] json ec: %d", key, yst);
            break;
        }
        yst = yajl_gen_map_open(j->g);
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate array for key [%s] json ec: %d",
                       key, yst);
            break;
        }
        ret = RUE_OK;
    } while(0);
    return ret;
}

RUAPI int32_t ruJsonStartKeyArray(ruJson rj, trans_chars key) {
    int32_t ret = RUE_GENERAL;
    json* j = jsonGetSet(rj, &ret);
    if (!j) return ret;
    if (!key) return RUE_PARAMETER_NOT_SET;
    do {
        ret = RUE_INVALID_PARAMETER;
        yajl_gen_status yst = yajl_gen_string(j->g,
                                               (trans_bytes)key,
                                               ruStrLen((char*)key));
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate key [%s] json ec: %d", key, yst);
            break;
        }
        yst = yajl_gen_array_open(j->g);
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to generate array for key [%s] json ec: %d",
                       key, yst);
            break;
        }
        ret = RUE_OK;
    } while(0);
    return ret;
}

RUAPI int32_t ruJsonWrite(ruJson rj, perm_chars* output) {
    int32_t ret = RUE_GENERAL;
    json* j = jsonGetSet(rj, &ret);
    if (!j) return ret;
    if (!output) return RUE_PARAMETER_NOT_SET;
    yajl_gen_status yst = yajl_gen_status_ok;
    do {
        if (j->open == yajl_t_object) {
            yst = yajl_gen_map_close(j->g);
        } else if (j->open == yajl_t_array) {
            yst = yajl_gen_array_close(j->g);
        }
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to close yail container json ec: %d", yst);
            break;
        }
        trans_bytes buf = NULL;
        rusize len;
        yst = yajl_gen_get_buf(j->g, &buf, &len);
        if (yst != yajl_gen_status_ok) {
            ruCritLogf("Failed to get encoded string. json ec: %d", yst);
            ret = RUE_GENERAL;
            break;
        }
        *output = (perm_chars)buf;
        ret = RUE_OK;
    } while(0);
    return ret;
}
//</editor-fold>

//<editor-fold desc="getter">
RUAPI ruJson ruJsonParse(trans_chars jsonStr, int32_t* status) {
    ruClearError();
    if (!jsonStr) ruRetWithCode(status, RUE_PARAMETER_NOT_SET, NULL);
    ruVerbLogf("Parsing: %s", jsonStr);
    json* j = ruMalloc0(1, json);
    j->type = MagicJson;
    char errbuf[1024];
    j->node = yajl_tree_parse(jsonStr, errbuf, sizeof(errbuf));
    /* parse error handling */
    if (j->node == NULL) {
        if (ruStrLen(errbuf)) {
            ruSetError("parse_error: '%s' Content: '%s'", errbuf, jsonStr);
            ruWarnLogf("parse_error: '%s' Content: '%s'", errbuf, jsonStr);
        } else {
            ruSetError("parse_error: 'unknown' Content: '%s'", jsonStr);
            ruWarnLogf("parse_error: 'unknown' Content: '%s'", jsonStr);
        }
        ruFree(j);
        ruRetWithCode(status, RUE_INVALID_PARAMETER, NULL);
    }
    ruRetWithCode(status, RUE_OK, (ruJson)j);
}

RUAPI rusize ruJsonArrayLen(ruJson rj, int32_t* status) {
    yajl_val v = getYajlVal(rj, status);
    if (!v) return 0;
    if (!YAJL_IS_ARRAY(v)) {
        ruRetWithCode(status, RUE_INVALID_PARAMETER, 0);
    }
    ruRetWithCode(status, RUE_OK, v->u.array.len);
}


RUAPI perm_chars ruJsonStr(ruJson rj, int32_t* status) {
    yajl_val v = getYajlVal(rj, status);
    if (!v) return NULL;
    return nodeStr(v, status);
}

RUAPI int64_t ruJsonParseInt(ruJson rj, int32_t* status) {
    yajl_val v = getYajlVal(rj, status);
    if (!v) return 0;
    return parseNum(v, status);
}

RUAPI int64_t ruJsonInt(ruJson rj, int32_t* status) {
    yajl_val v = getYajlVal(rj, status);
    if (!v) return 0;
    return nodeInt(v, status);
}

RUAPI ruList ruJsonKeys(ruJson rj, int32_t* status) {
    yajl_val v = getYajlVal(rj, status);
    if (!v) return NULL;
    if (!YAJL_IS_OBJECT(v)) {
        ruRetWithCode(status, RUE_INVALID_PARAMETER, NULL);
    }
    ruList out = ruListNew(ruTypeStrRef());
    for (size_t i = 0; i < v->u.object.len; i++) {
        ruListAppend(out, v->u.object.keys[i]);
    }
    ruRetWithCode(status, RUE_OK, out);
}

RUAPI perm_chars ruJsonKeyStr(ruJson rj, trans_chars key, int32_t* status) {
    yajl_val v = jsonKey(rj, key, yajl_t_string, status);
    if (!v) return NULL;
    return nodeStr(v, status);
}

RUAPI alloc_chars ruJsonKeyToStr(ruJson rj, trans_chars key, int32_t* status) {
    yajl_val v = jsonKey(rj, key, yajl_t_any, status);
    if (!v) return NULL;
    return toString(v, status);
}

RUAPI alloc_chars ruJsonKeyStrDup(ruJson rj, trans_chars key, int32_t* status) {
    return ruStrDup(ruJsonKeyStr(rj, key, status));
}

RUAPI int64_t ruJsonKeyParseInt(ruJson rj, trans_chars key, int32_t* status) {
    yajl_val v = jsonKey(rj, key, yajl_t_any, status);
    if (!v) return 0;
    return parseNum(v, status);
}

RUAPI int64_t ruJsonKeyInt(ruJson rj, trans_chars key, int32_t* status) {
    yajl_val v = jsonKey(rj, key, yajl_t_number, status);
    if (!v) return 0;
    return nodeInt(v, status);
}

RUAPI double ruJsonKeyDouble(ruJson rj, trans_chars key, int32_t* status) {
    yajl_val v = jsonKey(rj, key, yajl_t_number, status);
    if (!v) return 0;
    return nodeDouble(v, status);
}

RUAPI bool ruJsonKeyBool(ruJson rj, trans_chars key, int32_t* status) {
    yajl_val v = jsonKey(rj, key, yajl_t_any, status);
    if (!v) return 0;
    return nodeBool(v, status);
}

RUAPI ruJson ruJsonKeyMap(ruJson rj, trans_chars key, int32_t* status) {
    yajl_val v = jsonKey(rj, key, yajl_t_object, status);
    if (!v) return NULL;
    return nodeMap(v, status);
}

RUAPI ruJson ruJsonKeyArray(ruJson rj, trans_chars key, int32_t* status) {
    yajl_val v = jsonKey(rj, key, yajl_t_array, status);
    if (!v) return NULL;
    return nodeArray(v, status);
}

RUAPI perm_chars ruJsonIdxStr(ruJson rj, rusize index, int32_t* status) {
    yajl_val v = jsonIdx(rj, index, yajl_t_string, status);
    if (!v) return NULL;
    return nodeStr(v, status);
}

RUAPI alloc_chars ruJsonIdxStrDup(ruJson rj, rusize index, int32_t* status) {
    return ruStrDup(ruJsonIdxStr(rj, index, status));
}

RUAPI int64_t ruJsonIdxParseInt(ruJson rj, rusize index, int32_t* status) {
    yajl_val v = jsonIdx(rj, index, yajl_t_any, status);
    if (!v) return 0;
    return parseNum(v, status);
}

RUAPI int64_t ruJsonIdxInt(ruJson rj, rusize index, int32_t* status) {
    yajl_val v = jsonIdx(rj, index, yajl_t_number, status);
    if (!v) return 0;
    return nodeInt(v, status);
}

RUAPI double ruJsonIdxDouble(ruJson rj, rusize index, int32_t* status) {
    yajl_val v = jsonIdx(rj, index, yajl_t_number, status);
    if (!v) return 0;
    return nodeDouble(v, status);
}

RUAPI ruJson ruJsonIdxMap(ruJson rj, rusize index, int32_t* status) {
    yajl_val v = jsonIdx(rj, index, yajl_t_object, status);
    if (!v) return NULL;
    return nodeMap(v, status);
}

RUAPI ruJson ruJsonIdxArray(ruJson rj, rusize index, int32_t* status) {
    yajl_val v = jsonIdx(rj, index, yajl_t_array, status);
    if (!v) return NULL;
    return nodeArray(v, status);
}

//</editor-fold>

