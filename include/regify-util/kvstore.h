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
 * \defgroup kvstore_sec Key Value Storage
 * \brief A generic storage interface.
 *
 * \section kvstore_sec Key Value Storage
 *
 * Currently provided implementations are \ref ruFileStore for file system based
 * storage and \ref ruNullStore to omit local storage altogether.
 * It consists of a simple \ref kvget, \ref kvset and \ref kvlist call.
 *
 * \subsection kvstore Key Value Storage
 * A Key Value Store is a generic storage interface designed to allow for
 * flexible data serialization.
 *
 * \subsection kvkey kvstore key
 * This is a space separated string that also accept filesystem type globs like *.
 * Each intersecting space indicates a sub level much like a folder in a file
 * system, or a sub branch in a hierarchy. In most cases * is only supported at
 * the end.
 *
 * @{
 */

#ifndef REGIFY_UTIL_KVSTORE_H
#define REGIFY_UTIL_KVSTORE_H
/* Only need to export C interface if used by C++ source code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**
 * The key store container
 */
struct KvStore_;

/**
 * \brief KvStore setter function interface.
 * @param kvs \ref KvStore object.
 * @param key The \ref kvkey to the data in question.
 * @param val The value that will be stored or NULL if the key is to be removed.
 * @param len The number of bytes that make up val.
 * @return \ref RUE_OK on success else an error code.
 */
typedef int32_t (*kvset) (struct KvStore_* kvs, const char* key,
        const char* val, rusize_s len);

/**
 * \brief KvStore getter function interface.
 * @param kvs \ref KvStore object.
 * @param key The \ref kvkey to the data in question.
 * @param val Where the retrieved value will be stored. Most of the time the
 *            caller should free this with \ref ruFree when done with it.
 *            See implementation details.
 * @param len The number of bytes that make up val.
 * @return \ref RUE_OK on success else an error code.
 */
typedef int32_t (*kvget) (struct KvStore_* kvs, const char* key,
        char** val, rusize* len);

/**
 * \brief KvStore list function interface.
 * @param kvs KvStore context where data will be retrieved from.
 * @param key The \ref kvkey to the data in question. Should end with a *.
 * @param list An \ref ruList of \ref kvkey strings. Should be freed with
 *               \ref ruListFree after use.
 * @return \ref RUE_OK on success else an error code.
 */
typedef int32_t (*kvlist) (struct KvStore_* kvs, const char* key,
                          ruList* list);

/**
 * \brief The key/value store interface type.
 * This object holds the \ref kvget, \ref kvlist and \ref kvset methods of an
 * implementation.
 */
typedef struct KvStore_ {
    /** \brief The method that will be called to set data. */
    kvset set;
    /** \brief The method that will be called to retrieve data. */
    kvget get;
    /** \brief The method that will be called to list data keys. */
    kvlist list;
    /** \brief A user defined context that will be passed to all given methods. */
    void* ctx;
/** \cond noworry */
    // internal
    ruFreeFunc ctxFree;
    u_int32_t type;
/** \endcond */
} KvStore;
/** \cond noworry */
#define KvStoreMagic 23044203
/** \endcond */

/**
 * \brief Returns a generic \ref KvStore interface.
 * This function is mainly used by those implementing the KvStore interface.
 * @return A new empty \ref KvStore object;
 */
RUAPI KvStore* ruNewStore(void);

/**
 * \brief Public function to free the given \ref KvStore object.
 * @param obj The object to free
 */
RUAPI void ruFreeStore(void* obj);

/**
 * \brief Checks given object to be a valid \ref KvStore object.
 * @param obj The object to validate.
 * @return \ref RUE_OK if valid else an errro code.
 */
RUAPI int32_t ruValidStore(void* obj);

/**
 * \brief Opaque pointer to a File based KvStore interface.
 */
typedef void* ruFileStore;

/**
 * \brief Returns a newly created file system based \ref KvStore object.
 * If there is already store data in the given folder, it is opened and used.
 * @param folderPath Top level folder under which the data will be stored.
 * @param code Return code willbe \ref RUE_OK on success or error otherwise.
 * @return \ref KvStore object on success else NULL.
 */
RUAPI KvStore* ruNewFileStore (const char *folderPath, int32_t* code);

/**
 * \brief Set the value of key in the given FileStore.
 * @param kvs FileStore context where data will be stored.
 * @param key The \ref kvkey to the data in question.
 * @param val The value that will be stored or NULL if the key is to be removed.
 *            The value will be copied.
 * @param len The number of bytes that make up val.
 * @return \ref RUE_OK on success else an error code.
 */
RUAPI int32_t ruFileStoreSet (KvStore *kvs, const char* key, const char *val, rusize_s len);

/**
 * \brief Get the value of key from the given FileStore.
 * @param kvs FileStore context where data will be retrieved from.
 * @param key The \ref kvkey to the data in question.
 * @param val Where the retrieved value will be stored.
 *            Caller should free this with \ref ruFree when done with it.
 * @param len The number of bytes that make up val.
 * @return \ref RUE_OK on success else an error code.
 */
RUAPI int32_t ruFileStoreGet (KvStore *kvs, const char* key, char **val, rusize* len);

/**
 * \brief Returns a list of keys under the given key.
 * @param kvs FileStore context where key will be listed from.
 * @param key The \ref kvkey to the data in question. May end with " *"
 * @param result An \ref ruList of \ref kvkey strings. Should be freed with
 *               \ref ruListFree after use.
 * @return \ref RUE_OK on success else an error code.
 */
RUAPI int32_t ruFileStoreList (KvStore *kvs, const char* key, ruList* result);

/**
 * \brief Opaque pointer to a Null KvStore interface.
 */
typedef void* ruNullStore;

/**
 * \brief Returns a newly created NULL based KvStore interface.
 * This keystore silently ignores setters and never returns data.
 * @return \ref KvStore object.
 */
RUAPI KvStore* ruNewNullStore(void);

/**
 * \brief Set the value of key in the given FileStore.
 * @param kvs KvStore context where data will be stored.
 * @param key The \ref kvkey to the data in question.
 * @param val The value that will be stored or NULL if the key is to be removed.
 * @param len The number of bytes that make up val.
 * @return \ref RUE_OK on success else an error code.
 */
RUAPI int32_t ruNullStoreSet (KvStore *kvs, const char* key, const char *val, rusize_s len);

/**
 * \brief Get the value of key from the given KvStore.
 * @param kvs KvStore context where data will be retrieved from.
 * @param key The \ref kvkey to the data in question.
 * @param val Where the retrieved value will be stored. Will always be NULL.
 * @param len The number of bytes that make up val. Will always be 0.
 * @return \ref RUE_OK on success else an error code.
 */
RUAPI int32_t ruNullStoreGet (KvStore *kvs, const char* key, char **val, rusize* len);

/**
 * \brief Returns a list of items under given key.
 * @param kvs KvStore context where data will be retrieved from.
 * @param key The \ref kvkey to the data in question. Should end with a *.
 * @param list A NULL \ref ruList of keys that would normally be found.
 * @return \ref RUE_OK on success else an error code.
 */
RUAPI int32_t ruNullStoreList (KvStore *kvs, const char* key, ruList* list);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_KVSTORE_H
