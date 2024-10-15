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
option(TEST_ON_SMB "Whether tests are running on a SMB share. Disables some tests" OFF)

if(MSVC)
    set(CHECK_LIBRARIES check.lib)
elseif(MAC OR MINGW)
    set(CHECK_LIBRARIES check)
else()
    set(CHECK_LIBRARIES -lcheck -lm -lrt -lsubunit)
endif()
dump("CHECK_LIBRARIES")

if(IOS)
    set(PBX_ARCH ${NAME_ARCH})
    set(PBX_INCLUDES ${CHECK_INCLUDE_DIR})
    set(PBX_LDFLAGS "${LDIR}/libcheck.a ${PROJECT_BINARY_DIR}/libregify-util.a
        ${LDIR}/libicui18n.a ${LDIR}/libicuuc.a ${LDIR}/libicudata.a -lm")
    configure_file (
            "${PROJECT_SOURCE_DIR}/utilRunner/utilRunner.xcodeproj/project.pbxproj.in"
            "${PROJECT_SOURCE_DIR}/utilRunner/utilRunner.xcodeproj/project.pbxproj"
            @ONLY)
else()
    if (LINUX OR WIN OR MAC)
        set(FAMSRC testFam.c)
    else()
        set(FAMSRC "")
    endif()
    add_executable(runTests EXCLUDE_FROM_ALL
            runTests.cpp testCleaner.c ${FAMSRC} testHtml.c testIni.c testIo.c
            testJson.c testList.c testLogging.c testMap.c testMisc.c testRegex.c
            testSet.c testStore.c testString.c testThread.c)
    target_include_directories(runTests
            PRIVATE ${PROJECT_SOURCE_DIR}/include/ ${CHECK_INCLUDE_DIR})
    target_compile_definitions(runTests PRIVATE
            SRC_BASE=\"${CMAKE_CURRENT_SOURCE_DIR}/tree\"
            OUT_BASE="${CMAKE_CURRENT_BINARY_DIR}"
            logfile=\"${CMAKE_BINARY_DIR}/test.log\")
    if(TEST_ON_SMB)
        target_compile_definitions(runTests PRIVATE ON_SMB_SHARE)
    endif()

    if(NOT WIN)
        add_custom_command(OUTPUT loeh
            COMMAND ln -s ${CMAKE_CURRENT_SOURCE_DIR}/tree/doeh loeh )
        add_custom_command(OUTPUT löss
            COMMAND ln -s ${CMAKE_CURRENT_SOURCE_DIR}/tree/föss löss )
        add_custom_target(symlinks DEPENDS löss loeh)
        add_dependencies(runTests symlinks)
    endif()

    if(MINGW)
        # we want this to remove dependencies to:
        #  /msys64/mingw64/bin/libgcc_s_seh-1.dll
        #  /msys64/mingw64/bin/libwinpthread-1.dll
        # ...
        target_link_options(runTests PRIVATE -static)
        if(HAVE_CV2PDB)
            # to make stackwalk work on mingw for backtraces
            add_custom_command(OUTPUT runTests.pdb
                    COMMAND ${CV2PDB} runTests.exe
                    DEPENDS runTests.exe)
            add_custom_target(pdb DEPENDS runTests.pdb)
        endif()
    endif()
    target_link_directories(runTests PRIVATE ${CHECK_LIBRARY_DIRS})
    target_link_libraries(runTests PRIVATE ${staticlib} ${CHECK_LIBRARIES})

    add_custom_target(tests
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/runTests > run.log 2>&1
            COMMAND echo "* * *   Success   * * *"
            DEPENDS runTests)
    if(HAVE_CV2PDB)
        add_dependencies(tests pdb)
    endif()

    # memcheck on darwin
    find_program(LEAKS leaks)
    if(LEAKS)
        add_custom_target(leaks
                COMMAND CK_FORK=no MallocStackLogging=1 ${LEAKS}
                --list --atExit --
                ${CMAKE_CURRENT_BINARY_DIR}/runTests > run.log 2>&1
                USES_TERMINAL
                DEPENDS runTests)
    endif()
    # memcheck on linux
    find_program(VALGRIND valgrind)
    if(VALGRIND)
        add_custom_target(grind
                COMMAND rm -f grind.log
                COMMAND CK_FORK=no ${VALGRIND} --num-callers=50
                    --read-var-info=yes --show-reachable=yes
                    --leak-check=full --log-file=grind.log
                    ${CMAKE_CURRENT_BINARY_DIR}/runTests > run.log 2>&1
                COMMAND ! grep -A5 'are definitely lost' grind.log
                COMMAND echo "* * *   Success   * * *"
                DEPENDS runTests)
    endif()
endif()
