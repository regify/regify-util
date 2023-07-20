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

ruMakeTypeGetter(ListElmt, MagicListElmt)
ruMakeTypeGetter(List, MagicList)

List* ListNewType(ruType vt) {
    typeSpec* vs = typeSpecGet(vt, NULL);
    if (vt && !vs) {
        ruCritLog("Failed map creation due an invalid valSpec parameter");
        ruAbort();
    }
    List *list = ruMalloc0(1, List);
    if (vs) {
        list->destroy = vs->free;
        list->valIn = vs->in;
        list->valOut = vs->out;
        list->valSort = vs->comp;
    }
    vs = ruTypeFree(vs);
    list->type = MagicList;
    list->mux = ruMutexInit();
    // create the root node
    list->head = ruMalloc0(1, ListElmt);
    list->head->type = MagicListElmt;
    list->head->list = list;
    list->head->prev = list->head;
    list->tail = list->head;
    return list;
}

static inline void runValIn(ListElmt* item, ptr data) {
    List *list = item->list;
    if (list->valIn) {
        item->data = list->valIn(data);
    } else {
        item->data = data;
    }
}

static int32_t runValOut(ListElmt* el, ptr* target, bool moving) {
    if (!el) return RUE_PARAMETER_NOT_SET;
    if (!target) return RUE_OK;
    List *list = el->list;
    if (list->valOut) {
        return list->valOut(el->data, target);
    } else {
        *target = el->data;
        if (moving) el->data = NULL;
    }
    return RUE_OK;
}


static void ListClear(List *list) {
    // Remove each element.
    while (list->size > 0) {
        ListRemoveTo(list, list->head->next, NULL);
    }
}

static ListElmt* ListIndex(List *list, int32_t index, int32_t* code) {
    uint32_t offset = abs(index);
    if (index < 0) {
        // we get one more backwards
        if (offset > list->size) ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL);
    } else {
        if (offset >= list->size) ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL);
    }
    ListElmt* rle = NULL;
    if (index < 0) {
        rle = list->tail;
        index++;
        while(index && rle) {
            index++;
            rle = rle->prev;
        }

    } else {
        rle = list->head->next;
        while(index && rle) {
            rle = rle->next;
            index--;
        }
    }
    ruRetWithCode(code, RUE_OK, rle);
}

void ListFree(List *list) {
    ListClear(list);
    memset(list->head, 0, sizeof(ListElmt));
    ruFree(list->head);
    // No operations are allowed now, but clear the structure as a precaution.
    list->mux = ruMutexFree(list->mux);
    memset(list, 0, sizeof(List));
    ruFree(list);
}

int32_t ListInsertAfter(List *list, ruListElmt rle, ptr data) {
    int32_t ret;
    ListElmt *element = NULL;
    if (!rle) {
        element = list->head;
    } else {
        element = ListElmtGet(rle, &ret);
        if (ret != RUE_OK) return ret;
    }
    ListElmt* new_element = ruMalloc0(1, ListElmt);
    new_element->type = MagicListElmt;
    new_element->list = list;
    runValIn(new_element, data);
    /* Adjust the size of the list to account for the inserted element. */
    list->size++;

    ListElmt* next_element = element->next;
    if (next_element == NULL) {
        list->tail = new_element;
    } else {
        next_element->prev = new_element;
    }
    new_element->next = next_element;
    new_element->prev = element;
    element->next = new_element;
    return RUE_OK;
}

int32_t ListRemoveTo(List* list, ListElmt* old_element, ptr* dest) {
    if (!old_element) {
        return RUE_PARAMETER_NOT_SET;
    }
    if (!list->size) {
        // Do not allow removal from an empty list
        return RUE_INVALID_STATE;
    }
    if (old_element == old_element->prev) {
        // do not allow removing head
        return RUE_INVALID_PARAMETER;
    }
    int32_t ret = RUE_OK;
    if (dest) {
        ret = runValOut(old_element, dest, true);
    }
    if (list->destroy && old_element->data) {
        old_element->data = list->destroy(old_element->data);
    }

    // Remove the element from the list
    ListElmt* prev = old_element->prev;
    ListElmt* next = old_element->next;
    prev->next = next;
    if (next) {
        next->prev = prev;
    } else {
        list->tail = prev;
    }
    /* Free the storage allocated by the abstract data type. */
    memset(old_element, 0, sizeof(ListElmt));
    free(old_element);
    /* Adjust the size of the list to account for the removed element. */
    list->size--;
    return ret;
}

RUAPI ruList ruListNew(ruType valueType) {
    ruClearError();
    return (ruList) ListNewType(valueType);
}

RUAPI ruList ruListFree(ruList rl) {
    ruClearError();
    List *list = ListGet(rl, NULL);
    if (!list) return NULL;
    list->doQuit = true;
    ruMutexLock(list->mux);
    ruMutexUnlock(list->mux);
    ListFree(list);
    return NULL;
}

RUAPI int32_t ruListClear(ruList rl) {
    ruClearError();
    int32_t ret;
    List *list = ListGet(rl, &ret);
    if (ret != RUE_OK) return ret;
    if (list->doQuit) return RUE_USER_ABORT;
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        return RUE_USER_ABORT;
    }
    ListClear(list);
    ruMutexUnlock(list->mux);
    return RUE_OK;
}

RUAPI int32_t ruListAppendPtr(ruList rl, perm_ptr data) {
    ruClearError();
    int32_t ret;
    List *list = ListGet(rl, &ret);
    if (!list) return ret;
    if (list->doQuit) return RUE_USER_ABORT;
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        return RUE_USER_ABORT;
    }
    ret = ListInsertAfter(list, list->tail, (ptr)data);
    ruMutexUnlock(list->mux);
    return ret;
}

RUAPI int32_t ruListInsertIdx(ruList rl, int32_t index, perm_ptr data) {
    ruClearError();
    int32_t code;
    List *list = ListGet(rl, &code);
    if (!list) return code;
    if (list->doQuit) return RUE_USER_ABORT;
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        return RUE_USER_ABORT;
    }
    ListElmt* rle = ListIndex(list, index, &code);
    if (code != RUE_OK) {
        rle = list->tail;
    } else {
        rle = rle->prev;
    }
    code = ListInsertAfter(list, rle, (ptr)data);
    ruMutexUnlock(list->mux);
    return code;
}

RUAPI int32_t ruListInsertBefore(ruList rl, ruListElmt rle, perm_ptr data) {
    ruClearError();
    int32_t code;
    List *list = ListGet(rl, &code);
    if (!list) return code;
    if (list->doQuit) return RUE_USER_ABORT;
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        return RUE_USER_ABORT;
    }
    ListElmt *element = NULL;
    if (!rle) {
        element = list->head;
    } else {
        element = ListElmtGet(rle, &code);
        if (!element) {
            ruMutexUnlock(list->mux);
            return code;
        }
        element = element->prev;
    }
    code = ListInsertAfter(list, element, (ptr)data);
    ruMutexUnlock(list->mux);
    return code;
}

RUAPI int32_t ruListInsertAfter(ruList rl, ruListElmt rle, perm_ptr data) {
    ruClearError();
    int32_t code;
    List *list = ListGet(rl, &code);
    if (!list) return code;
    if (list->doQuit) return RUE_USER_ABORT;
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        return RUE_USER_ABORT;
    }
    code = ListInsertAfter(list, rle, (ptr)data);
    ruMutexUnlock(list->mux);
    return code;
}

RUAPI int32_t ruListRemoveDataTo(ruList rl, ruListElmt* rle, ptr* dest) {
    int32_t ret;
    List *list = ListGet(rl, &ret);
    if (!list) return ret;
    if (list->doQuit) return RUE_USER_ABORT;
    ListElmt *elem = ListElmtGet(*rle, &ret);
    if (!elem) return ret;
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        return RUE_USER_ABORT;
    }
    *rle = (ruListElmt)elem->prev;
    ret = ListRemoveTo(list, elem, dest);
    ruMutexUnlock(list->mux);
    return ret;
}

RUAPI ptr ruListRemove(ruList rl, ruListElmt* rle, int32_t* code) {
    ptr dest = NULL;
    int32_t ret = ruListRemoveDataTo(rl, rle, &dest);
    ruRetWithCode(code, ret, dest);
}

RUAPI int32_t ruListRemoveIdxDataTo(ruList rl, int32_t index, ptr* dest) {
    int32_t ret;
    ruClearError();
    List *list = ListGet(rl, &ret);
    if (!list) return ret;
    if (!dest) return RUE_PARAMETER_NOT_SET;
    if (list->doQuit) return RUE_USER_ABORT;
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        return RUE_USER_ABORT;
    }
    ListElmt* rle = ListIndex(list, index, &ret);
    if (ret == RUE_OK) {
        ret = ListRemoveTo(list, rle, dest);
    }
    ruMutexUnlock(list->mux);
    return ret;
}

RUAPI ptr ruListRemoveIdx(ruList rl, int32_t index, int32_t* code) {
    ptr dest = NULL;
    int32_t ret = ruListRemoveIdxDataTo(rl, index, &dest);
    ruRetWithCode(code, ret, dest);
}


RUAPI int32_t ruListPopDataTo(ruList rl, ptr* dest) {
    int32_t ret;
    List *list = ListGet(rl, &ret);
    if (!list) return ret;
    if (list->doQuit) return RUE_USER_ABORT;
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        return RUE_USER_ABORT;
    }
    ListElmt *element = NULL;
    do {
        element = list->head;
        if (element->next == NULL) {
            ret = RUE_FILE_NOT_FOUND;
            break;
        }
        ret = ListRemoveTo(list, element->next, dest);
    } while (0);
    ruMutexUnlock(list->mux);
    return ret;
}

RUAPI ptr ruListPop(ruList rl, int32_t *code) {
    ptr dest = NULL;
    int32_t ret = ruListPopDataTo(rl, &dest);
    ruRetWithCode(code, ret, dest);
}

RUAPI uint32_t ruListSize(ruList rl, int32_t *code) {
    ruClearError();
    List *list = ListGet(rl, code);
    if (!list) return 0;
    return list->size;
}

RUAPI ruListElmt ruListHead(ruList rl, int32_t* code) {
    ruClearError();
    List *list = ListGet(rl, code);
    if (!list) return NULL;
    return list->head;
}

RUAPI ruListElmt ruListTail(ruList rl, int32_t* code) {
    ruClearError();
    List *list = ListGet(rl, code);
    if (!list) return NULL;
    return list->tail;
}

RUAPI bool ruListElmtIsHead(ruListElmt re, int32_t* code) {
    ruClearError();
    ListElmt *el = ListElmtGet(re, code);
    if (!el) return false;
    List *list = el->list;
    bool ret = false;
    if (list->doQuit) ruRetWithCode(code, RUE_USER_ABORT, ret);
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        ruRetWithCode(code, RUE_USER_ABORT, ret);
    }
    if (el && el == list->head) ret = true;
    ruMutexUnlock(list->mux);
    return ret;
}

RUAPI bool ruListElmtIsTail(ruListElmt re, int32_t* code) {
    ruClearError();
    ListElmt *el = ListElmtGet(re, code);
    if (!el) return false;
    if (el->next == NULL) return true;
    return false;
}

RUAPI ruListElmt ruListNextElmt(ruListElmt re, int32_t* code) {
    ruClearError();
    ListElmt *el = ListElmtGet(re, code);
    if (!el) return NULL;
    return (ruListElmt)el->next;
}

RUAPI int32_t ruListNextDataTo(ruListElmt *re, ptr* dest) {
    int32_t ret;
    ruClearError();
    if (!re) return RUE_PARAMETER_NOT_SET;
    ListElmt *el = ListElmtGet(*re, &ret);
    if (!el) return ret;
    (*re) = (ruListElmt)el->next;
    if (el->next) {
        return runValOut(el->next, dest, false);
    }
    return ret;
}

RUAPI ptr ruListNextData(ruListElmt *re, int32_t* code) {
    ptr dest = NULL;
    int32_t ret = ruListNextDataTo(re, &dest);
    ruRetWithCode(code, ret, dest);
}

RUAPI int32_t ruListIdxDataTo(ruList rl, int32_t index, ptr* dest) {
    int32_t ret;
    ruClearError();
    List *list = ListGet(rl, &ret);
    if (!list) return ret;
    if (!dest) return RUE_PARAMETER_NOT_SET;
    if (list->doQuit) return RUE_USER_ABORT;
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        return RUE_USER_ABORT;
    }
    ListElmt* rle = ListIndex(list, index, &ret);
    ruMutexUnlock(list->mux);
    if (ret != RUE_OK) return ret;
    if (!rle) return RUE_OK;

    return runValOut(rle, dest, false);
}

RUAPI ptr ruListIdxData(ruList rl, int32_t index, int32_t* code) {
    ptr dest = NULL;
    int32_t ret = ruListIdxDataTo(rl, index, &dest);
    ruRetWithCode(code, ret, dest);
}

RUAPI int32_t ruListTryPopDataTo(ruList rl, msec_t timeoutMs, ptr* dest) {
    int32_t ret;
    List *list = ListGet(rl, &ret);
    if (!list) return ret;
    bool done = false;
    usec_t delay = 50;
    usec_t try = delay;
    msec_t end = ruTimeMs()+timeoutMs;
    do {
        if (list->doQuit) return RUE_USER_ABORT;
        if (!ruMutexTryLock(list->mux)) {
            ruSleepUs(1);
            try--;
            if (try) continue;
            try = delay;
        } else {
            if (list->doQuit) {
                ruMutexUnlock(list->mux);
                return RUE_USER_ABORT;
            }
            if (list->size) {
                ret = ListRemoveTo(list, list->head->next, dest);
                done = true;
            }
            ruMutexUnlock(list->mux);
            if (done) return ret;
            ruSleepUs(delay);
            try = delay;
        }
    } while (end > ruTimeMs());
    return RUE_FILE_NOT_FOUND;
}

RUAPI ptr ruListTryPop(ruList rl, msec_t timeoutMs, int32_t *code) {
    ptr dest = NULL;
    int32_t ret = ruListTryPopDataTo(rl, timeoutMs, &dest);
    ruRetWithCode(code, ret, dest);
}

RUAPI alloc_chars ruListJoin(ruList rl, trans_chars delim, int32_t* code) {
    List *list = ListGet(rl, code);
    if (!list) return NULL;

    if (!delim) delim = "";
    ruString buf = ruStringNew("");
    ruIterator li = ruListIter(list);
    bool started = false;
    for(perm_chars item = ruIterNext(li, perm_chars);
        li; item = ruIterNext(li, perm_chars)) {
        if (started) {
            ruStringAppend(buf, delim);
        } else {
            started = true;
        }
        ruStringAppend(buf, item);
    }

    alloc_chars out = ruStringGetCString(buf);
    buf = ruStringFree(buf, true);
    ruRetWithCode(code, RUE_OK, out);
}

RU_THREAD_LOCAL ruCompFunc listSort_;
#define SORT_TYPE ListElmt*
#define SORT_NAME ruList
#define SORT_CMP(x, y) listSort_((x), (y))
#ifdef RUMS
#pragma warning( disable : 4018 4242 4244 4388 )
#endif
#include "sort/sort.h"
RUAPI int32_t ruListSort(ruList rl) {
    int32_t ret;
    List *list = ListGet(rl, &ret);
    if (!list) return ret;
    if (!list->valSort) return RUE_PARAMETER_NOT_SET;

    if (list->doQuit) return RUE_USER_ABORT;
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        return RUE_USER_ABORT;
    }

    // setup
    ListElmt** arr = ruMalloc0(list->size, ListElmt*);
    listSort_ = list->valSort;
    // make the array
    int i = 0;
    for (ListElmt* e = list->head->next; e; e = e->next) {
        arr[i++] = e->data;
    }
    // sort it
    ruList_tim_sort(arr, list->size);
    // store the data in the new order
    i = 0;
    for (ListElmt* e = list->head->next; e; e = e->next) {
        e->data = arr[i++];
    }
    // clean up
    listSort_ = NULL;
    ruFree(arr);

    ruMutexUnlock(list->mux);
    return ret;
}
