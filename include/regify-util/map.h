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
 * \defgroup map Hash Map
 * \brief This section contains a hash map implementation.
 * @{
 */
#ifndef REGIFY_UTIL_MAP_H
#define REGIFY_UTIL_MAP_H
/* Only need to export C interface if used by C++ source code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief An opaque type representing a map object.
 */
typedef void* ruMap;

/**
 * \brief Creates a new thread safe map object.
 * This is the long but flexible version to create a hash map.
 * @param hash A required function that returns a hash for the given key to be
 *             used internally to determine bucket assignments.
 * @param match A required function that determines whether the 2 given keys are
 *              equal.
 * @param keyFree An optional function to be called when a key is to be
 *                finalized (deleted).
 * @param valFree An optional function to be called when a value is to be
 *                finalized (deleted).
 * @param expectedSize An optional or 0 initial estimate of how many members
 *                     the map will have.
 * @return The newly created map object to be freed with \ref ruMapFree or NULL
 *         if required arguments weren't given.
 */
RUAPI ruMap ruMapNew(u_int32_t (*hash)(const void *key),
                     bool (*match)(const void* key1, const void* key2),
                     ruFreeFunc keyFree, ruFreeFunc valFree,
                     u_int32_t expectedSize);

/**
 * \brief Returns a hash for given number.
 *
 * @param key Number to hash
 * @return The hash of the given number which is itself.
 */
RUAPI u_int32_t ruIntHash(const void* key);

/**
 * \brief Convenience integer match function for Maps.
 * @param s1 First comparison int.
 * @param s2 Second comparison int.
 * @return true if they are equal
 */
RUAPI bool ruIntMatch(const void* s1, const void* s2);

/**
 * \brief Returns a hash for given string.
 *
 * This function is useful for Maps.
 * @param key String to hash.
 * @return The hash of the given string or 0 if NULL was given.
 */
RUAPI u_int32_t ruStrHash(const void* key);

/**
 * \brief Convenience match function for Maps.
 * @param s1 First comparison string.
 * @param s2 Second comparison string.
 * @return true if \ref ruStrcmp returns 0
 */
RUAPI bool ruStrMatch(const void* s1, const void* s2);

/**
 * \brief A convenience constructor for maps with string keys.
 * @param keyFree An optional function to be called when a key is to be
 *                finalized (deleted).
 * @param valFree An optional function to be called when a value is to be
 *                finalized (deleted).
 * @return The newly created map object to be freed with \ref ruMapFree.
 */
RUAPI ruMap ruMapNewString(ruFreeFunc keyFree, ruFreeFunc valFree);

/**
 * \brief Frees the given map and its members
 * @param rm Map to free
 * @return NULL
 */
RUAPI ruMap ruMapFree(ruMap rm);

/**
 * \brief Insert a key/val pair into the map.
 * @param map Map to insert key / value pair into.
 * @param key The key to insert.
 * @param val The associated value to go with the key.
 * @return \ref RUE_OK on success \ref RUE_USER_ABORT when a threaded map has
 *          quit else a regify error code.
 */
RUAPI int32_t ruMapPutData(ruMap map, void *key, void *val);

/**
 * \brief Runs \ref ruMapPutData with void* casts.
 * @param map Map to insert key / value pair into.
 * @param key The key to insert.
 * @param val The associated value to go with the key.
 * @return \ref RUE_OK on success \ref RUE_USER_ABORT when a threaded map has
 *          quit else a regify error code.
 */
#define ruMapPut(map, key, val) ruMapPutData(map, (void*)(key), (void*)(val))

/**
 * \brief Removes an entry from the map.
 * @param rm The map to remove the entry from.
 * @param key The key to be removed.
 * @param val (Optional) Where to store the retrieved value associated wiht the
 *            key.
 * @return \ref RUE_OK on success \ref RUE_USER_ABORT when a threaded map has
 *          quit else a regify error code.
 */
RUAPI int32_t ruMapRemoveData(ruMap rm, void *key, void **val);

/**
 * \brief Runs \ref ruMapRemoveData with void* casts.
 * @param rm The map to remove the entry from.
 * @param key The key to be removed.
 * @param val (Optional) Where to store the retrieved value associated wiht the
 *            key.
 * @return \ref RUE_OK on success \ref RUE_USER_ABORT when a threaded map has
 *          quit else a regify error code.
 */
#define ruMapRemove(rm, key, val) ruMapRemoveData(rm, (void*)(key), (void**)(val))

/**
 * \brief Removes all entries from the map.
 * @param rm The map to remove the entry from.
 * @return \ref RUE_OK on success \ref RUE_USER_ABORT when a threaded map has
 *          quit else a regify error code.
 */
RUAPI int32_t ruMapRemoveAll(ruMap rm);

/**
 * \brief Retrieves an entry from the map.
 * @param rm The map to retrieve the entry from.
 * @param key The key to be retrieved.
 * @param val Where to store the retrieved value associated with the key.
 * @return \ref RUE_OK on success \ref RUE_USER_ABORT when a threaded map has
 *          quit else a regify error code.
 */
RUAPI int32_t ruMapGetValue(ruMap rm, void *key, void **val);

#define ruMapGet(rm, key, val) ruMapGetValue(rm, (void*)(key), (void**)(val))

/**
 * \brief Tests whether map has an entry for given key.
 * @param rm The map to check.
 * @param key The key to be searched.
 * @param code (Optional) Stores \ref RUE_OK on success
 *         \ref RUE_USER_ABORT when a threaded map has quit
 *         else a regify error code.
 * @return true if key exists or false on error or if not.
 */
RUAPI bool ruMapHasKey(ruMap rm, void *key, int32_t *code);

/**
 * \brief Runs \ref ruMapHasKey with a void* casted key.
 * @param rm The map to check.
 * @param key The key to be searched.
 * @param code (Optional) Stores \ref RUE_OK on success
 *         \ref RUE_USER_ABORT when a threaded map has quit
 *         else a regify error code.
 * @return true if key exists or false on error or if not.
 */
#define ruMapHas(rm, key, code) ruMapHasKey(rm, (void*)(key), code)

/**
 * Initializes a map iterator and populates given parameters with the first
 * key pair to serve as a for loop initializer.
 *
 * Example:
 * ~~~~~{.c}
 * int ret;
 * void *key = NULL, *val = NULL;
 * for (ret = ruMapFirst(map, &key, &val); ret == RUE_OK;
 *     ret = ruMapNext(map, &key, &val)) {
 *     // work with key and/or val
 * }
 * ~~~~~
 *
 * NOTE: Either key or value must be set.
 * @param rm The map to iterate over.
 * @param key Where to store the current key. (optional)
 * @param value Where to store the current value. (optional)
 * @return \ref RUE_OK on success
 *         \ref RUE_FILE_NOT_FOUND at the end of the set
 *         \ref RUE_USER_ABORT when a threaded map has quit
 *         else a regify error code.
 */
RUAPI int32_t ruMapFirstSet(ruMap rm, void **key, void **value);

/**
 * Runs \ref ruMapFirstSet with void* casts
 */
#define ruMapFirst(rm, key, value) ruMapFirstSet(rm, (void **)(key), (void **)(value))

/**
 * \brief Retrieves the next key/value pair from the map.
 *
 * NOTE: Either key or value must be set.
 * @param rm The map to retrieve the entry from.
 * @param key Where to store the current key. (optional)
 * @param value Where to store the current value. (optional)
 * @return \ref RUE_OK on success,
 *         \ref RUE_FILE_NOT_FOUND at the end of the set
 *         \ref RUE_INVALID_STATE if \ref ruMapFirstSet hasn't been called or
 *              been invalidated.
 *         \ref RUE_USER_ABORT when a threaded map has quit
 *         else a regify error code.
 */
RUAPI int32_t ruMapNextSet(ruMap rm, void **key, void **value);

/**
 * Runs \ref ruMapNextSet with void* casts
 */
#define ruMapNext(rm, key, value) ruMapNextSet(rm, (void **)(key), (void **)(value))

/**
 * \brief Return a keyset of the giuven map.
 *
 * @param rm The map to get the keyset from.
 * @param copy Optional copy function with which the keys are copied.
 * @param keys Where to store the resulting list.
 * @param listFree Optional free function for the list or NULL to use the maps
 *        keyfree function.
 * @return \ref RUE_OK on success,
 *         \ref RUE_USER_ABORT when a threaded map has quit
 *         else a regify error code.
 */
RUAPI int32_t ruMapKeySet(ruMap rm, ruCloneFunc copy, ruList* keys, ruFreeFunc listFree);

/**
 * \brief Returns the size of the map.
 * @param rm Map to return the size of.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return \ref RUE_OK on success,
 *         \ref RUE_USER_ABORT when a threaded map has quit
 *         else a regify error code.
 */
RUAPI u_int32_t ruMapSize(ruMap rm, int32_t *code);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_MAP_H
