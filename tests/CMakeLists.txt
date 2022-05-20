# Copyright regify
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

set(CHECK_LIBRARY_DIRS ${ICU_LIBRARY_DIRS} CACHE STRING
        "Where the check libraries are located")
set(CHECK_INCLUDE_DIR ${ICU_INCLUDE_DIRS} CACHE STRING
        "Where the check libraries are located")

if(MSVC)
    set(CHECK_LIBRARIES check.lib)
elseif(MAC OR MINGW)
    set(CHECK_LIBRARIES check)
else()
    set(CHECK_LIBRARIES -lcheck -lm -lrt -lsubunit)
endif()
dump("CHECK_LIBRARIES")

if(IOS)
    set(PBX_ARCH ${BUILD_ARCH})
    set(PBX_INCLUDES ${CHECK_INCLUDE_DIR})
    set(PBX_LDFLAGS "${LDIR}/libcheck.a ${PROJECT_BINARY_DIR}/libregify-util.a
        ${LDIR}/libicui18n.a ${LDIR}/libicuuc.a ${LDIR}/libicudata.a -lm")
    configure_file (
            "${PROJECT_SOURCE_DIR}/utilRunner/utilRunner.xcodeproj/project.pbxproj.in"
            "${PROJECT_SOURCE_DIR}/utilRunner/utilRunner.xcodeproj/project.pbxproj"
            @ONLY)
else()
    add_executable(runTests EXCLUDE_FROM_ALL
            runTests.cpp testCleaner.c testIo.c testList.c
            testMap.c testMisc.c testRegex.c testStore.c testString.c
            )
endif()

add_definitions(-DTEST_BASE=\"${CMAKE_CURRENT_SOURCE_DIR}/tree\")

target_include_directories(runTests
        PUBLIC ${PROJECT_SOURCE_DIR}/include/ ${CHECK_INCLUDE_DIR}
        )

target_link_directories(runTests PUBLIC ${CHECK_LIBRARY_DIRS})
target_link_libraries(runTests PUBLIC ${staticlib} ${CHECK_LIBRARIES})

add_test(runTests ${CMAKE_CURRENT_BINARY_DIR}/runTests)

add_custom_target(grind make runTests
        COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/grind.log
        COMMAND TEMP=/tmp CK_FORK=no valgrind --num-callers=24
        --read-var-info=yes --show-reachable=yes
        --leak-check=full --log-file=grind.log
        ${CMAKE_CURRENT_BINARY_DIR}/runTests
        COMMAND ! grep -A5 'are definitely lost'
        ${CMAKE_CURRENT_BINARY_DIR}/grind.log
        COMMAND echo "* * *   Success   * * *")
