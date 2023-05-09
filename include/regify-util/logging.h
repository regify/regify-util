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
 */
typedef void (*ruLogFunc) (perm_ptr user_data, trans_chars message);


/**
 * \brief A convenience logging implementation that logs to stderr
 * @param udata User data. This is ignored in this function.
 * @param msg The message to log.
 */
RUAPI void ruStdErrorLogger(perm_ptr udata, trans_chars msg);

/**
 * \brief Sets the global logging function for this process.
 * @param logger Logging function that will be called with messages.
 * @param logLevel Loglevel to determine what gets logged.
 * @param userData Opaque custum user data that will be passed to the
 *                 \ref ruLogFunc implementation.
 */
RUAPI void ruSetLogger(ruLogFunc logger, uint32_t logLevel, perm_ptr userData);

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
 * \cond noworry Internal
 * \brief Logs the given parameters if logging is set.
 *
 * This function is really interal and used by the log macros.
 * @param log_level Log level of this message.
 * @param file The source file where this message originates from.
 * @param func The function that created this log entry.
 * @param line The line where the log was created at.
 * @param format The format specifier for the remaining arguments.
 * @param ... The remaining arguments that make up the log message.
 */
RUAPI void ruDoLog(uint32_t log_level, trans_chars filePath, trans_chars func,
                   int32_t line, trans_chars format, ...);

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
