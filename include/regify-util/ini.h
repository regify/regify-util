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
 * \defgroup ini Ini File Handling
 * \brief This section contains ini file handling support.
 *
 * Example:
 * ~~~~~{.c}
   ruIni cf = NULL;
   const char* inipath = "/path/to/file.ini";
   // We are going to ignore some return codes for brevity.
   int32_t ret = ruIniRead(inipath, &cf);
   const char* name = NULL;
   ret = ruIniGet(cf, NULL, "username", &name);
   printf("Username: %s", name);
   // make some changes
   ret = ruIniSet(cf, NULL, "password", "secret");
   ret = ruIniWrite(cf, inipath);
   cf = ruIniFree(cf);
 * ~~~~~
 *
 * @{
 */

#ifndef REGIFY_UTIL_INI_H
#define REGIFY_UTIL_INI_H

/**
 * \brief An opaque type representing an ini object.
 */
typedef void* ruIni;

/**
 * \brief Frees the given ini object and its members
 * @param ini object to free
 * @return NULL
 */
RUAPI ruIni ruIniFree(ruIni ini);

/**
 * \brief Create a new empty ini object
 * @return The object
 */
RUAPI ruIni ruIniNew(void);

/**
 * \brief Writes given ini object into given filename
 * @param iniOb Ini object to write
 * @param filename Path to ini file
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruIniWrite(ruIni iniOb, trans_chars filename);

/**
 * \brief Parse given INI-style file.
 * May have [section]s, name=value pairs (whitespace stripped), and comments
 * starting with ';' (semicolon). Section is "" if name=value pair parsed before
 * any section heading. name:value pairs are also supported as a concession to
 * Python's configparser.
 *
 * For each name=value pair parsed, call handler function with given user
 * pointer as well as section, name, and value (data only valid for duration
 * of handler call). Handler should return nonzero on success, zero on error.
 *
 * @param filename path to file to parse
 * @param iniOb Where the result \ref ruIni object will be stored. Free with \ref ruIniFree.
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruIniRead(trans_chars filename, ruIni* iniOb);

/**
 * \brief Returns a list of sections from the given ini object
 * @param iniOb Object to get sections from
 * @param sections List of sections to be freed by caller after use.
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruIniSections(ruIni iniOb, ruList* sections);

/**
 * \brief Returns a list of keys from given section or global.
 * @param iniOb Object to get sections from
 * @param section Section to get keys from or NULL for global keys
 * @param keys List of keys to be freed by caller after use.
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruIniKeys(ruIni iniOb, trans_chars section, ruList* keys);

/**
 * \brief Retrieves a value or given default from the given ini object.
 * @param iniOb Object to get value from
 * @param section Section to get value from. NULL for no section.
 * @param key Key name.
 * @param def Will be returned if not NULL and value is NULL or empty string.
 * @param code Optional, where to store the return code of the operation
 * @return The found value or def if the value was not found or blank.
 */
RUAPI perm_chars ruIniGetDef(ruIni iniOb, trans_chars section, trans_chars key,
                              trans_chars def, int32_t* code);

/**
 * \brief Retrieves a value from the given ini object.
 * @param iniOb Object to get value from
 * @param section Section to get value from. NULL for no section.
 * @param key Key name.
 * @param value Where the value will be stored. Caller should copy as needed
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruIniGet(ruIni iniOb, trans_chars section, trans_chars key, perm_chars* value);

/**
 * \brief Set a value in the given ini object.
 * @param iniOb Object to set value in
 * @param section Section to set value in. NULL for no section.
 * @param key Key name
 * @param value The value to set. Use blank for an empty entry and NULL to remove
 * @return \ref RUE_OK on success else an error code
 */
RUAPI int32_t ruIniSet(ruIni iniOb, trans_chars section, trans_chars key, trans_chars value);

#endif //REGIFY_UTIL_INI_H

/**
 * @}
 */
