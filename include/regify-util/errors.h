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
 * \file
 * The regify-utils/errors.h header file contains general regify-util error codes.
 *
 * \defgroup ruerror regify-util Error Codes
 * \brief This section specifies regify-util error codes and their meanings.
 *
 * @{
 */

#ifndef REGIFY_UTIL_ERRORS_H
#define REGIFY_UTIL_ERRORS_H
/* Only need to export C interface if used by C++ source code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \name regify Errors
 * @{
 */
/**
 * Success
 */
#define RUE_OK	0

// file i/o
/**
 * File not found.
 */
#define RUE_FILE_NOT_FOUND	60
/**
 * The given file already exists cannot not be overwritten.
 */
#define RUE_FILE_EXISTS 	324
/**
 * The given file can not be opened. Please check your path and permissions.
 */
#define RUE_CANT_OPEN_FILE	69
/**
 * The given file cannot be written. Please check your permissions.
 */
#define RUE_CANT_WRITE	    70

// parameters
/**
 * A required parameter is missing.
 */
#define RUE_PARAMETER_NOT_SET	64
/**
 * The given parameter is invalid.
 */
#define RUE_INVALID_PARAMETER	77

// miscellaneous
/**
 * We are out of memory.
 */
#define RUE_OUT_OF_MEMORY	68
/**
 * Overflow
 */
#define RUE_OVERFLOW	96
/**
 * User aborted
 */
#define RUE_USER_ABORT	98
/**
 * There has been a general error. Please consult your logs.
 */
#define RUE_GENERAL	        99
/**
 * The request call is invalid at this stage.
 */
#define RUE_INVALID_STATE	325

/** @} */
/** @} */



#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //REGIFY_UTIL_ERRORS_H
