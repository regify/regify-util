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
 * \defgroup list List Collection
 * \brief This section contains a singly linked list implementation.
 *
 * Example:
 * ~~~~~{.c}
 * ruList recipients = ruListNew(NULL);
 * // We are going to ignore some return codes for brevity.
 * int32_t ret = ruListAppend(recipients, "bob");
 * ret = ruListAppend(recipients, "alice");
 * ruIterator li = ruListIter(recipients);
 * for(char* recip = ruIterCurrent(li, char*);
 *         li; recip = ruIterNext(li, char*)) {
 *     printf("Recipient: %s", recip);
 * }
 * ruListFree(recipients);
 * ~~~~~
 *
 * @{
 */
#ifndef REGIFY_UTIL_LIST_H
#define REGIFY_UTIL_LIST_H
/* Only need to export C interface if used by C++ source code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*****************************************************************************
*                                                                            *
*  ------------------------ Public List Interface -------------------------  *
*                                                                            *
*****************************************************************************/
/**
 * Opaque pointer to list object. See \ref list
 */
typedef void* ruList;

/**
 * Opaque pointer to list element object. See \ref list
 */
typedef void* ruListElmt;

/**
 * Opaque pointer to list iterator object. See \ref list
 */
typedef void* ruIterator;

/**
 * Creates a new list object. To be freed with \ref ruListFree.
 * @param destructor (Optional) destructor for list elements.
 * @return Guaranteed to return new list object, or process abort.
 */
RUAPI ruList ruListNew(void (*destructor)(void *data));

/**
 * Frees the given list object.
 * @param rl list to free.
 */
RUAPI void ruListFree(ruList rl);

/**
 * Appends the given object to the list.
 * @param rl List to append object to.
 * @param data Object to append.
 * @return \ref RUE_OK on success or regify error code.
 */
RUAPI int32_t ruListAppendPtr(ruList rl, const void *data);

/**
 * Calls \ref ruListAppendPtr but handles the void* cast.
 * @param rl List to append object to.
 * @param data Object to append.
 * @return \ref RUE_OK on success or regify error code.
 */
#define ruListAppend(rl, data) ruListAppendPtr(rl, (void*)data)

/**
 * Stack usage synonym for \ref ruListAppend.
 * @param rl List to append object to.
 * @param data Object to append.
 * @return \ref RUE_OK on success or regify error code.
 */
#define ruListPush(rl, data) ruListAppendPtr(rl, (void*)data)

/**
 * Inserts given object after given list element.
 * @param rl List in which to insert object.
 * @param rle Element after which to insert object. NULL for first element.
 * @param data Object to insert.
 * @return \ref RUE_OK on success or regify error code.
 */
RUAPI int32_t ruListInsertAfter(ruList rl, ruListElmt rle, const void *data);

/**
 * Remove and return the list element after the given element from list.
 * @param rl List to remove object from.
 * @param rle Element after which to remove object. NULL for first element.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Object which was removed from list.
 */
RUAPI void* ruListRemoveAfter(ruList rl, ruListElmt rle, int32_t *code);

/**
 * Stack usage synonym for \ref ruListRemoveAfter at head.
 * @param rl List to pop object from.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Object which was popped off of the list.
 */
#define ruListPop(rl, code) ruListRemoveAfter(rl, NULL, code)

/**
 * Returns the number of elements in the list.
 * @param rl List to return size of.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Number of elements in list.
 */
RUAPI int32_t ruListSize(ruList rl, int32_t* code);

/**
 * Returns the first element in the given list.
 * @param rl List to return element of.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return The first element of the list or NULL on failure.
 */
RUAPI ruListElmt ruListHead(ruList rl, int32_t* code);

/**
 * Returns the first element in the given list to iterate over.
 * @param rl List to return element of.
 * @return The first element of the list or NULL on failure.
 */
#define ruListIter(list) ruListHead(list, NULL)

/**
 * Returns the last element in the given list.
 * @param rl List to return element of.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return The last element of the list or NULL on failure.
 */
RUAPI ruListElmt ruListTail(ruList rl, int32_t* code);

/**
 * Indicates whether given element is the first in the given list.
 * @param rl List in question.
 * @param re List element in question
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return true if the element is the first in the list.
 */
RUAPI bool ruListElmtIsHead(ruList rl, ruListElmt re, int32_t* code);

/**
 * Indicates whether given element is the last in the given list.
 * @param re List element in question
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return true if the element is the last in the list.
 */
RUAPI bool ruListElmtIsTail(ruListElmt re, int32_t* code);

/**
 * Return the next element following the given element from list.
 * @param re List element to return follower of.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Next element or NULL if given element was the last or an error occured.
 */
RUAPI ruListElmt ruListNextElmt(ruListElmt re, int32_t* code);

/**
 * Returns the data payload of the next list element.
 * @param re Current list element to get the follower of.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return NULL if error or given element was the last or the data of the next
 *         element.
 */
RUAPI void* ruListNextData(ruListElmt *re, int32_t* code);

/**
 * Returns the data payload of the next list element casted to type.
 * @param re Current list element to get the follower of.
 * @param type Data type to cast the returned data payload to.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return NULL if error or given element was the last or the data of the next
 *         element.
 */
#define ruListNext(re, type, code) (type) ruListNextData(re, code)

/**
 * Returns the data payload of the next list element casted to type.
 * @param re Current list element to get the follower of.
 * @param type Data type to cast the returned data payload to.
 * @return NULL if given element was the last or the data of the next element.
 */
#define ruIterNext(re, type) (type) ruListNextData(&(re), NULL)

/**
 * Returns the data payload of the given list element.
 * @param re List element to return data from.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Data of the given element including NULL or NULL if there was an error.
 */
RUAPI void* ruListElmtData(ruListElmt re, int32_t* code);

/**
 * Returns the data payload of the given list element casted to type.
 * @param re List element to return data from.
 * @param type Data type to cast the returned data payload to.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Data of the given element including NULL or NULL if there was an error.
 */
#define ruListData(re, type, code) (type)ruListElmtData(re, code)

/**
 * Returns the data payload of the given list element casted to type.
 * @param re List element to return data from.
 * @param type Data type to cast the returned data payload to.
 * @return Data of the given element including NULL or NULL if there was an error.
 */
#define ruIterCurrent(re, type) (type)ruListElmtData(re, NULL)

/**
 * Returns the data payload of the element at given 0 based index.
 * @param rl List from which to return the element data payload.
 * @param index Index of the element in question. 0 is the first element.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Data of the given element including NULL or NULL if there was an error.
 */
RUAPI void* ruListIdxElmtData(ruList rl, int32_t index, int32_t* code);

/**
 * Returns the data payload of the element at given 0 based index casted to type.
 * @param rl List from which to return the element data payload.
 * @param index Index of the element in question. 0 is the first element.
 * @param type Data type to cast the returned data payload to.
 * @param code (Optional) Stores \ref RUE_OK on success or regify error code.
 * @return Data of the given element including NULL or NULL if there was an error.
 */
#define ruListIdxData(rl, index, type, code) (type)ruListIdxElmtData(rl, index, code)

/**
 * @}
 */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_LIST_H
