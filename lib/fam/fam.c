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
// file access monitoring functions
#include "../lib.h"

perm_chars created = "created";
perm_chars deleted = "deleted";
perm_chars modified = "modified";
perm_chars moved = "moved";
perm_chars attrib = "attrib";
perm_chars unknown = "unknown";

RUAPI perm_chars ruFamEventTypeString(int32_t action) {
    switch (action) {
        case RU_FAM_CREATED:
            return created;
        case RU_FAM_DELETED:
            return deleted;
        case RU_FAM_MODIFIED:
            return modified;
        case RU_FAM_MOVED:
            return moved;
        case RU_FAM_ATTRIB:
            return attrib;
        default:
            return unknown;
    }
}

RUAPI void ruFamLogEventFunc(uint32_t loglevel, trans_chars file, trans_chars func,
                       int32_t line, ruFamEvent *fe, trans_chars prefix) {
    if (!fe) {
        ruDoLog(loglevel, file, func, line,
                "%s famEvent (null)", prefix);
        return;
    }
    if (fe->dstPath) {
        ruDoLog(loglevel, file, func, line,
                "%s famEvent type: %s file: [%s] dest: [%s]",
                prefix, ruFamEventTypeString(fe->eventType),
                fe->srcPath, fe->dstPath);
    } else {
        ruDoLog(loglevel, file, func, line,
                "%s famEvent type: %s file: [%s]",
                prefix, ruFamEventTypeString(fe->eventType),
                fe->srcPath);
    }
}

RUAPI alloc_chars ruFamEventString(ruFamEvent *fe) {
    if (fe->dstPath) {
        return ruDupPrintf("famEvent type: %s file: [%s] dest: [%s]",
                           ruFamEventTypeString(fe->eventType),
                           fe->srcPath, fe->dstPath);
    } else {
        return ruDupPrintf("famEvent type: %s file: [%s]",
                           ruFamEventTypeString(fe->eventType),
                           fe->srcPath);
    }
}

RUAPI ruFamEvent* ruFamEventNew(int eventType, trans_chars filePath, trans_chars destPath) {
    ruFamEvent* fe = ruMalloc0(1, ruFamEvent);
    fe->eventType = eventType;
    fe->srcPath = ruStrDup(filePath);
    fe->dstPath = ruStrDup(destPath);
//    famEventLog(RU_LOG_VERB, fe, "Created");
    return fe;
}

RUAPI ruFamEvent* ruFamEventFree(ruFamEvent* fe) {
    if (!fe) return NULL;
    ruFree(fe->srcPath);
    ruFree(fe->dstPath);
    ruFree(fe);
    return NULL;
}

RUAPI void ruFamEventFreeV(void* fe) {
    ruFamEventFree((ruFamEvent*) fe);
}

