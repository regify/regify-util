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

//ruMakeTypeGetter(ListElmt, ListElmtMagic)
ruMakeTypeGetter(List, ListMagic)

ListElmt* ListElmtGet(void* ptr, int32_t* code) {
    ListElmt* ret = (ListElmt*) ptr;
    if (!ptr) {
        do {
            if (code) { *code = 64; }
            return ((void*) 0);
        }
        while (0);
    }
    if (ptr < (void*) 0xffff || 2304 != ret->type) {
        do {
            if (code) { *code = 77; }
            return ((void*) 0);
        }
        while (0);
    }
    do {
        if (code) { *code = 0; }
        return ret;
    }
    while (0);
}

List* ListNew(ruFreeFunc freefn) {
    List *list = ruMalloc0(1, List);
    list->destroy = freefn;
    list->type = ListMagic;
    return list;
}

RUAPI ruList ruListNew(ruFreeFunc freefn) {
    ruClearError();
    return (ruList)ListNew(freefn);
}

void ListClear(List *list) {
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
    memset(list, 0, sizeof(ruList));
    ruFree(list);
}

RUAPI ruList ruListFree(ruList rl) {
    ruClearError();
    List *list = ListGet(rl, NULL);
    if (!list) return NULL;
    ListFree(list);
    return NULL;
}

RUAPI int32_t ruListClear(ruList rl) {
    ruClearError();
    int32_t code;
    List *list = ListGet(rl, &code);
    if (code != RUE_OK) return code;
    ListClear(list);
    return RUE_OK;
}

RUAPI int32_t ruListAppendPtr(ruList rl, const void *data) {
    ruClearError();
    int32_t code;
    ruListElmt rle = ruListTail(rl, &code);
    if (code != RUE_OK) return code;
    return ruListInsertAfter(rl, rle, data);
}

int32_t ListInsertAfter(List *list, ruListElmt rle, const void *data) {
    int32_t code;
    ListElmt *element = ListElmtGet(rle, &code);
    if (code == RUE_INVALID_PARAMETER) return code;
    ListElmt *new_element;

    new_element = ruMalloc0(1, ListElmt);
    new_element->type = ListElmtMagic;
    new_element->data = (void *)data;
    if (element == NULL) {
        /* Handle insertion at the head of the list. */
        if (ruListSize(list, &code) == 0) list->tail = new_element;
        new_element->next = list->head;
        list->head = new_element;
    } else {
        /* Handle insertion somewhere other than at the head. */
        if (element->next == NULL) list->tail = new_element;
        new_element->next = element->next;
        element->next = new_element;
    }
    /* Adjust the size of the list to account for the inserted element. */
    list->size++;
    return RUE_OK;
}

RUAPI int32_t ruListInsertAfter(ruList rl, ruListElmt rle, const void *data) {
    ruClearError();
    int32_t code;
    List *list = ListGet(rl, &code);
    if (!list) return code;
    return ListInsertAfter(list, rle, data);
}

void* ListRemoveAfter(List *list, ruListElmt rle, int32_t *code) {
    int32_t ret;
    ListElmt *element = ListElmtGet(rle, &ret);
    if (ret == RUE_INVALID_PARAMETER) ruRetWithCode(code, ret, NULL);
    ListElmt *old_element;
    void *data = NULL;

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
    free(old_element);
    /* Adjust the size of the list to account for the removed element. */
    list->size--;
    ruRetWithCode(code, RUE_OK, data);
}

RUAPI void* ruListRemoveAfter(ruList rl, ruListElmt rle, int32_t *code) {
    List *list = ListGet(rl, code);
    if (!list) return NULL;
    return ListRemoveAfter(list, rle, code);
}

RUAPI int32_t ruListSize(ruList rl, int32_t *code) {
    ruClearError();
    List *list = ListGet(rl, code);
    if (!list) return -1;
    return list->size;
}

ListElmt* ListHead(List *list) {
    if (!list->head) return NULL;
    return list->head;
}

RUAPI ruListElmt ruListHead(ruList rl, int32_t* code) {
    ruClearError();
    List *list = ListGet(rl, code);
    if (!list) return NULL;
    return ListHead(list);
}

RUAPI ruListElmt ruListTail(ruList rl, int32_t* code) {
    ruClearError();
    List *list = ListGet(rl, code);
    if (!list) return NULL;
    if (!list->tail) return NULL;
    return list->tail;
}

RUAPI bool ruListElmtIsHead(ruList rl, ruListElmt re, int32_t* code) {
    ruClearError();
    List *list = ListGet(rl, code);
    if (!list) return false;
    if (!list->head) return false;
    ListElmt *el = ListElmtGet(re, code);
    if (!el) return false;
    if (el == list->head) return true;
    return false;
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

RUAPI void* ruListNextData(ruListElmt *re, int32_t* code) {
    ruClearError();
    if (!re) ruRetWithCode(code, RUE_PARAMETER_NOT_SET, NULL);
    ListElmt *el = ListElmtGet(*re, code);
    if (el) {
        (*re) = (ruListElmt)el->next;
        if (el->next) return el->next->data;
    }
    return NULL;
}

RUAPI void* ruListElmtData(ruListElmt re, int32_t* code) {
    ruClearError();
    ListElmt *el = ListElmtGet(re, code);
    if (!el) return NULL;
    return el->data;
}

RUAPI void* ruListIdxElmtData(ruList rl, int32_t index, int32_t* code) {
    ruClearError();
    List *list = ListGet(rl, code);
    if (!list) return NULL;
    if (index >= list->size) ruRetWithCode(code, RUE_INVALID_PARAMETER, NULL);
    ListElmt *rle = list->head;
    while(--index >= 0 && rle) rle = rle->next;
    if (!rle) ruRetWithCode(code, RUE_OK, NULL);
    ruRetWithCode(code, RUE_OK, rle->data);
}
