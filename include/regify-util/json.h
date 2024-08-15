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
 * \defgroup json JSON
 * \brief A simplified wrapper of the yajl library.
 *
 * This is a wrapper of https://lloyd.github.io/yajl/
 * It wraps the the yajl_tree API for parsing small content, and the yajl_gen
 * API for generating JSON.
 * See \ref ruJsonNew and \ref ruJsonStart for generation samples
 * and \ref ruJsonParse for a parsing sample.
 *
 * @{
 */

#ifndef REGIFY_UTIL_JSON_H
#define REGIFY_UTIL_JSON_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**
 * \brief An opaque type representing a JSON object.
 */
typedef void* ruJson;

#define RU_JSON_PRETTIFY 0x01
#define RU_JSON_ESCAPE_SLASH 0x02


/**
 * Free the given \ref ruJson object
 * @param rj JSON ctx to free
 * @return NULL
 */
RUAPI ruJson ruJsonFree(ruJson rj);

/**
 * Creates a new \ref ruJson object for generating JSON
 *
 * @param flags Optional ORing of \ref RU_JSON_PRETTIFY and
 *              \ref RU_JSON_ESCAPE_SLASH
 * @param ident Optional indent string to use when flags has
 *              \ref RU_JSON_PRETTIFY set.
 *
 * Example:
 * ~~~~~{.c}
    ruJson jsn = NULL;
    do{
        jsn = ruJsonNew(RU_JSON_PRETTIFY, NULL);
        if (!jsn) break;

        if (RUE_OK != ruJsonStartArray(jsn)) break;
        if (RUE_OK != ruJsonSetStr(jsn, "value")) break;
        if (RUE_OK != ruJsonSetInt(jsn, 23)) break;
        if (RUE_OK != ruJsonStartMap(jsn)) break;
        if (RUE_OK != ruJsonSetKeyStr(jsn, "key", "other value")) break;
        if (RUE_OK != ruJsonSetKeyInt(jsn, "num", 42)) break;
        if (RUE_OK != ruJsonEndMap(jsn)) break;
        if (RUE_OK != ruJsonEndArray(jsn)) break;

        perm_chars str =  NULL;
        if (RUE_OK != ruJsonWrite(jsn, &str)) break;
        printf("JSON: '%s'", str);

    } while(0);
    ruJsonFree(jsn);
 * ~~~~~
 *
 * @return ctx to generate JSON with.
 */
RUAPI ruJson ruJsonNew(int32_t flags, trans_chars ident);

/**
 * Creates a new \ref ruJson object with a top level container for generating JSON.
 *
 * Example:
 * ~~~~~{.c}
    ruJson jsn = NULL;
    do{
        jsn = ruJsonStart(true);
        if (!jsn) break;

        if (RUE_OK != ruJsonSetKeyStr(jsn, "key", "value")) break;
        if (RUE_OK != ruJsonSetKeyInt(jsn, "num", 23)) break;

        perm_chars str =  NULL;
        if (RUE_OK != ruJsonWrite(jsn, &str)) break;
        printf("JSON: '%s'", str);

    } while(0);
    ruJsonFree(jsn);
 * ~~~~~
 *
 * The created top level array or map will be automatically closed before writing
 * @param map true to create a map false for an array
 * @return ctx to generate JSON with.
 */
RUAPI ruJson ruJsonStart(bool map);

/**
 * Start a new map in the current context
 * @param rj JSON ctx to work on
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruJsonStartMap(ruJson rj);

/**
 * End the map context started with \ref ruJsonStartMap or \ref ruJsonStartKeyMap
 * @param rj JSON ctx to work on
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruJsonEndMap(ruJson rj);

/**
 * Start a new array in the current context
 * @param rj JSON ctx to work on
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruJsonStartArray(ruJson rj);

/**
 * End the array context started with \ref ruJsonStartArray or \ref ruJsonStartKeyArray
 * @param rj JSON ctx to work on
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruJsonEndArray(ruJson rj);

/**
 * Create an integer entry in the current context
 * @param rj JSON ctx to work on
 * @param val integer to set
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruJsonSetInt(ruJson rj, int64_t val);

/**
 * Create an double entry in the current context
 * @param rj JSON ctx to work on
 * @param val double to set
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruJsonSetDouble(ruJson rj, double val);

/**
 * Create a string entry in the current context
 * @param rj JSON ctx to work on
 * @param val string to set
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruJsonSetStr(ruJson rj, trans_chars val);

/**
 * Create a string key value entry in the current context
 * @param rj JSON ctx to work on
 * @param key key string
 * @param val string to set
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruJsonSetKeyStr(ruJson rj, trans_chars key, trans_chars val);

/**
 * Create a integer key value entry in the current context
 * @param rj JSON ctx to work on
 * @param key key string
 * @param val integer to set
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruJsonSetKeyInt(ruJson rj, trans_chars key, int64_t val);

/**
 * Create a double key value entry in the current context
 * @param rj JSON ctx to work on
 * @param key key string
 * @param val double to set
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruJsonSetKeyDouble(ruJson rj, trans_chars key, double val);

/**
 * Start a new map under given key in the current context
 * @param rj JSON ctx to work on
 * @param key key string
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruJsonStartKeyMap(ruJson rj, trans_chars key);

/**
 * Start a new array under given key in the current context
 * @param rj JSON ctx to work on
 * @param key key string
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruJsonStartKeyArray(ruJson rj, trans_chars key);

/**
 * Generate a JSON string representing the given JOSN object
 * @param rj JSON ctx to work on
 * @param output where the string will be stored. User must not free.
 *               Valid until rj is freed with \ref  ruJsonFree.
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruJsonWrite(ruJson rj, perm_chars* output);

/**
 * Parse given string into a \ref ruJson object.
 *
 * Example:
 * ~~~~~{.c}
    ruJson jsn = NULL;
    int32_t ret;
    do{

        perm_chars jsonStr = "{"
                 "\"map\": {"
                    "\"key\": \"2342\", "
                    "\"num\": 2342, "
                    "\"dbl\": 23.42"
                    "}, "
                 "\"arr\": ["
                    "2342, \"2342\", 23.42"
                    "]"
                 "}";
        jsn = ruJsonParse(jsonStr, &ret);
        if (ret != RUE_OK) break;

        // the map
        ruJson jm = ruJsonKeyMap(jsn, "map", &ret);
        if (ret != RUE_OK) break;

        perm_chars str = ruJsonKeyStr(jm, "key", &ret);
        if (ret != RUE_OK) break;
        printf("str: '%s'", str);

        int64_t i64 = ruJsonKeyInt(jm, "num", &ret);
        if (ret != RUE_OK) break;
        printf("int: %ld", i64);

        double db = ruJsonKeyDouble(jm, "dbl", &ret);
        if (ret != RUE_OK) break;
        printf("float: %f", db);

        // the array
        ruJson ja = ruJsonKeyArray(jsn, "arr", &ret);
        if (ret != RUE_OK) break;

        i64 = ruJsonIdxInt(ja, 0, &ret);
        if (ret != RUE_OK) break;
        printf("int: %ld", i64);

        str = ruJsonIdxStr(ja, 1, &ret);
        if (ret != RUE_OK) break;
        printf("str: '%s'", str);

        i64 = ruJsonIdxParseInt(ja, 1, &ret);
        if (ret != RUE_OK) break;
        printf("int: %ld", i64);

        db = ruJsonIdxDouble(ja, 2, &ret);
        if (ret != RUE_OK) break;
        printf("float: %f", db);

    } while(0);
    ruJsonFree(jsn);
 * ~~~~~
 *
 * @param jsonStr
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return \ref ruJson object to be freed with \ref ruJsonFree after use.
 */
RUAPI ruJson ruJsonParse(trans_chars jsonStr, int32_t* status);

/**
 * Return array size of underlying \ref ruJson reference
 * @param rj \ref ruJson reference pointing to an array
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return size of the array
 */
RUAPI rusize ruJsonArrayLen(ruJson rj, int32_t* status);

/**
 * Return string of underlying \ref ruJson reference.
 * This is only used if the JSON object consists of only a string.
 * @param rj \ref ruJson reference pointing to a string
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return string in question. Belongs to the \ref ruJson object.
 */
RUAPI perm_chars ruJsonStr(ruJson rj, int32_t* status);

/**
 * Return parsed integer of underlying \ref ruJson reference.
 * This is only used if the JSON object consists of only an integer or quoted integer.
 * @param rj \ref ruJson reference pointing to an integer.
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return integer in question
 */
RUAPI int64_t ruJsonParseInt(ruJson rj, int32_t* status);

/**
 * Return integer of underlying \ref ruJson reference.
 * This is only used if the JSON object consists of only an integer.
 * @param rj \ref ruJson reference pointing to an integer.
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return integer in question
 */
RUAPI int64_t ruJsonInt(ruJson rj, int32_t* status);

/**
 * Return parsed boolean of underlying \ref ruJson reference.
 * The boolean may also be an integer 0, 1 or a quoted case insensitive true or false.
 * @param rj \ref ruJson reference pointing to an integer.
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return boolean in question and false on error.
 */
RUAPI bool ruJsonParseBool(ruJson rj, int32_t* status);

/**
 * Return boolean of underlying \ref ruJson reference.
 * This is only used if the JSON object consists of only a boolean.
 * @param rj \ref ruJson reference pointing to a boolean.
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return boolean in question and false on error.
 */
RUAPI bool ruJsonBool(ruJson rj, int32_t* status);

/**
 * \brief Returns an \ref ruList of keys in the current map.
 * @param rj \ref ruJson in question.
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return The list of map keys. Caller to free with \ref ruListFree. List is
 *         only valid while given \ref ruJson object is there.
 */
RUAPI ruList ruJsonKeys(ruJson rj, int32_t* status);

/**
 * Return string of the key element from underlying \ref ruJson map reference.
 * @param rj \ref ruJson in question.
 * @param key key under which string resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return string in question. Belongs to the \ref ruJson object.
 */
RUAPI perm_chars ruJsonKeyStr(ruJson rj, trans_chars key, int32_t* status);

/**
 * Return string version of the key element from underlying \ref ruJson map.
 * @param rj \ref ruJson in question.
 * @param key key under which value resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return string version of value in question. Caller must free.
 */
RUAPI alloc_chars ruJsonKeyToStr(ruJson rj, trans_chars key, int32_t* status);

/**
 * Return string copy of the key element from underlying \ref ruJson map reference.
 * @param rj \ref ruJson in question.
 * @param key key under which string resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return string in question. Must be freed by the caller.
 */
RUAPI alloc_chars ruJsonKeyStrDup(ruJson rj, trans_chars key, int32_t* status);

/**
 * Return parsed integer of the key element from underlying \ref ruJson map reference.
 * The integer may be a quoted string.
 * @param rj \ref ruJson in question.
 * @param key key under which integer resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return integer in question.
 */
RUAPI int64_t ruJsonKeyParseInt(ruJson rj, trans_chars key, int32_t* status);

/**
 * Return integer of the key element from underlying \ref ruJson map reference.
 * @param rj \ref ruJson in question.
 * @param key key under which integer resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return integer in question.
 */
RUAPI int64_t ruJsonKeyInt(ruJson rj, trans_chars key, int32_t* status);

/**
 * Return double of the key element from underlying \ref ruJson map reference.
 * @param rj \ref ruJson in question.
 * @param key key under which double resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return double in question.
 */
RUAPI double ruJsonKeyDouble(ruJson rj, trans_chars key, int32_t* status);

/**
 * Return parsed boolean of the key element from underlying \ref ruJson map reference.
 * The boolean may also be an integer 0, 1 or a quoted case insensitive true or false.
 * @param rj \ref ruJson in question.
 * @param key key under which boolean resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return boolean in question and false on error.
 */
RUAPI bool ruJsonKeyParseBool(ruJson rj, trans_chars key, int32_t* status);

/**
 * Return boolean of the key element from underlying \ref ruJson map reference.
 * @param rj \ref ruJson in question.
 * @param key key under which boolean resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return boolean in question and false on error.
 */
RUAPI bool ruJsonKeyBool(ruJson rj, trans_chars key, int32_t* status);

/**
 * Retuens a \ref ruJson reference to the map under given key
 * @param rj \ref ruJson in question.
 * @param key key under which map resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return \ref ruJson referencing the map
 */
RUAPI ruJson ruJsonKeyMap(ruJson rj, trans_chars key, int32_t* status);

/**
 * Retuens a \ref ruJson reference to the array under given key
 * @param rj \ref ruJson in question.
 * @param key key under which array resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return \ref ruJson referencing the array
 */
RUAPI ruJson ruJsonKeyArray(ruJson rj, trans_chars key, int32_t* status);

/**
 * Return string of the indexed element from underlying \ref ruJson array reference.
 * @param rj \ref ruJson in question.
 * @param index index under which the string resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return string in question. Belongs to the \ref ruJson object.
 */
RUAPI perm_chars ruJsonIdxStr(ruJson rj, rusize index, int32_t* status);

/**
 * Return string copy of the indexed element from underlying \ref ruJson array reference.
 * @param rj \ref ruJson in question.
 * @param index index under which the string resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return string in question. Must be freed by the caller.
 */
RUAPI alloc_chars ruJsonIdxStrDup(ruJson rj, rusize index, int32_t* status);

/**
 * Return parsed integer of the indexed element from underlying \ref ruJson array reference.
 * The integer may be a quoted string.
 * @param rj \ref ruJson in question.
 * @param index index under which the string resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return integer in question.
 */
RUAPI int64_t ruJsonIdxParseInt(ruJson rj, rusize index, int32_t* status);

/**
 * Return integer of the indexed element from underlying \ref ruJson array reference.
 * @param rj \ref ruJson in question.
 * @param index index under which the integer resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return integer in question.
 */
RUAPI int64_t ruJsonIdxInt(ruJson rj, rusize index, int32_t* status);

/**
 * Return parsed boolean of the indexed element from underlying \ref ruJson array reference.
 * The boolean may also be an integer 0, 1 or a quoted case insensitive true or false.
 * @param rj \ref ruJson in question.
 * @param index index under which the string resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return boolean in question and false on error.
 */
RUAPI bool ruJsonIdxParseBool(ruJson rj, rusize index, int32_t* status);

/**
 * Return boolean of the indexed element from underlying \ref ruJson array reference.
 * @param rj \ref ruJson in question.
 * @param key key under which boolean resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return boolean in question and false on error.
 */
RUAPI bool ruJsonIdxBool(ruJson rj, rusize index, int32_t* status);

/**
 * Return double of the indexed element from underlying \ref ruJson array reference.
 * @param rj \ref ruJson in question.
 * @param index index under which the double resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return double in question.
 */
RUAPI double ruJsonIdxDouble(ruJson rj, rusize index, int32_t* status);

/**
 * Retuens a \ref ruJson reference to the map under given array index
 * @param rj \ref ruJson in question.
 * @param index index under which the map resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return \ref ruJson referencing the map
 */
RUAPI ruJson ruJsonIdxMap(ruJson rj, rusize index, int32_t* status);

/**
 * Retuens a \ref ruJson reference to the array under given array index
 * @param rj \ref ruJson in question.
 * @param index index under which the array resides
 * @param status where the \ref RUE_OK on success or an error code will be stored.
 * @return \ref ruJson referencing the array
 */
RUAPI ruJson ruJsonIdxArray(ruJson rj, rusize index, int32_t* status);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_JSON_H
