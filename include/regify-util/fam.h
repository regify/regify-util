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
 * \defgroup fam File Access Monitoring
 * \brief This section contains file access monitoring routines.
 *
 * Example:
 * ~~~~~{.c}
    void myfamHandler(ruFamEvent* fe, perm_ptr ctx) {
        ruFamEventLog(RU_LOG_VERB, fe, "got");
        // important to free this
        ruFamEventFree(fe);
    }

    ruFamCtx famCtx = ruFamMonitorFilePath(
            "/some/dir","test",
            myfamHandler, NULL);
    // run your program...
    ruFamKillMonitor(famCtx);
 * ~~~~~
 *
 * @{
 */

#ifndef REGIFY_UTIL_FAM_H
#define REGIFY_UTIL_FAM_H
/** File was created */
#define RU_FAM_CREATED     0x001
/** File was deleted */
#define RU_FAM_DELETED     0x002
/** File was modified */
#define RU_FAM_MODIFIED    0x004
/** File was moved or renamed */
#define RU_FAM_MOVED       0x008
/** File meta data changed */
#define RU_FAM_ATTRIB      0x010

/** seconds to wait before hard kill */
#define RU_FAM_KILL_TIMEOUT    5
/** to wait when no events are pending */
#define RU_FAM_POLL_TIMEOUT    250
/** to wait when events (like moves) are pending */
#define RU_FAM_QUEUE_TIMEOUT   50

typedef struct ruFamEvent_ ruFamEvent;
struct ruFamEvent_ {
    int eventType;          // type of event #fam_*
    perm_chars srcPath;     // object file path
    perm_chars dstPath;     // destination path for #RU_FAM_MOVED
};

/**
 * Signature of call back event handler
 * @param fe \ref ruFamEvent to process. Caller must free with \ref ruFamEventFree
 * @param ctx User defined context passed in with \ref ruFamMonitorFilePath
 */
typedef void (*ruFamHandler) (ruFamEvent* fe, perm_ptr ctx);

/**
 * Opaque internal fam context pointer
 */
typedef void* ruFamCtx;


/**
 * This function creates a new thread that recursively watches filePath and
 * below and calls eventCallBack with events.
 *
 * @param filePath Filepath to monitor.
 * @param threadName The name that the new thread should go by in the logs
 * @param eventCallBack callback that will process given event
 * @param ctx user defined data that is then passed to the given eventCallBack
 *        along with the event.
 * @return
 */
RUAPI ruFamCtx ruFamMonitorFilePath(trans_chars filePath, trans_chars threadName,
                                    ruFamHandler eventCallBack, perm_ptr ctx);

/**
 * This function stop the thread started by \ref ruFamMonitorFilePath and closes
 * the associated monitor handles.
 * @param o famObj to kill
 * @return NULL
 */
RUAPI ruFamCtx ruFamKillMonitor(ruFamCtx o);

/**
 * Reports whether fam is in a running state or not
 * @param o famObj to check
 * @return false if fam is running
 */
RUAPI bool ruFamQuit(ruFamCtx o);

/**
 * Return string representation of the event type code
 * @param action eventType
 * @return string representating the given eventType. User must not free this.
 */
RUAPI const char* ruFamEventTypeString(int32_t action);

/**
 * Internal logging function used by \ref ruFamEventLog
 * @param loglevel Log level of this message.
 * @param file  __FILE__
 * @param func  __func__
 * @param line  __LINE__
 * @param fe famE vent tp log
 * @param prefix optional prefix message
 */
RUAPI void ruFamLogEventFunc(uint32_t loglevel, trans_chars file, trans_chars func,
                             int32_t line, ruFamEvent *fe, trans_chars prefix);

/**
 * Log string representation of famEvent
 * @param lvl Log level of this message.
 * @param fe famEvent tp log
 * @param prefix optional prefix message
 */
#define ruFamEventLog(lvl, fe, prefix) ruMacStart { \
    if(ruDoesLog(lvl)) { \
        ruFamLogEventFunc(lvl, __FILE__, __func__, __LINE__, fe, prefix); \
    } \
} ruMacEnd

/**
 * String representation of famEvent
 * @param fe event tp print out
 * @return string to be freed after use
 */
RUAPI char* ruFamEventString(ruFamEvent* fe);

/**
 * famEvent Constructor
 * @param eventType event type
 * @param filePath file path
 * @param destPath optional destination path
 * @return famEvent free with \ref ruFamEventFree after use
 */
RUAPI ruFamEvent* ruFamEventNew(int eventType, trans_chars filePath, trans_chars destPath);

/**
 * famEvent destructor
 * @param fe famEvent to free
 * @return NULL
 */
RUAPI ptr ruFamEventFree(ptr o);

/**
 * Generic famEvent destructor for collections
 *
 * @param fe famEvent to free
 */
RUAPI void ruFamEventFreeV(void* fe);

#endif //REGIFY_UTIL_FAM_H

/**
 * @}
 */
