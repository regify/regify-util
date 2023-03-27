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

List* ListNew(ruFreeFunc freefn) {
    List *list = ruMalloc0(1, List);
    list->destroy = freefn;
    list->type = MagicList;
    list->mux = ruMutexInit();
    // create the root node
    list->head = ruMalloc0(1, ListElmt);
    list->head->type = MagicListElmt;
    list->head->prev = list->head;
    list->tail = list->head;
    return list;
}

static void ListClear(List *list) {
    int32_t ret;
    // Remove each element.
    while (list->size > 0) {
        void *data = ListRemove(list, list->head->next, &ret);
        if (ret == RUE_OK && list->destroy != NULL) {
            list->destroy(data);
        }
    }
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
    ListElmt* new_element;
    ListElmt* next_element;

    new_element = ruMalloc0(1, ListElmt);
    new_element->type = MagicListElmt;
    new_element->data = data;
    next_element = element->next;
    if (next_element == NULL) {
        list->tail = new_element;
    } else {
        next_element->prev = new_element;
    }
    new_element->next = next_element;
    new_element->prev = element;
    element->next = new_element;
    /* Adjust the size of the list to account for the inserted element. */
    list->size++;
    return RUE_OK;
}

ptr ListRemove(List* list, ListElmt* old_element, int32_t* code) {
    if (!old_element) {
        ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL);
    }
    if (!list->size) {
        // Do not allow removal from an empty list
        ruRetWithCode(code, RUE_INVALID_STATE, NULL);
    }
    if (old_element == old_element->prev) {
        // do not allow removing head
        ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL);
    }
    ptr data = old_element->data;

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
    ruRetWithCode(code, RUE_OK, data);
}

ptr ListRemoveAfter(List *list, ruListElmt rle, int32_t *code) {
    int32_t ret;
    ListElmt *element = NULL;
    if (!rle) {
        element = list->head;
    } else {
        element = ListElmtGet(rle, &ret);
        if (ret != RUE_OK) ruRetWithCode(code, ret, NULL);
    }
    if (element->next == NULL) {
        ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL);
    }
    return ListRemove(list, element->next, code);
}

RUAPI ruList ruListNew(ruFreeFunc freefn) {
    ruClearError();
    return (ruList)ListNew(freefn);
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

RUAPI int32_t ruListInsertAt(ruList rl, int32_t index, perm_ptr data) {
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
    if (!index) {
        code = ListInsertAfter(list, list->head, (ptr)data);
    } else {
        ListElmt* rle = list->head->next;
        while(--index > 0 && rle) rle = rle->next;
        code = ListInsertAfter(list, rle, (ptr)data);
    }
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

RUAPI ptr ruListRemove(ruList rl, ruListElmt* rle, int32_t* code) {
    List *list = ListGet(rl, code);
    if (!list) return NULL;
    if (list->doQuit) ruRetWithCode(code, RUE_USER_ABORT, NULL);
    ListElmt* elem = ListElmtGet(*rle, code);
    if (!elem) ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL);
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        ruRetWithCode(code, RUE_USER_ABORT, NULL);
    }
    (*rle) = (ruListElmt)elem->prev;
    ptr ret = ListRemove(list, elem, code);
    ruMutexUnlock(list->mux);
    return ret;
}

RUAPI ptr ruListPop(ruList rl, int32_t *code) {
    List *list = ListGet(rl, code);
    if (!list) return NULL;
    if (list->doQuit) ruRetWithCode(code, RUE_USER_ABORT, NULL);
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        ruRetWithCode(code, RUE_USER_ABORT, NULL);
    }
    ptr out = NULL;
    ListElmt *element = NULL;
    do {
        element = list->head;
        if (element->next == NULL) {
            if (code) *code = RUE_FILE_NOT_FOUND;
            break;
        }
        out = ListRemove(list, element->next, code);
    } while (0);
    ruMutexUnlock(list->mux);
    return out;
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

RUAPI bool ruListElmtIsHead(ruList rl, ruListElmt re, int32_t* code) {
    ruClearError();
    List *list = ListGet(rl, code);
    if (!list) return false;
    bool ret = false;
    if (list->doQuit) ruRetWithCode(code, RUE_USER_ABORT, ret);
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        ruRetWithCode(code, RUE_USER_ABORT, ret);
    }
    ListElmt *el = ListElmtGet(re, code);
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

RUAPI ptr ruListNextData(ruListElmt *re, int32_t* code) {
    ruClearError();
    if (!re) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL);
    ListElmt *el = ListElmtGet(*re, code);
    if (el) {
        (*re) = (ruListElmt)el->next;
        if (el->next) return el->next->data;
    }
    return NULL;
}

RUAPI ptr ruListElmtData(ruListElmt re, int32_t* code) {
    ruClearError();
    ListElmt *el = ListElmtGet(re, code);
    if (!el) return NULL;
    return el->data;
}

RUAPI ptr ruListIdxElmtData(ruList rl, int32_t index, int32_t* code) {
    ruClearError();
    List *list = ListGet(rl, code);
    if (!list) return NULL;
    uint32_t offset = abs(index);
    if (index < 0) {
        // we get one more backwards
        if (offset > list->size) ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL);
    } else {
        if (offset >= list->size) ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL);
    }
    if (list->doQuit) ruRetWithCode(code, RUE_USER_ABORT, NULL);
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        ruRetWithCode(code, RUE_USER_ABORT, NULL);
    }
    ListElmt *rle = NULL;
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
    ruMutexUnlock(list->mux);
    if (!rle) ruRetWithCode(code, RUE_OK, NULL);
    ruRetWithCode(code, RUE_OK, rle->data);
}

RUAPI ptr ruListTryPop(ruList rl, msec_t timeoutMs, int32_t *code) {
    List *list = ListGet(rl, code);
    if (!list) return NULL;
    void* res = NULL;
    bool done = false;
    usec_t delay = 50;
    usec_t try = delay;
    msec_t end = ruTimeMs()+timeoutMs;
    do {
        if (list->doQuit) ruRetWithCode(code, RUE_USER_ABORT, NULL);
        if (!ruMutexTryLock(list->mux)) {
            ruSleepUs(1);
            try--;
            if (try) continue;
            try = delay;
        } else {
            if (list->doQuit) {
                ruMutexUnlock(list->mux);
                ruRetWithCode(code, RUE_USER_ABORT, NULL);
            }
            if (list->size) {
                res = ListRemove(list, list->head->next, code);
                done = true;
            }
            ruMutexUnlock(list->mux);
            if (done) ruRetWithCode(code, RUE_OK, res);
            ruSleepUs(delay);
            try = delay;
        }
    } while (end > ruTimeMs());
    ruRetWithCode(code, RUE_FILE_NOT_FOUND, NULL);
}

RUAPI alloc_chars ruListJoin(ruList rl, trans_chars delim, int32_t* code) {
    List *list = ListGet(rl, code);
    if (!list) return NULL;

    if (!delim) delim = "";
    ruString buf = ruStringNew("");
    ruIterator li = ruListIter(list);
    bool started = false;

    for(char* item = ruIterNext(li, char*);
        li; item = ruIterNext(li, char*)) {
        if (started) {
            ruStringAppendf(buf, "%s%s", delim, item);
        } else {
            ruStringAppendf(buf, "%s", item);
            started = true;
        }
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
RUAPI int32_t ruListSort(ruList rl, ruCompFunc sort) {
    int32_t ret;
    List *list = ListGet(rl, &ret);
    if (!list) return ret;
    if (!sort) return RUE_PARAMETER_NOT_SET;

    if (list->doQuit) return RUE_USER_ABORT;
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        return RUE_USER_ABORT;
    }

    // setup
    ListElmt** arr = ruMalloc0(list->size, ListElmt*);
    listSort_ = sort;
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
