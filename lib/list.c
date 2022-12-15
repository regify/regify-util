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
    return list;
}

static void ListClear(List *list) {
    int32_t ret;
    // Remove each element.
    while (list->size > 0) {
        void *data = ListRemoveAfter(list, NULL, &ret);
        if (ret == RUE_OK && list->destroy != NULL) {
            list->destroy(data);
        }
    }
}

void ListFree(List *list) {
    ListClear(list);
    // No operations are allowed now, but clear the structure as a precaution.
    list->mux = ruMutexFree(list->mux);
    memset(list, 0, sizeof(List));
    ruFree(list);
}

int32_t ListInsertAfter(List *list, ruListElmt rle, ptr data) {
    int32_t code;
    ListElmt *element = ListElmtGet(rle, &code);
    if (code == RUE_INVALID_PARAMETER) return code;
    ListElmt* new_element;
    ListElmt* next_element;

    new_element = ruMalloc0(1, ListElmt);
    new_element->type = MagicListElmt;
    new_element->data = data;
    if (element == NULL) {
        /* Handle insertion at the head of the list. */
        if (!ruListSize(list, NULL)) {
            list->tail = new_element;
        }
        new_element->next = list->head;
        if (list->head) list->head->prev = new_element;
        list->head = new_element;
    } else {
        /* Handle insertion somewhere other than at the head. */
        next_element = element->next;
        if (next_element == NULL) {
            list->tail = new_element;
        } else {
            next_element->prev = new_element;
        }
        new_element->next = next_element;
        new_element->prev = element;
        element->next = new_element;
    }
    /* Adjust the size of the list to account for the inserted element. */
    list->size++;
    return RUE_OK;
}

ptr ListRemove(List* list, ListElmt* old_element, int32_t* code) {
    if (!old_element) {
        ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL);
    }
    /* Do not allow removal from an empty list. */
    if (!list->size) {
        ruRetWithCode(code, RUE_INVALID_STATE, NULL);
    }

    ptr data = old_element->data;

    /* Remove the element from the list. */
    ListElmt* prev = old_element->prev;
    ListElmt* next = old_element->next;
    if (prev) {
        prev->next = next;
    } else  {
        list->head = next;
    }
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
    // TODO: tie in ListRemove
    int32_t ret;
    ListElmt *element = ListElmtGet(rle, &ret);
    if (ret == RUE_INVALID_PARAMETER) ruRetWithCode(code, ret, NULL);
    ListElmt *old_element;
    ptr data = NULL;

    /* Do not allow removal from an empty list. */
    if (ruListSize(list, code) == 0) {
        ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL);
    }

    /* Remove the element from the list. */
    if (element == NULL) {
        /* Handle removal from the head of the list. */
        data = list->head->data;
        old_element = list->head;
        list->head = list->head->next;
        if (ruListSize(list, code) == 1) list->tail = NULL;
    } else {
        /* Handle removal from somewhere other than the head. */
        if (element->next == NULL) {
            ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL);
        }
        data = element->next->data;
        old_element = element->next;
        element->next = element->next->next;
        if (element->next == NULL) list->tail = element;
    }
    /* Free the storage allocated by the abstract data type. */
    memset(old_element, 0, sizeof(ListElmt));
    free(old_element);
    /* Adjust the size of the list to account for the removed element. */
    list->size--;
    ruRetWithCode(code, RUE_OK, data);
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
    int32_t code;
    List *list = ListGet(rl, &code);
    if (code != RUE_OK) return code;
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
    int32_t code;
    List *list = ListGet(rl, &code);
    if (!list) return code;
    if (list->doQuit) return RUE_USER_ABORT;
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        return RUE_USER_ABORT;
    }
    ruListElmt rle = list->tail;
    code = ListInsertAfter(list, rle, (ptr)data);
    ruMutexUnlock(list->mux);
    return code;
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
        code = ListInsertAfter(list, NULL, (ptr)data);
    } else {
        ListElmt* rle = list->head;
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

RUAPI ptr ruListRemoveAfter(ruList rl, ruListElmt rle, int32_t *code) {
    List *list = ListGet(rl, code);
    if (!list) return NULL;
    if (list->doQuit) ruRetWithCode(code, RUE_USER_ABORT, NULL);
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        ruRetWithCode(code, RUE_USER_ABORT, NULL);
    }
    void* ret = ListRemoveAfter(list, rle, code);
    ruMutexUnlock(list->mux);
    return ret;
}

RUAPI int32_t ruListSize(ruList rl, int32_t *code) {
    ruClearError();
    List *list = ListGet(rl, code);
    if (!list) return -1;
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
    if (!list->head) return false;
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
    if (index >= list->size) ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL);
    if (list->doQuit) ruRetWithCode(code, RUE_USER_ABORT, NULL);
    ruMutexLock(list->mux);
    if (list->doQuit) {
        ruMutexUnlock(list->mux);
        ruRetWithCode(code, RUE_USER_ABORT, NULL);
    }
    ListElmt *rle = list->head;
    while(--index >= 0 && rle) rle = rle->next;
    ruMutexUnlock(list->mux);
    if (!rle) ruRetWithCode(code, RUE_OK, NULL);
    ruRetWithCode(code, RUE_OK, rle->data);
}

RUAPI ptr ruListTryPop(ruList rl, uint32_t timeoutMs, int32_t *code) {
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
            ruUsleep(1);
            try--;
            if (try) continue;
            try = delay;
        } else {
            if (list->doQuit) {
                ruMutexUnlock(list->mux);
                ruRetWithCode(code, RUE_USER_ABORT, NULL);
            }
            if (list->size) {
                res = ListRemoveAfter(list, NULL, code);
                done = true;
            }
            ruMutexUnlock(list->mux);
            if (done) return res;
            ruUsleep(delay);
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

    for(char* item = ruIterCurrent(li, char*);
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

