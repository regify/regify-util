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
 * \defgroup logging Logging Facility
 * \brief This section contains logging specific APIs.
 *
 * Example:
 * ~~~~~{.c}
 * #include <regify-util.h>
 *
 * void testlog(trans_chars logPath, bool cleaning, bool threaded) {
 *     ruCleaner rc = NULL;
 *     ruSinkCtx sc = NULL;
 *     // Use the pre logger until we determine where to log to...
 *     ruPreCtx pc = ruPreCtxNew();
 *     ruSetLogger(ruPreLogSink, RU_LOG_DBUG, pc, NULL, threaded);
 *
 *     if (cleaning) {
 *         ruInfoLog("Using log cleaner");
 *         rc = ruCleanNew(0);
 *         ruCleanAdd(rc, "testsecret", "^^^TEST_SECRET^^^");
 *         ruVerbLog("keeping testsecret hidden");
 *     }
 *     if (ruStrEmpty(logfile)) {
 *         ruInfoLog("Using std error logger");
 *         ruSetLogger(ruStdErrLogSink, RU_LOG_DBUG, NULL, rc, threaded);
 *     } else {
 *         ruInfoLogf("Logging to '%s'", logfile);
 *         sc = ruSinkCtxNew(logfile, NULL, NULL);
 *         ruSetLogger(ruFileLogSink, RU_LOG_DBUG, sc, rc, threaded);
 *     }
 *     // flush and free the pre logger
 *     pc = ruPreCtxFree(pc, true);
 *
 *     // program logic here...
 *     ruInfoLog("starting with testsecret and cleaner");
 *
 *     // clean up when finished
 *     ruInfoLog("stopping logger");
 *     ruStopLogger();
 *     ruSinkCtxFree(sc);
 *     ruCleanFree(rc);
 * }
 * ~~~~~
 * @{
 */

#ifndef REGIFY_UTIL_LOGGING_H
#define REGIFY_UTIL_LOGGING_H
/* Only need to export C interface if used by C++ source code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief This level disables logging altogether.
 */
#define RU_LOG_NONE 0

/**
 * \brief This level reduces logging to fatal errors and generally unexpected
 * serious stuff.
 */
#define RU_LOG_CRIT 1

/**
 * \brief This level includes \ref RU_LOG_CRIT and general warning messages.
 */
#define RU_LOG_WARN 2

/**
 * \brief This level includes \ref RU_LOG_WARN and general information.
 * Usual low quantity.
 */
#define RU_LOG_INFO 3

/**
 * \brief This is the verbose logging level used for debugging and usually
 * high quantity.
 */
#define RU_LOG_VERB 4

/**
 * \brief This is a debugging logging level used for debugging things that are
 * normally assumed to be working. It is usually off because it litters the logs
 * with often redundant data.
 */
#define RU_LOG_DBUG 5

/**
 * \brief The type of function to pass to \ref ruSetLogger in order to hook up
 * logging.
 *
 * When this function receives NULL for msg, it means that this may be the
 * last log call this function will receive in the current context. This is
 * intended to finalize whatever resources it may have, or simply to flush data.
 *
 * @param userData The userData reference that was passed to \ref ruSetLogger.
 * @param logLevel Log level pertaining to this message. Log filtering has been
 *                 done at this point, and this is to facilitate further log
 *                 level based processing decisions.
 * @param msg The message to log, or NULL for flushing or closing.
 */
typedef void (*ruLogFunc) (perm_ptr userData, uint32_t logLevel, trans_chars msg);

/**
 * \brief The type of function to pass to \ref ruSinkCtxNew which will be called
 * when \ref ruFileLogSink has closed its logfile.
 * @param userData The closeCtx reference that was passed to \ref ruSinkCtxNew.
 */
typedef void (*ruCloseFunc) (perm_ptr userData);

/**
 * \brief Opaque pointer to \ref ruPreLogSink object.
 */
typedef ptr ruPreCtx;

/**
 * \brief Creates a new pre logging context for use with \ref ruPreLogSink
 *
 * This context is useful for temporarily storing log entries until the final
 * log sink has been established. At that point this context may be freed
 * optionally logging its content into the current logsink.
 * @return \ref ruPreCtx object
 */
RUAPI ruPreCtx ruPreCtxNew(void);

/**
 * \brief Frees the given \ref ruPreCtx
 * @param rpc context to free
 * @param flush log the accumulated entries before freeing them.
 * @return NULL
 */
RUAPI ruPreCtx ruPreCtxFree(ruPreCtx rpc, bool flush);

/**
 * \brief Pre logging sink to store log entries until a final log sink has been
 * established.
 *
 * When calling \ref ruSetLogger with this sink, the log level should be set to
 * \ref RU_LOG_DBUG so that all potential entries will be accumulated. The actual
 * filtering is done when this context is flushed. At that point entries will be
 * filtered by the then current log level.
 *
 * @param rpc User data. Must be a \ref ruPreCtx.
 * @param logLevel Log level pertaining to this message. Will be stored for
 *                 flush time.
 * @param msg The message to log. NULL will be ignored.
 */
RUAPI void ruPreLogSink(perm_ptr rpc, uint32_t logLevel, trans_chars msg);

/**
 * \brief Opaque pointer to \ref ruFileLogSink object.
 */
typedef ptr ruSinkCtx;

/**
 * \brief Creates a new context to use with \ref ruFileLogSink
 * @param filePath Path to where log file will be written. Directory must be
 *                 writable.
 * @param closeCb Optional callback to run when the old file has been closed
 *                after \ref ruSinkCtxPath was called.
 * @param closeCtx Optional context which will be passed to closeCb
 * @return New \ref ruSinkCtx to free with \ref ruSinkCtxFree.
 */
RUAPI ruSinkCtx ruSinkCtxNew(trans_chars filePath, ruCloseFunc closeCb,
                             perm_ptr closeCtx);

/**
 * \brief Allows changing the file path of the given \ref ruSinkCtx.
 *
 * This will have immediate effect on logs sent to \ref ruFileLogSink. When the
 * old log has been closed the closeCb function given to \ref ruSinkCtxNew will
 * be called exactly one time. This will not happen if the new filePath is equal
 * to the old filePath.
 * @param rsc \ref ruSinkCtx to update.
 * @param filePath Path to new log file.
 * @return Status of the operation.
 */
RUAPI int32_t ruSinkCtxPath(ruSinkCtx rsc, trans_chars filePath);

/**
 * \brief Frees the given \ref ruSinkCtx after \ref ruStopLogger has been called.
 * This must not be called when the context is still in use.
 * @param rsc \ref ruSinkCtx to free.
 * @return NULL
 */
RUAPI ruSinkCtx ruSinkCtxFree(ruSinkCtx rsc);

/**
 * \brief A log sink that logs into the filepath set in the given \ref ruSinkCtx
 *
 * This sink checks its underlying file every 3 seconds to see whether it has
 * moved or the \ref ruSinkCtx filePath has changed. When it does, it closes its
 * current file handle and reopens it to allow for logrotation to work.
 * When it receives a NULL message, like through \ref ruFlushLog, it closes its file.
 * Through the use of \ref ruSinkCtxPath, output files can be changed on the
 * fly. Every time it closes its log file, the \ref ruSinkCtx closeCb is called.
 *
 * @param rsc User data. Must be a \ref ruSinkCtx.
 * @param logLevel Ignored log level pertaining to this message.
 * @param msg The message to log. NULL will cause it to close the log file.
 */
RUAPI void ruFileLogSink(perm_ptr rsc, uint32_t logLevel, trans_chars msg);

/**
 * \brief A convenience logging implementation that logs to stderr
 * @param udata User data. This is ignored in this function.
 * @param logLevel Log level pertaining to this message.
 * @param msg The message to log.
 */
RUAPI void ruStdErrLogSink(perm_ptr udata, uint32_t logLevel, trans_chars msg);

/**
 * \brief Sets the global logging function for this process.
 *
 * When threaded is set, the log thread always follows up with one NULL message
 * when there are no more messages to log. This is intended to facilitate log
 * flushing. In the absense of the log thread this can also be accomplished by
 * calling \ref ruFlushLog.
 * At the end \ref ruStopLogger should be called to flush and close the log file.
 *
 * @param logger Logging function that will be called with messages.
 * @param logLevel Loglevel to determine what gets logged.
 * @param userData Opaque custom user data that will be passed to the
 *                 \ref ruLogFunc implementation.
 * @param cleaner Optional \ref ruCleaner instance for real time password cleaning.
 * @param threaded Whether to receive all logger calls from a dedicated thread.
 *                 This is useful when many threads do lots of logging.
 */
RUAPI void ruSetLogger(ruLogFunc logger, uint32_t logLevel, perm_ptr userData,
                       ruCleaner cleaner, bool threaded);

/**
 * \brief Stop the current logger and flush the queue before returning.
 */
RUAPI void ruStopLogger(void);

/**
 * \brief Adjusts the current log level.
 * @param logLevel New log level to log at.
 */
RUAPI void ruSetLogLevel(uint32_t logLevel);

/**
 * \brief Returns the currently set log level.
 * @return The currently set log level.
 */
RUAPI uint32_t ruGetLogLevel(void);

/**
 * \brief Returns whether the given log level should log.
 *
 * This function considers the set log level only if a logging function was set.
 * If no logging function was set it will return false.
 * @param log_level Level to be considered.
 * @return Whether to log.
 */
RUAPI bool ruDoesLog(uint32_t log_level);

/**
 * \brief Logs the given parameters if logging is set.
 *
 * This function is really internal and used by the log macros.
 * @param log_level Log level of this message.
 * @param filePath The source file where this message originates from.
 * @param func The function that created this log entry.
 * @param line The line where the log was created at.
 * @param format The format specifier for the remaining arguments.
 * @param ... The remaining arguments that make up the log message.
 */
RUAPI void ruDoLog(uint32_t log_level, trans_chars filePath, trans_chars func,
                   int32_t line, trans_chars format, ...);

/**
 * \brief Logs the given parameters if logging is set.
 *
 * This function is advanced and designed to be used by custom log macros.
 * @param log_level Log level of this message.
 * @param filePath The source file where this message originates from.
 * @param func The function that created this log entry.
 * @param line The line where the log was created at.
 * @param format The format specifier for the remaining arguments.
 * @param args variable argument list
 */
RUAPI void ruDoLogV(uint32_t log_level, trans_chars filePath, trans_chars func,
              int32_t line, trans_chars format, va_list args);

/**
 * Internal logging function used by the log macros.
 * @param log_level Log level of this message.
 * @param filePath The source file where this message originates from.
 * @param func The function that created this log entry.
 * @param line The line where the log was created at.
 * @param format The format specifier for the remaining arguments.
 * @param args variable argument list
 * @return The allocated log message to be freed by the caller after usage.
 */
RUAPI alloc_chars ruMakeLogMsgV(uint32_t log_level, trans_chars filePath,
                                trans_chars func, int32_t line,
                                trans_chars format, va_list args);

/**
 * \brief Sends the given message as is to the current log sink.
 * @param log_level Log level of this message used solely for filtering.
 * @param msg string to be logged without modification. The trailing line feed
 *            must be given.
 */
RUAPI void ruRawLog(uint32_t log_level, trans_chars msg);

/**
 * \brief Sends a NULL message with log level \ref RU_LOG_CRIT to the current
 * log sink, usually causing it to flush or close the log file.
 *
 * If the logger is threaded this will block until a NULL has been sent to the
 * given log sink.
 */
RUAPI void ruFlushLog(void);

/**
 * \brief Sends a NULL message with log level \ref RU_LOG_NONE to the current
 * log sink, usually causing it to close the log file and call a potential
 * close callback.
 *
 * If the logger is threaded this will block until a NULL has been sent to the
 * given log sink.
 */
RUAPI void ruLastLog(void);

/**
 * Internal logging function used by the log macros.
 * @param log_level Log level of this message.
 * @param file The source file where this message originates from.
 * @param func The function that created this log entry.
 * @param line The line where the log was created at.
 * @param format The format specifier for the remaining arguments.
 * @param ... The remaining arguments that make up the log message.
 * @return The allocated log message to be freed by the caller after usage.
 */
RUAPI char* ruMakeLogMsg(uint32_t log_level, trans_chars file, trans_chars func,
                   int32_t line, trans_chars format, ...);

/**
 * \cond noworry Internal
 * Internal logging macro used by the public log macros.
 * @param lvl Log level of this message.
 * @param format The format specifier for the remaining arguments.
 * @param ... The remaining arguments that make up the log message.
 */
#define ruLog_(lvl, format, ...) ruMacStart { \
    if(ruDoesLog(lvl)) { \
        ruDoLog(lvl, __FILE__, __func__, __LINE__, format, __VA_ARGS__); \
    } \
} ruMacEnd
/** \endcond */

/**
 * \brief Emits a CRIT level log message.
 * @param msg The message to be logged.
 */
#define ruCritLog(msg) ruLog_(RU_LOG_CRIT, "%s", msg)

/**
 * \brief Emits a CRIT level log message.
 * @param fmt The format specifier for the remaining arguments.
 * @param ... The remaining arguments that make up the log message.
 */
#define ruCritLogf(fmt, ...) ruLog_(RU_LOG_CRIT, fmt, __VA_ARGS__)

/**
 * \brief Emits a WARN level log message.
 * @param msg The message to be logged.
 */
#define ruWarnLog(msg) ruLog_(RU_LOG_WARN, "%s", msg)

/**
 * \brief Emits a WARN level log message.
 * @param fmt The format specifier for the remaining arguments.
 * @param ... The remaining arguments that make up the log message.
 */
#define ruWarnLogf(fmt, ...) ruLog_(RU_LOG_WARN, fmt, __VA_ARGS__)

/**
 * \brief Emits a INFO level log message.
 * @param msg The message to be logged.
 */
#define ruInfoLog(msg) ruLog_(RU_LOG_INFO, "%s", msg)

/**
 * \brief Emits a INFO level log message.
 * @param fmt The format specifier for the remaining arguments.
 * @param ... The remaining arguments that make up the log message.
 */
#define ruInfoLogf(fmt, ...) ruLog_(RU_LOG_INFO, fmt, __VA_ARGS__)

/**
 * \brief Emits a VERB level log message.
 * @param msg The message to be logged.
 */
#define ruVerbLog(msg) ruLog_(RU_LOG_VERB, "%s", msg)

/**
 * \brief Emits a VERB level log message.
 * @param fmt The format specifier for the remaining arguments.
 * @param ... The remaining arguments that make up the log message.
 */
#define ruVerbLogf(fmt, ...) ruLog_(RU_LOG_VERB, fmt, __VA_ARGS__)

/**
 * \brief Emits a DBUG level log message.
 * @param msg The message to be logged.
 */
#define ruDbgLog(msg) ruLog_(RU_LOG_DBUG, "%s", msg)

/**
 * \brief Emits a DBUG level log message.
 * @param fmt The format specifier for the remaining arguments.
 * @param ... The remaining arguments that make up the log message.
 */
#define ruDbgLogf(fmt, ...) ruLog_(RU_LOG_DBUG, fmt, __VA_ARGS__)

/**
 * \brief Debug logs the 8 bytes following start preceded by given message
 * @param msg prefix message
 * @param start address from where to start dumping bytes
 */
#define ruDump8(msg, start) \
    ruDbgLogf("%s @0x%x %02x %02x %02x %02x  %02x %02x %02x %02x", msg, (start), \
        (uint8_t)*(start), (uint8_t)*((start)+1), (uint8_t)*((start)+2), (uint8_t)*((start)+3), \
        (uint8_t)*((start)+4), (uint8_t)*((start)+5), (uint8_t)*((start)+6), (uint8_t)*((start)+7))

/**
 * \brief Debug logs the 16 bytes following start preceded by given message
 * @param msg prefix message
 * @param start address from where to start dumping bytes
 */
#define ruDump16(msg, start) \
    ruDbgLogf("%s @0x%x %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x  %02x %02x %02x %02x", msg, (start), \
        (uint8_t)*(start), (uint8_t)*((start)+1), (uint8_t)*((start)+2), (uint8_t)*((start)+3), \
        (uint8_t)*((start)+4), (uint8_t)*((start)+5), (uint8_t)*((start)+6), (uint8_t)*((start)+7), \
        (uint8_t)*((start)+8), (uint8_t)*((start)+9), (uint8_t)*((start)+10), (uint8_t)*((start)+11), \
        (uint8_t)*((start)+12), (uint8_t)*((start)+13), (uint8_t)*((start)+14), (uint8_t)*((start)+15))

/**
 * @}
 */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_LOGGING_H
