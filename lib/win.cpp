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
#include "lib.h"
#include <ctime>
#include <iomanip>
#include <sstream>

sec_t timeParse(trans_chars dateformat, trans_chars datestr, bool utc) {
    ruZeroedStruct(struct tm, t);
    if (!dateformat || !datestr) return -1;
    std::istringstream input(datestr);
    input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
    input >> std::get_time(&t, dateformat);
    if (input.fail()) {
        return -1;
    }
    if (utc) {
        return (sec_t)_mkgmtime(&t);
    } else {
        return (sec_t)mktime(&t);
    }
}
