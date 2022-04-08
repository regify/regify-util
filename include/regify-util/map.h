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
 * \brief Creates a new map object.
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
                   bool (*match)(const void *key1, const void *key2),
                   void (*keyFree)(void *key),
                   void (*valFree)(void *val), u_int32_t expectedSize);

/**
 * \brief Returns a hash for given string.
 *
 * This function is useful for Maps.
 * @param key String to hash.
 * @return The hash of the given string or 0 if NULL was given.
 */
RUAPI u_int32_t ruStrHash(const void *key);

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
RUAPI ruMap ruMapNewString(void (*keyFree)(void *key),
                     void (*valFree)(void *val));

/**
 * \brief Frees the given map and its members
 * @param rm Map to free````
 */
RUAPI void ruMapFree(ruMap rm);

/**
 * \brief Insert a key/val pair into the map.
 * @param map Map to insert key / value pair into.
 * @param key The key to insert.
 * @param val The associated value to go with the key.
 * @return \ref RUE_OK on success else a regify error code.
 */
RUAPI int32_t ruMapPut(ruMap map, void *key, void *val);

/**
 * \brief Removes an entry from the map.
 * @param rm The map to remove the entry from.
 * @param key The key to be removed.
 * @param val (Optional) Where to store the retrieved value associated wiht the
 *            key.
 * @return \ref RUE_OK on success else a regify error code.
 */
RUAPI int32_t ruMapRemove(ruMap rm, void *key, void **val);

/**
 * \brief Removes all entries from the map.
 * @param rm The map to remove the entry from.
 * @return \ref RUE_OK on success else a regify error code.
 */
RUAPI int32_t ruMapRemoveAll(ruMap rm);

/**
 * \brief Retrieves an entry from the map.
 * @param rm The map to retrieve the entry from.
 * @param key The key to be retrieved.
 * @param val Where to store the retrieved value associated with the key.
 * @return \ref RUE_OK on success else a regify error code.
 */
RUAPI int32_t ruMapGetValue(ruMap rm, void *key, void **val);

#define ruMapGet(rm, key, val) ruMapGetValue(rm, (void*)(key), (void**)(val))

/**
 * \brief Tests whether map has an entry for given key.
 * @param rm The map to check.
 * @param key The key to be searched.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return true if key exists or false on error or if not.
 */
RUAPI bool ruMapHas(ruMap rm, void *key, int32_t *code);

/**
 * \brief Initialize an iterator to cycle over all map elements with \ref ruMapNext.
 *
 * Any map modifications invalidate the iterator state.
 * @param rm The map to iterate over.
 * @return \ref RUE_OK on success else a regify error code.
 */
RUAPI int32_t ruMapIterInit(ruMap rm);

/**
 * \brief Retrieves the next key/value pair from the map.
 *
 * NOTE: Either key or value must be set.
 * @param rm The map to retrieve the entry from.
 * @param key Where to store the current key. (optional)
 * @param value Where to store the current value. (optional)
 * @return \ref RUE_OK on success, \ref RUE_FILE_NOT_FOUND at the end of the
 *         set or \ref RUE_INVALID_STATE if \ref ruMapIterInit hasn't been
 *         called or been invalidated.
 */
RUAPI int32_t ruMapNextSet(ruMap rm, void **key, void **value);

#define ruMapNext(rm, key, value) ruMapNextSet(rm, (void **)(key), (void **)(value))

/**
 * \brief Returns the size of the map.
 * @param rm Map to return the size of.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return
 */
RUAPI u_int32_t ruMapSize(ruMap rm, int32_t *code);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_MAP_H
