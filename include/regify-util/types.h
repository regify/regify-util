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
#ifndef REGIFY_UTIL_TYPES_H
#define REGIFY_UTIL_TYPES_H
/* Only need to export C interface if used by C++ source code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**
 * \defgroup specs Container Type Specifications
 * \brief This section contains specifications for objects in collections.
 *
 * These are used in \ref ruMap, \ref ruSet and \ref ruList.
 *
 * @{
 */

/**
 * \brief Function signature of a hashing function for \ref ruMap or \ref ruSet keys.
 */
typedef ru_uint (*ruHashFunc)(trans_ptr key);

/**
 * \brief Function signature of a matching function for \ref ruMap or \ref ruSet keys.
 */
typedef bool (*ruMatchFunc)(trans_ptr testKey, trans_ptr existingKey);

/**
 * \brief Signature of a generic comparator function for sorting.
 */
typedef int32_t (*ruCompFunc) (trans_ptr a, trans_ptr b);

/**
 * \brief Signature of a generic clone function.
 * This is the signature of the \a in function in \ref ruTypeNew.
 * @param val address where data to clone is stored
 */
typedef ptr (*ruCloneFunc)(ptr val);

/**
 * \brief Signature of a function that stores the value of the first pointer
 * in a certain type by de-referencing the second pointer.
 * This is the signature of the \a out function in \ref ruTypeNew.
 * @param src Where the outgoing value is stored, usually in the collection.
 * @param dst Where the value will be copied to and properly type casted.
 */
typedef int32_t (*ruPtr2TypeFunc)(ptr src, ptr* dst);


/**
 * \defgroup typeint64 64 Bit Integer
 * \brief This section contains collection support for 64 Bit Integers such as int64_t.
 * @{
 */
/**
 * \brief A pointer to an allocated 64 bit int. Free with \ref ruFree.
 */
typedef void* ru_Int64;

/**
 * \brief Returns a hash for given 64 bit integer number.
 *
 * @param key Number to hash
 * @return The hash of the given number.
 */
RUAPI ru_uint ruInt64Hash(trans_ptr key);

/**
 * \brief Convenience 64 bit integer match function for Maps.
 * @param s1 First comparison int.
 * @param s2 Second comparison int.
 * @return true if they are equal
 */
RUAPI bool ruInt64Match(trans_ptr s1, trans_ptr s2);

/**
 * \brief 64 bit integer comparator function for the \ref ruType system
 * @param testVal Value to compare against existing value.
 * @param existingVal The existing value to compare testVal to.
 * @return -1 if testVal is less existingVal,
 *          1 if testVal is greater existingVal or
 *          0 in case both of them are equal.
 */
RUAPI int32_t ruInt64Comp(trans_ptr testVal, trans_ptr existingVal);

/**
 * \brief Allocates given 64 bit integer on the heap and returns the pointer.
 * @param val address where int to allocate is stored
 * @return Pointer to allocated integer. Caller must free with \ref ruFree
 */
RUAPI ru_Int64 ruInt64(ptr val);

/**
 * \brief Copies the 64 bit integer stored in \a src to \a dst.
 * @param src Where the 64 bit integer is stored, usually in the collection.
 * @param dst Where the value will be copied to properly type casted.
 * @return \ref RUE_OK unless \a dst is not set.
 */
RUAPI int32_t ruRefPtrInt64(ptr src, ptr* dst);

/**
 * \brief Returns an \ref ruType used for 64 bit integers.
 *
 * Example:
 * ~~~~~{.c}
    // error checking left out for brevity
    int64_t k1 = 23, v1 = 42, r = 0;
    ruMap rm = ruMapNew(ruTypeInt64(), ruTypeInt64());
    ruMapPut(rm, &k1, &v1);

    ruMapGet(rm, &k1, &r);
    printf("k: %ld v: %ld\n", k1, r);
    ruMapFree(rm);
 * ~~~~~
 * @return 64 bit integer type specification. Caller need not free.
 */
RUAPI ruType ruTypeInt64(void);
/** @} */

/**
 * \defgroup typelong Long Integer
 * \brief This section contains collection support for long Integers such as long.
 * @{
 */
/**
 * \brief Returns a hash for given long integer number.
 *
 * @param key Number to hash
 * @return The hash of the given number.
 */
RUAPI ru_uint ruLongHash(trans_ptr key);

/**
 * \brief Convenience long integer match function for Maps.
 * @param s1 First comparison int.
 * @param s2 Second comparison int.
 * @return true if they are equal
 */
RUAPI bool ruLongMatch(trans_ptr s1, trans_ptr s2);

/**
 * \brief long integer comparator function for the \ref ruType system
 * @param testVal Value to compare against existing value.
 * @param existingVal The existing value to compare testVal to.
 * @return -1 if testVal is less existingVal,
 *          1 if testVal is greater existingVal or
 *          0 in case both of them are equal.
 */
RUAPI int32_t ruLongComp(trans_ptr testVal, trans_ptr existingVal);

/**
 * \brief Returns the content of the given long integer address as pointer
 * @param in address to long integer
 * @return content of given address in pointer sized field
 */
RUAPI ptr ruLongRefPtr(ptr in);

/**
 * \brief Copies the long integer stored in \a src to \a dst.
 * @param src Where the long integer is stored, usually in the collection.
 * @param dst Where the value will be copied to properly type casted.
 * @return \ref RUE_OK unless \a dst is not set.
 */
RUAPI int32_t ruRefPtrLong(ptr src, ptr* dst);

/**
 * \brief Returns an \ref ruType used for long integers.
 * Example:
 * ~~~~~{.c}
    // error checking left out for brevity
    long k1 = 23, v1 = 42, r = 0;
    ruMap rm = ruMapNew(ruTypeLong(), ruTypeLong());
    ruMapPut(rm, &k1, &v1);

    ruMapGet(rm, &k1, &r);
    printf("k: %ld v: %ld\n", k1, r);
    ruMapFree(rm);
 * ~~~~~
 * @return long integer type specification. Caller need not free.
 */
RUAPI ruType ruTypeLong(void);
/** @} */


/**
 * \defgroup typeint32 32 Bit Integer
 * \brief This section contains collection support for 32 Bit Integers such as int32_t.
 * @{
 */
/**
 * \brief Returns a hash for given 32 bit integer number.
 *
 * @param key Number to hash
 * @return The hash of the given number.
 */
RUAPI ru_uint ruInt32Hash(trans_ptr key);

/**
 * \brief Convenience 32 bit integer match function for Maps.
 * @param testKey First comparison int.
 * @param existingKey Second comparison int.
 * @return true if they are equal
 */
RUAPI bool ruInt32Match(trans_ptr testKey, trans_ptr existingKey);

/**
 * \brief 32 bit integer comparator function for the \ref ruType system
 * @param testVal Value to compare against existing value.
 * @param existingVal The existing value to compare testVal to.
 * @return -1 if testVal is less existingVal,
 *          1 if testVal is greater existingVal or
 *          0 in case both of them are equal.
 */
RUAPI int32_t ruInt32Comp(trans_ptr testVal, trans_ptr existingVal);

/**
 * \brief Returns the content of the given 32 bit integer address as pointer
 * @param in address to 32 bit integer
 * @return content of given address in pointer sized field
 */
RUAPI ptr ruInt32RefPtr(ptr in);

/**
 * \brief Copies the 32 bit integer stored in \a src to \a dst.
 * @param src Where the 32 bit integer is stored, usually in the collection.
 * @param dst Where the value will be copied to properly type casted.
 * @return \ref RUE_OK unless \a dst is not set.
 */
RUAPI int32_t ruRefPtrInt32(ptr src, ptr* dst);

/**
 * \brief Returns an \ref ruType used for 32 bit integers.
 * @return 32 bit integer type specification. Caller need not free.
 */
RUAPI ruType ruTypeInt32(void);
/** @} */


/**
 * \defgroup typeint16 16 Bit Integer
 * \brief This section contains collection support for 16 Bit Integers such as int16_t.
 * @{
 */
/**
 * \brief Returns a hash for given 16 bit integer number.
 *
 * @param key Number to hash
 * @return The hash of the given number.
 */
RUAPI ru_uint ruInt16Hash(trans_ptr key);

/**
 * \brief Convenience 16 bit integer match function for Maps.
 * @param testKey First comparison int.
 * @param existingKey Second comparison int.
 * @return true if they are equal
 */
RUAPI bool ruInt16Match(trans_ptr testKey, trans_ptr existingKey);

/**
 * \brief 16 bit integer comparator function for the \ref ruType system
 * @param testVal Value to compare against existing value.
 * @param existingVal The existing value to compare testVal to.
 * @return -1 if testVal is less existingVal,
 *          1 if testVal is greater existingVal or
 *          0 in case both of them are equal.
 */
RUAPI int32_t ruInt16Comp(trans_ptr testVal, trans_ptr existingVal);

/**
 * \brief Returns the content of the given 16 bit integer address as pointer
 * @param in address to 16 bit integer
 * @return content of given address in pointer sized field
 */
RUAPI ptr ruInt16RefPtr(ptr in);

/**
 * \brief Copies the 16 bit integer stored in \a src to \a dst.
 * @param src Where the 16 bit integer is stored, usually in the collection.
 * @param dst Where the value will be copied to properly type casted.
 * @return \ref RUE_OK unless \a dst is not set.
 */
RUAPI int32_t ruRefPtrInt16(ptr src, ptr* dst);

/**
 * \brief Returns an \ref ruType used for 16 bit integers.
 * @return 16 bit integer type specification. Caller need not free.
 */
RUAPI ruType ruTypeInt16(void);
/** @} */


/**
 * \defgroup typeint8 8 Bit Integer
 * \brief This section contains collection support for 8 Bit Integers such as int8_t.
 * @{
 */
/**
 * \brief Returns a hash for given 8 bit integer number.
 *
 * @param key Number to hash
 * @return The hash of the given number.
 */
RUAPI ru_uint ruInt8Hash(trans_ptr key);

/**
 * \brief Convenience 8 bit integer match function for Maps.
 * @param testKey First comparison int.
 * @param existingKey Second comparison int.
 * @return true if they are equal
 */
RUAPI bool ruInt8Match(trans_ptr testKey, trans_ptr existingKey);

/**
 * \brief 8 bit integer comparator function for the \ref ruType system
 * @param testVal Value to compare against existing value.
 * @param existingVal The existing value to compare testVal to.
 * @return -1 if testVal is less existingVal,
 *          1 if testVal is greater existingVal or
 *          0 in case both of them are equal.
 */
RUAPI int32_t ruInt8Comp(trans_ptr testVal, trans_ptr existingVal);

/**
 * \brief Returns the content of the given 8 bit integer address as pointer
 * @param in address to 8 bit integer
 * @return content of given address in pointer sized field
 */
RUAPI ptr ruInt8RefPtr(ptr in);

/**
 * \brief Copies the 8 bit integer stored in \a src to \a dst.
 * @param src Where the 8 bit integer is stored, usually in the collection.
 * @param dst Where the value will be copied to properly type casted.
 * @return \ref RUE_OK unless \a dst is not set.
 */
RUAPI int32_t ruRefPtrInt8(ptr src, ptr* dst);

/**
 * \brief Returns an \ref ruType used for 8 bit integers.
 * @return 8 bit integer type specification. Caller need not free.
 */
RUAPI ruType ruTypeInt8(void);
/** @} */


/**
 * \defgroup typebool Boolean
 * \brief This section contains collection support for booleans such as bool.
 * @{
 */
/**
 * \brief Returns a hash for given boolean number.
 *
 * This is nonsense for a map but is here since types are used as values as well.
 * @param key boolean to hash
 * @return The hash of the given boolean i.e 1 or 0.
 */
RUAPI ru_uint ruBoolHash(trans_ptr key);

/**
 * \brief Convenience boolean match function for Maps.
 * @param testKey First comparison bool.
 * @param existingKey Second comparison bool.
 * @return true if they are equal
 */
RUAPI bool ruBoolMatch(trans_ptr testKey, trans_ptr existingKey);

/**
 * \brief boolean comparator function for the \ref ruType system
 * @param testVal Value to compare against existing value.
 * @param existingVal The existing value to compare testVal to.
 * @return -1 if testVal is less existingVal,
 *          1 if testVal is greater existingVal or
 *          0 in case both of them are equal.
 */
RUAPI int32_t ruBoolComp(trans_ptr testVal, trans_ptr existingVal);

/**
 * \brief Returns the content of the given boolean address as pointer
 * @param in address to a boolean
 * @return content of given address in pointer sized field
 */
RUAPI ptr ruBoolRefPtr(ptr in);

/**
 * \brief Copies the boolean stored in \a src to \a dst.
 * @param src Where the boolean is stored, usually in the collection.
 * @param dst Where the value will be copied to properly type casted.
 * @return \ref RUE_OK unless \a dst is not set.
 */
RUAPI int32_t ruRefPtrBool(ptr src, ptr* dst);

/**
 * \brief Returns an \ref ruType used for booleans.
 *
 * @return boolean type specification. Caller need not free.
 */
RUAPI ruType ruTypeBool(void);
/** @} */


/**
 * \defgroup typestring Strings
 * \brief This section contains collection support for single byte strings such as char*.
 * @{
 */
/**
 * \brief Returns a hash for given string.
 *
 * This function is useful for Maps.
 * @param key String to hash.
 * @return The hash of the given string or 0 if NULL was given.
 */
RUAPI ru_uint ruStrHash(trans_ptr key);

/**
 * \brief Convenience match function for Maps.
 * @param s1 First comparison string.
 * @param s2 Second comparison string.
 * @return true if \ref ruStrCmp returns 0
 */
RUAPI bool ruStrMatch(trans_ptr s1, trans_ptr s2);

/**
 * \brief String comparator function for the \ref ruType system
 * @param testVal Value to compare against existing value.
 * @param existingVal The existing value to compare testVal to.
 * @return -1 if testVal is less existingVal,
 *          1 if testVal is greater existingVal or
 *          0 in case both of them are equal.
 */
RUAPI int32_t ruStrComp(trans_ptr testVal, trans_ptr existingVal);

/**
 * \brief Returns an \ref ruType used for permanent strings.
 *
 * Example:
 * ~~~~~{.c}
    // error checking left out for brevity
    perm_chars k1 = "23";
    perm_chars v1 = "42";
    ruMap rm = ruMapNew(ruTypeStrRef(), ruTypeStrRef());
    ruMapPut(rm, k1, v1);

    perm_chars r = NULL;
    ruMapGet(rm, "23", &r);
    printf("k: '23' v: '%s'\n", r);
    ruMapFree(rm);
 * ~~~~~
 * Given keys will neither be copied nor freed and must exist for the duration
 * of the map using them.
 * @return Permanent string type specification. Caller need not free.
 */
RUAPI ruType ruTypeStrRef(void);

/**
 * \brief Returns an \ref ruType used for transient strings.
 *
 * Example:
 * ~~~~~{.c}
    // error checking left out for brevity
    ruMap rm = ruMapNew(ruTypeStrDup(), ruTypeStrDup());
    ruMapPut(rm, "23", "42");

    perm_chars r = NULL;
    ruMapGet(rm, "23", &r);
    printf("k: '23' v: %ld\n", r);
    ruMapFree(rm);
 * ~~~~~
 * Given keys will be copied when added and freed on removal or \ref ruMapFree.
 * @return Duplicating string type specification. Caller need not free.
 */
RUAPI ruType ruTypeStrDup(void);

/**
 * \brief Returns an \ref ruType used for externally allocated string.
 *
 * Example:
 * ~~~~~{.c}
    // error checking left out for brevity
    ruMap rm = ruMapNew(ruTypeStrFree(), ruTypeStrFree());
    alloc_chars k = ruStrDup("23");
    alloc_chars v = ruStrDup("42");
    ruMapPut(rm, k, v);

    perm_chars r = NULL;
    ruMapGet(rm, "23", &r);
    printf("k: '23' v: '%s'\n", r);
    ruMapFree(rm);
 * ~~~~~
 * Given keys will not be copied when added but freed on removal or \ref ruMapFree.
 * @return Freeable string type specification. Caller need not free.
 */
RUAPI ruType ruTypeStrFree(void);
/** @} */


/**
 * \brief Returns an \ref ruType used for malloced types that use standard free.
 * @return Freeable type specification. Caller need not free.
 */
RUAPI ruType ruTypePtrFree(void);

/**
 * \brief Creates a custom type specification usable as a map key
 *
 * Example:
 * ~~~~~{.c}
    // error checking left out for brevity
    perm_chars k1 = "23";
    perm_chars k2 = "42";
    int64_t v1 = 42, v2 = 23, r = 0;

    // this is the same as returned by ruTypeStrRef()
    ruType ks = ruTypeNew(ruStrHash, ruStrMatch, ruStrComp, NULL, NULL, NULL);
    // this can also be accomplished by ruTypeInt64()
    ruType vs = ruTypeNew(NULL, NULL, NULL, NULL, ruInt64, ruRefPtrInt64);
    ruMap rm = ruMapNew(ks, vs);

    ruMapPut(rm, k1, &v1);
    ruMapPut(rm, k2, &v2);

    ruMapGet(rm, k1, &r);
    printf("k: '%s' v: %ld\n", k1, r);
    ruMapGet(rm, k2, &r);
    printf("k: '%s' v: %ld\n", k2, r);

    ruMapFree(rm);
 * ~~~~~
 *
 * @param hash The hashing function. Required for \ref ruMap or \ref ruSet keys.
 * @param match The matching function. Required for \ref ruMap or \ref ruSet keys.
 * @param comp A comparator function for list sorting functionality.
 * @param free The free function to be called when the type is removed from a collection.
 * @param in This function is called when storing the given value such as the
 *           \ref ruMapPut or \ref ruListAppend functions
 * @param out This function is called in retrieval functions to place the proper
 *            type in the recipient pointers.
 * @return New type specification for collections. This is usually freed by the
 *         collection that it's given to. Otherwise free with \ref ruTypeFree.
 */
RUAPI ruType ruTypeNew(ruHashFunc hash, ruMatchFunc match, ruCompFunc comp,
                       ruClearFunc free, ruCloneFunc in, ruPtr2TypeFunc out);

/**
 * \brief Creates a custom pointer type specification
 *
 * @param free The free function to be called when the type is removed from a collection.
 * @return New type specification for collections. This is usually freed by the
 *         collection that it's given to. Otherwise free with \ref ruTypeFree.
 */
RUAPI ruType ruTypePtr(ruClearFunc free);

/**
 * \brief Returns a copy of the given \ref ruType
 * @param tp \ref ruType to copy
 * @return A copy or reference of the given \ref ruType. Needs consuming or
 *         freeing only if the original did.
 */
RUAPI ruType ruTypeClone(ruType tp);

/**
 * \brief Frees given custom \ref ruType.
 *
 * This function need not be called on standard \ref ruType references from
 * \ref ruTypeInt64 or similar, but will handle these gracefully.
 * @param tp \ref ruType from \ref ruTypeNew or \ref ruTypePtr to free
 * @return NULL
 */
RUAPI ruType ruTypeFree(ruType tp);

/**
 * @}
 */

#endif //REGIFY_UTIL_TYPES_H
