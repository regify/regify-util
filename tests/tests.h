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
#ifndef _TESTS_H
# define _TESTS_H 1

// optional logging sink
#ifndef logfile
#define logfile NULL
#endif

#include <stdio.h>
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif
#include <check.h>
#ifdef __clang__
#pragma clang diagnostic pop
#endif
#include <regify-util.h>
#include "../lib/lib.h"

/* Only need to export C interface if used by C++ source code */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifdef DO_IOS
int32_t mainTest (const char *tmpDir, const char *treepath);
#endif

extern const char* testBase;
extern char pathBuffer[];
perm_chars makePath(const char *filepath);
char* insureTestFolder(const char* folderName);

TCase* miscTests(void);
TCase* listTests(void);
TCase* regexTests(void);
TCase* stringTests(void);
TCase* mapTests(void);
TCase* setTests(void);
TCase* ioTests(void);
TCase* iniTests(void);
TCase* jsonTests(void);
TCase* storeTests(void);
TCase* cleanerTests(void);
TCase* threadTests(void);
#if defined(__linux__) || defined(ITS_OSX) || defined(_WIN32)
TCase* famTests(void);
#endif

#ifdef __cplusplus
}   /* extern "C" */
#endif /* __cplusplus */

#endif /* _TESTS_H */
