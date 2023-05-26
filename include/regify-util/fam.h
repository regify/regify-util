#ifndef REGIFYTK_LIBFAM_H
#define REGIFYTK_LIBFAM_H

/** File was created */
#define fam_created     0x001
/** File was deleted */
#define fam_deleted     0x002
/** File was modified */
#define fam_modified    0x004
/** File was moved or renamed */
#define fam_moved       0x008
/** File meta data changed */
#define fam_attrib      0x010

/** seconds to wait before hard kill */
#define FAM_KILL_TIMEOUT    5
/** to wait when no events are pending */
#define FAM_POLL_TIMEOUT    250
/** to wait when events (like moves) are pending */
#define FAM_QUEUE_TIMEOUT   50

typedef struct famEvent_ famEvent;
struct famEvent_ {
    int eventType;          // type of event #fam_*
    perm_chars srcPath;     // object file path
    perm_chars dstPath;     // destination path for #fam_moved
};

/**
 * Signature of call back event handler
 * @param fe \ref famEvent to process must free with \ref famEventFree
 * @param ctx User defined context passed in with \ref fam_monitorFilePath
 */
typedef void (*famHandler) (famEvent* fe, perm_ptr ctx);

/**
 * Opaque internal fam context pointer
 */
typedef void* famObj;


/**
 * This function creates a new thread that recursively watches filePath and
 * below and calls eventCallBack with events.
 *
 * @param filePath Filepath to monitor.
 * @param threadName The name that the new thraed should go by in the logs
 * @param eventCallBack callback that will process given event
 * @param ctx user defined data that is then passed to the given eventCallBack
 *        along with the event.
 * @return
 */
famObj fam_monitorFilePath(trans_chars filePath, trans_chars threadName,
                             famHandler eventCallBack, perm_ptr ctx);

/**
 * This function stop the thread started by \ref fam_monitorFilePath and closes
 * the associated monitor handles.
 * @param o famObj to kill
 * @return NULL
 */
famObj fam_killMonitor(famObj o);

/**
 * Reports whether fam is in a running state or not
 * @param o famObj to check
 * @return false if fam is running
 */
bool fam_quit(famObj o);

/**
 * Return string representation of the event type code
 * @param action eventType
 * @return string representating the given eventType. User must not free this.
 */
const char* famEventTypeString(int32_t action);

/**
 * Log string representation of famEvent
 * @param loglevel Log level of this message.
 * @param fe famEvent tp log
 * @param prefix optional prefix message
 */
#define famEventLog(lvl, fe, prefix) ruMacStart { \
    if(ruDoesLog(lvl)) { \
        famLogEventFunc(lvl, __FILE__, __func__, __LINE__, fe, prefix); \
    } \
} ruMacEnd

/**
 * Internal logging function used by \ref famLogEvent
 * @param loglevel Log level of this message.
 * @param file  __FILE__
 * @param func  __func__
 * @param line  __LINE__
 * @param fe famE vent tp log
 * @param prefix optional prefix message
 */
void famLogEventFunc(uint32_t loglevel, const char *file, const char *func,
                     int32_t line, famEvent *fe, const char* prefix);

/**
 * String representation of famEvent
 * @param fe event tp print out
 * @return string to be freed after use
 */
char* famEventString(famEvent* fe);

/**
 * famEvent Constructor
 * @param eventType event type
 * @param filePath file path
 * @param destPath optional destination path
 * @return famEvent free with \ref famEventFree after use
 */
famEvent* famEventNew(int eventType, trans_chars filePath, trans_chars destPath);

/**
 * famEvent destructor
 * @param fe famEvent to free
 * @return NULL
 */
famEvent* famEventFree(famEvent* fe);

/**
 * Generic famEvent destructor for collections
 *
 * @param fe famEvent to free
 */
void famEventFreeV(void* fe);

#endif //REGIFYTK_LIBFAM_H
