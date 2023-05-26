// file access monitoring functions
#include "../lib.h"

perm_chars created = "created";
perm_chars deleted = "deleted";
perm_chars modified = "modified";
perm_chars moved = "moved";
perm_chars attrib = "attrib";
perm_chars unknown = "unknown";

perm_chars famEventTypeString(int32_t action) {
    switch (action) {
        case fam_created:
            return created;
        case fam_deleted:
            return deleted;
        case fam_modified:
            return modified;
        case fam_moved:
            return moved;
        case fam_attrib:
            return attrib;
        default:
            return unknown;
    }
}

void famLogEventFunc(uint32_t loglevel, trans_chars file, trans_chars func,
                     int32_t line, famEvent *fe, trans_chars prefix) {
    if (!fe) {
        ruDoLog(loglevel, file, func, line,
                "%s famEvent (null)", prefix);
        return;
    }
    if (fe->dstPath) {
        ruDoLog(loglevel, file, func, line,
                "%s famEvent type: %s file: [%s] dest: [%s]",
                prefix, famEventTypeString(fe->eventType),
                fe->srcPath, fe->dstPath);
    } else {
        ruDoLog(loglevel, file, func, line,
                "%s famEvent type: %s file: [%s]",
                prefix, famEventTypeString(fe->eventType),
                fe->srcPath);
    }
}

alloc_chars famEventString(famEvent *fe) {
    if (fe->dstPath) {
        return ruDupPrintf("famEvent type: %s file: [%s] dest: [%s]",
                           famEventTypeString(fe->eventType),
                           fe->srcPath, fe->dstPath);
    } else {
        return ruDupPrintf("famEvent type: %s file: [%s]",
                           famEventTypeString(fe->eventType),
                           fe->srcPath);
    }
}

famEvent* famEventNew(int eventType, trans_chars filePath, trans_chars destPath) {
    famEvent* fe = ruMalloc0(1, famEvent);
    fe->eventType = eventType;
    fe->srcPath = ruStrDup(filePath);
    fe->dstPath = ruStrDup(destPath);
//    famEventLog(RU_LOG_VERB, fe, "Created");
    return fe;
}

famEvent* famEventFree(famEvent* fe) {
    if (!fe) return NULL;
    ruFree(fe->srcPath);
    ruFree(fe->dstPath);
    ruFree(fe);
    return NULL;
}

void famEventFreeV(void* fe) {
    famEventFree((famEvent*)fe);
}

