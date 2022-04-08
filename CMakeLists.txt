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
cmake_minimum_required(VERSION 3.5)

# The main project
set(NAME regify-util)
project(${NAME})

# A separate sub project that only cleans passwords. Does not need ICU.
set(CLEANER pwcleaner)

#  Here are a set of rules to help you update your library version information:
#  Start with version information of `0.0.0' for each libtool library.
#  Update the version information only immediately before a public release of
#  your software. More frequent updates are unnecessary, and only guarantee that
#  the current interface number gets larger faster.
#  If the library source code has changed at all since the last update, then
#  increment revision (`c.r.a' becomes `c.r+1.a').
#  If any interfaces have been added, removed, or changed since the last update,
#  increment current, and set revision to 0.
#  If any interfaces have been added since the last public release, then
#  increment age.
#  If any interfaces have been removed since the last public release, then set
#  age to 0.
set(NAME_SOVERSION 0.1.0 CACHE STRING "The library version")
set(NAME_VERSION 1.2.0 CACHE STRING "The package version")
set(NAME_REVISION dev CACHE STRING "The build revision")
set(CMAKE_BUILD_TYPE Debug CACHE STRING "What to build")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Whether to build dynamic libraries")
set(TESTING OFF CACHE BOOL "Whether to build the tests")

set(DEPS_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE STRING "Prefix path under which to find ICU")

# regify utility functionality
set(LOG_NONE 0)
set(LOG_INFO 1)
set(LOG_VERB 2)
function (doLog level msg)
    if (${logLevel} EQUAL ${level} OR ${logLevel} GREATER ${level})
        message(${msg})
    endif()
endfunction()

function(docopy srcDir destDir fileglob)
    file(GLOB includes RELATIVE ${srcDir} ${srcDir}/${fileglob})
    foreach(file ${includes})
        if(NOT IS_DIRECTORY ${srcDir}/${file})
            configure_file(${srcDir}/${file} ${destDir}/${file} COPYONLY)
        endif()
    endforeach()
endfunction()

function (dojoin mylist myseparator myoutvar)
    if(CMAKE_VERSION VERSION_GREATER 3.11.99)
        # JOIN came in 3.12
        list(JOIN mylist "${myseparator}" myvar)
    else()
        foreach(l IN LISTS mylist)
            if (myvar)
                set(myvar "${myvar}${myseparator}${l}")
            else()
                set(myvar ${l})
            endif()
        endforeach()
    endif()
    set(${myoutvar} "${myvar}" PARENT_SCOPE)
endfunction()


if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(logLevel LOG_INFO)
    doLog(${logLevel} "Log Level ${logLevel}")
else()
    set(logLevel LOG_NONE)
endif()

if(CMAKE_VERSION VERSION_GREATER 3.7) # greater_equal came in 3.7
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12)
        cmake_policy(SET CMP0074 NEW)
    endif()
endif()

doLog(LOG_VERB "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
if(CMAKE_SYSTEM_NAME MATCHES "^Linux$")
    set(LINUX 1)

elseif(CMAKE_SYSTEM_NAME MATCHES "^Android$")
    set(ANDROID 1)

elseif(CMAKE_SYSTEM_NAME MATCHES "^Darwin$")
    set(MAC 1)
    add_definitions (-DITS_OSX)

elseif(CMAKE_SYSTEM_NAME MATCHES "^Windows$")
    set(WIN 1)
    if(NAME_SUFFIX MATCHES "^mingw")
        set(MINGW 1)
    endif()

elseif(CMAKE_SYSTEM_NAME MATCHES "^iOS$")
    set(IOS 1)
    add_definitions (-DITS_IOS)

else()
    message(FATAL_ERROR "Unknown platform")
endif()

if (MAC OR LINUX)
    if(NOT NAME_SUFFIX)
        set(NAME_SUFFIX x86_64)
    endif()
endif()

if(NOT NAME_SUFFIX)
    message(FATAL_ERROR "NAME_SUFFIX not set")
endif()

set(LDIR ${DEPS_PREFIX}/lib)
# where static dependencies are found
set(DEPS_LIB ${DEPS_PREFIX}/lib)
set(DEPS_INCLUDES ${DEPS_PREFIX}/include)

if (${CMAKE_PROJECT_NAME} MATCHES ${NAME})
    set(INCLUDED 0)
else()
    set(INCLUDED 1)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

if(LINUX OR MAC)
    find_package(Threads REQUIRED)
endif()

if(WIN AND NOT MINGW)
    set(ICU_LIBRARIES icuio.lib icuin.lib icuuc.lib icudt.lib)
elseif(LINUX AND BUILD_SHARED_LIBS)
    # for appliance builds
    set(ICU_LIBRARIES -licuio -licui18n -licuuc -licudata -lm -ldl)
else()
    set(ICU_LIBRARIES libicuio.a libicui18n.a libicuuc.a libicudata.a -lm)
endif()

if(NOT BUILD_SHARED_LIBS)
    # where dependencies are stored for subsequent packaging
    set(PKG_DIR "${CMAKE_BINARY_DIR}/pkg")
    set(RF_INCLUDE_DIR ${PKG_DIR}/include)
    set(RF_LIB_DIR ${PKG_DIR}/lib)

    foreach(lib IN LISTS ICU_LIBRARIES)
        if(NOT "${lib}" MATCHES "^-")
            configure_file(${DEPS_LIB}/${lib} ${RF_LIB_DIR}/${lib} COPYONLY)
            list(APPEND RF_LIBRARIES ${RF_LIB_DIR}/${lib})
        else()
            list(APPEND RF_LIBRARIES ${lib})
        endif()
    endforeach()

    # includes
    docopy(${DEPS_INCLUDES}/unicode ${RF_INCLUDE_DIR}/unicode *) # ICU
    if(CMAKE_BUILD_TYPE MATCHES "^Release$")
        docopy(${DEPS_PREFIX}/share/icu ${PKG_DIR}/share/icu LICENSE )
    endif()
else()
    set(RF_LIBRARIES ${ICU_LIBRARIES})
endif()

doLog(LOG_VERB "ICU libraries at: ${ICU_LIBRARIES}")
# s/point to install folder
doLog(LOG_VERB "RF libraries: ${RF_LIBRARIES}")

if(MSVC)
    add_definitions (
        /MP -D_CRT_SECURE_NO_DEPRECATE
        /Wall /WX /wd4100 /wd4204 /wd4255 /wd4464
        /wd4625 /wd4626 /wd4668 /wd4710 /wd4711
        /wd4820 /wd5039 /wd5045
    )
else(MSVC)
    if (NOT ANDROID)
        set(L_PTHREAD -lpthread)
    endif()
    add_definitions (
            -Wall
            -Werror
            --pedantic
            -fPIC
            -ffunction-sections
            -fdata-sections
            -fvisibility=hidden
    )
    set(CMAKE_SHARED_LINKER_FLAGS "-fvisibility=hidden \
            -fvisibility-inlines-hidden \
            -Wl,--gc-sections \
            -Wl,--exclude-libs,ALL")
endif(MSVC)
add_definitions(-DBUILD_VERSION=\"${NAME_VERSION}-${NAME_REVISION}\")


file(GLOB HEADERS include/*.h)
file(GLOB SRCS src/*.[hc] src/*.cpp)

add_library(${NAME} ${SRCS} ${HEADERS})
add_library(${CLEANER} src/cleaner.c)
target_compile_definitions(${CLEANER} PUBLIC CLEANER_ONLY)

set_target_properties(${NAME} PROPERTIES
        SOVERSION ${NAME_SOVERSION})

target_include_directories(${NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${DEPS_INCLUDES}
        $<INSTALL_INTERFACE:include>)

install(TARGETS ${NAME} DESTINATION lib)
install(TARGETS ${CLEANER} DESTINATION lib)
if (NOT BUILD_SHARED_LIBS)
    install(DIRECTORY include/ DESTINATION include)
else()
    dojoin("${RF_LIBRARIES}" " " NAME_LIBS)
    configure_file (
            "${PROJECT_SOURCE_DIR}/resources/${NAME}.pc.in"
            "${PROJECT_BINARY_DIR}/${NAME}.pc"
            @ONLY)
    install(FILES ${PROJECT_BINARY_DIR}/${NAME}.pc DESTINATION lib/pkgconfig)
endif()

if (NOT INCLUDED)
    if(TESTING)
        doLog(LOG_INFO "Adding unit tests")
        enable_testing()
        add_subdirectory(tests)
    else()
        doLog(LOG_INFO "Skipping unit tests")
    endif()

    # for documentation
    find_package(Doxygen)
    if (WIN AND NOT DOXYGEN_FOUND)
        set(DOXYGEN_EXECUTABLE /msys64/mingw32/bin/doxygen.exe)
        set(DOXYGEN_FOUND ON)
        doLog(LOG_INFO "Using hardcoded doxygen path")
    endif()

    doLog(LOG_VERB "DOXYGEN_EXECUTABLE ${DOXYGEN_EXECUTABLE}")
    # add a target to generate API documentation with Doxygen
    if(DOXYGEN_FOUND)
        if (WIN)
            set(PREDEFS "WINDOWS")
        else()
            set(PREDEFS "")
        endif()

        configure_file(${CMAKE_SOURCE_DIR}/resources/Doxyfile.in
                ${CMAKE_BINARY_DIR}/Doxfile @ONLY)

        add_custom_target(doc
                COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxfile
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen" VERBATIM)
    endif(DOXYGEN_FOUND)

endif()
