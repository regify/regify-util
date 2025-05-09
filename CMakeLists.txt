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

#<editor-fold desc="options">
set(NAME regify-util)
set(VERSION 2.1.0 CACHE STRING "The package version")
set(REVISION 0 CACHE STRING "The build revision")
set(DEPS_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE STRING "General dependency installation prefix")
set(ICU_PREFIX ${DEPS_PREFIX} CACHE STRING "Prefix path under which to find ICU")
set(TIDY_PREFIX ${DEPS_PREFIX} CACHE STRING "libtidy installation prefix")
set(YAJL_PREFIX ${DEPS_PREFIX} CACHE STRING "yajl installation prefix")
set(CMAKE_INSTALL_LIBDIR "lib" CACHE STRING "Folder under prefix where the libs are stored")
set(DOC_FOLDER "share/${NAME}/doc" CACHE STRING "Folder under prefix where the libs are stored")
set(LIC_FOLDER "share/${NAME}" CACHE STRING "Folder under prefix where the libs are stored")
option(BUILD_SHARED_LIBS "Whether to build dynamic libraries" OFF)
option(INCLUDE_ICU "Whether to include ICU statically" ON)
option(INCLUDE_TIDY "Whether to include libtidy statically" ON)
option(INCLUDE_YAJL "Whether to include YAJL statically" ON)
option(INSTALL_DEPS "Whether this configuration installs its included dependencies. Used for packaging" OFF)
option(DOCS "Whether to generate the documentation" OFF)
option(TESTING "Whether to build the tests" OFF)
option(PACKAGING "Whether to generate a package targets" ON)
option(TGZSOURCE "Whether to generate a source tar file instead of zip" OFF)
option(SRC_ONLY "Whether to only generate a source tarball, ignore dependencies" OFF)

option(MSVC_STATIC_RUNTIME "Whether to link against the static runtime library" ON)
#</editor-fold>

#<editor-fold desc="general">
cmake_minimum_required(VERSION 3.13)
set(staticlib ${NAME}_static)
set(sharedlib ${NAME})
project(${NAME}
        LANGUAGES C CXX
        VERSION ${VERSION}
        )
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

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
set(SOCUR 1)
set(SOREV 0)
set(SOAGE 0)
set(SOVERSION ${SOCUR}.${SOREV}.${SOAGE})

include(resources/helpers.cmake)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(logLevel LOG_VERB)
    doLog(${logLevel} "Log Level ${logLevel}")
else()
    set(logLevel LOG_INFO)
endif()
#</editor-fold>

#<editor-fold desc="platform switches">
doLog(LOG_VERB "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

string(TOLOWER ${CMAKE_SYSTEM_NAME} NAME_OS)
string(SUBSTRING ${NAME_OS} 0 3 NAME_OS)
if(CMAKE_SYSTEM_NAME MATCHES "^Linux$")
    set(LINUX 1)
    set(CONDS "linux")

elseif(CMAKE_SYSTEM_NAME MATCHES "^Emscripten$")
    set(LINUX 1)
    set(WASM 1)
    set(CONDS "wasm")

elseif(CMAKE_SYSTEM_NAME MATCHES "^Android$")
    set(ANDROID 1)
    set(CONDS "android")

elseif(CMAKE_SYSTEM_NAME MATCHES "^Darwin$")
    set(MAC 1)
    set(NAME_OS mac)
    add_definitions (-DITS_OSX)
    set(CONDS "mac")

elseif(CMAKE_SYSTEM_NAME MATCHES "^Windows$")
    set(WIN 1)
    if(NAME_ARCH MATCHES "^mingw")
        set(CONDS "win mingw")
        set(MINGW 1)
    else()
        set(CONDS "win")
    endif()

elseif(CMAKE_SYSTEM_NAME MATCHES "^iOS$")
    set(IOS 1)
    add_definitions (-DITS_IOS)
    set(CONDS "ios")

else()
    message(FATAL_ERROR "Unknown platform")
endif()

if (MAC OR LINUX)
    if(NOT NAME_ARCH)
        set(NAME_ARCH x86_64)
    endif()
endif()

if(MAC)
    find_library(CORESVC CoreServices)
endif()

if(LINUX OR ANDROID)
    option(DEBUG_SYMBOLS "Whether to extract binary debug symbols" ON)
endif()

if(NOT NAME_ARCH)
    message(FATAL_ERROR "NAME_ARCH not set")
endif()
#</editor-fold>

#<editor-fold desc="dependencies">
if(NOT SRC_ONLY)
if(LINUX OR MAC OR MINGW)
    find_package(Threads REQUIRED)
endif()

if(MINGW)
    set(CV2PDB_DIR "" CACHE STRING "Folder where cv2pdb can be found")
    find_program(CV2PDB
            NAMES cv2pdb cv2pdb.exe
            PATHS ${CV2PDB_DIR})
    if(NOT "CV2PDB-NOTFOUND" STREQUAL "${CV2PDB}")
        set(HAVE_CV2PDB 1)
        message("Have cv2pdb")
    endif()
endif()

if(SKIP_TRACE)
    message("Skipping backtrace functionality")
else()
    if (WIN)
        set(STACKWALK 1)
    else()
        # this nonsense is needed because CHECK_INCLUDE_FILE doesn't exists for
        # android and ios while it does exist for darwin on the same system
        get_directory_property(DEFINED_MACROS DIRECTORY
                ${CMAKE_CURRENT_SOURCE_DIR} MACROS)
        list(FIND DEFINED_MACROS "CHECK_INCLUDE_FILE" MACRO_INDEX)
        if(MACRO_INDEX EQUAL -1)
            # Macro does not exist
            message("no CHECK_INCLUDE_FILE")
        else()
            CHECK_INCLUDE_FILE(backtrace.h ILTBacktrace
                    CMAKE_REQUIRED_DEFINITIONS=backtrace_create_state;backtrace_full)
        endif()
        if (NOT ILTBacktrace)
            find_path (Unwind_INCLUDE_DIR
                    NAMES unwind.h libunwind.h
                    DOC "unwind include directory")
            find_library (Unwind_LIBRARY
                    NAMES unwind
                    DOC "unwind library")
            if (Unwind_INCLUDE_DIR)
                message("Unwind_INCLUDE_DIR: '${Unwind_INCLUDE_DIR}'")
                message("Unwind_LIBRARY: '${Unwind_LIBRARY}'")
                if (NOT IOS OR Unwind_LIBRARY)
                    set(Unwind_FOUND 1)
                endif()
            endif()
        endif()
    endif()
endif()

find_package(PkgConfig)
doLog(LOG_VERB "PkgConfig_FOUND: '${PkgConfig_FOUND}'")
if(PkgConfig_FOUND)
    if ("${ICU_PREFIX}" MATCHES "^$")
        pkg_check_modules(ICU icu-io)
        doLog(LOG_VERB "ICU_FOUND: '${ICU_FOUND}'")
    endif()
    if ("${YAJL_PREFIX}" MATCHES "^$")
        pkg_check_modules(YAJL yajl)
        doLog(LOG_VERB "YAJL_FOUND: '${YAJL_FOUND}'")
    endif()
    if ("${TIDY_PREFIX}" MATCHES "^$")
        pkg_check_modules(TIDY tidy)
        doLog(LOG_VERB "TIDY_FOUND: '${TIDY_FOUND}'")
    endif()
endif()

if(NOT ICU_FOUND)
    set(ICU_INCLUDE_DIRS ${ICU_PREFIX}/include)
    set(ICU_LIBRARY_DIRS ${ICU_PREFIX}/lib)
    if(WIN AND NOT MINGW)
        if(BUILD_SHARED_LIBS AND NOT INCLUDE_ICU)
            set(ICU_LIBRARIES icuio icuin icuuc icudt)
        else()
            set(ICU_LIBRARIES sicuio sicuin sicuuc sicudt)
        endif()
    else()
        set(ICU_LIBRARIES -licuio -licui18n -licuuc -licudata -lm -lstdc++)
    endif()
endif()

if(NOT YAJL_FOUND)
    set(YAJL_INCLUDE_DIRS ${YAJL_PREFIX}/include)
    set(YAJL_LIBRARY_DIRS ${YAJL_PREFIX}/lib)
    if(WIN AND NOT MINGW)
        set(YAJL_LIBRARIES yajl)
    else()
        set(YAJL_LIBRARIES -lyajl)
    endif()
endif()

if(NOT TIDY_FOUND)
    set(TIDY_INCLUDE_DIRS ${TIDY_PREFIX}/include)
    set(TIDY_LIBRARY_DIRS ${TIDY_PREFIX}/lib)
    if(WIN AND NOT MINGW)
        set(TIDY_LIBRARIES tidy)
    else()
        set(TIDY_LIBRARIES -ltidy)
    endif()
endif()

# tidy header storage is a mess
find_path(TIDY_HEADER_DIR tidy.h
        PATHS ${TIDY_INCLUDE_DIRS} ${TIDY_INCLUDEDIR}
        PATH_SUFFIXES tidy
        NO_DEFAULT_PATH
        NO_CMAKE_FIND_ROOT_PATH)

add_custom_target(entities
        COMMAND ${CMAKE_SOURCE_DIR}/resources/mkmap.py > ${CMAKE_SOURCE_DIR}/lib/htmlEntities.c)

if(INCLUDE_ICU)
    installLicensePath(ICU ICU ICU-license.txt ${ICU_PREFIX}/share/icu/LICENSE)
endif()
if(INCLUDE_YAJL)
    installLicensePath(YAJL YAJL YAJL-license.txt ${YAJL_PREFIX}/share/yajl/LICENSE)
endif()
if(INCLUDE_TIDY)
    installLicensePath(TIDY tidy tidy-license.txt ${TIDY_PREFIX}/share/tidy/LICENSE)
endif()

add_compile_definitions(BUILD_VERSION=\"${VERSION}-${REVISION}\")
add_subdirectory(lib)
endif(NOT SRC_ONLY)
#</editor-fold>

#<editor-fold desc="installation">
if(NOT SRC_ONLY)
include(CMakePackageConfigHelpers)
# generate the config file that is includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/resources/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${NAME}Config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${NAME}"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO )
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${NAME}ConfigVersion.cmake"
        VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
        COMPATIBILITY AnyNewerVersion
)

set(INCLUDES
        include/regify-util/cleaner.h
        include/regify-util/errors.h
        include/regify-util/html.h
        include/regify-util/ini.h
        include/regify-util/io.h
        include/regify-util/json.h
        include/regify-util/kvstore.h
        include/regify-util/list.h
        include/regify-util/logging.h
        include/regify-util/map.h
        include/regify-util/regex.h
        include/regify-util/string.h
        include/regify-util/thread.h
        include/regify-util/types.h
        )
if (LINUX OR WIN OR MAC)
    list(APPEND INCLUDES include/regify-util/fam.h)
endif()

foreach(hdr IN LISTS INCLUDES)
    set(DOXY_SOURCES "${DOXY_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/${hdr}")
endforeach()

configure_file(include/regify-util.h regify-util.h @ONLY)

install(FILES ${CMAKE_BINARY_DIR}/regify-util.h DESTINATION include)
install(FILES ${INCLUDES} DESTINATION include/regify-util)
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${NAME}
        )
install(EXPORT ${NAME}Targets
        FILE ${NAME}Targets.cmake
        NAMESPACE RegifyUtil::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${NAME} )
if(NOT MSVC AND NOT WASM)
    if(NOT INCLUDE_ICU)
        dojoin("-l${ICU_LIBRARIES}" " -l" ICU_LIBS)
    endif()
    configure_file (
            "${PROJECT_SOURCE_DIR}/resources/${NAME}.pc.in"
            "${PROJECT_BINARY_DIR}/${NAME}.pc"
            @ONLY)
    install(FILES ${PROJECT_BINARY_DIR}/${NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()
install(FILES LICENSE DESTINATION ${LIC_FOLDER} RENAME regify-util-license.txt)
endif(NOT SRC_ONLY)
#</editor-fold>

#<editor-fold desc="documentation & packaging">
if(DOCS)
    # add a target to generate API documentation with Doxygen
    # add these to your PATH id they're not found even though they are installed
    find_package(Doxygen REQUIRED)
    doLog(LOG_VERB "DOXYGEN_EXECUTABLE ${DOXYGEN_EXECUTABLE}")
    # add a target to generate API documentation with Doxygen
    if (WIN)
        set(PREDEFS "_WIN32")
    else()
        set(PREDEFS "")
    endif()

    configure_file(${CMAKE_SOURCE_DIR}/resources/Doxyfile.in
            ${CMAKE_BINARY_DIR}/Doxfile @ONLY)

    add_custom_target(doc ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxfile
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM)

    install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html/
            DESTINATION ${DOC_FOLDER})
endif()
if(PACKAGING)
    # packaging
    include(InstallRequiredSystemLibraries)
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION}-${REVISION})
    set(CPACK_SYSTEM_NAME ${NAME_OS}.${NAME_ARCH})
    set(CPACK_GENERATOR ZIP)
    list(APPEND CPACK_SOURCE_IGNORE_FILES ${CMAKE_SOURCE_DIR}/cmake-)
    list(APPEND CPACK_SOURCE_IGNORE_FILES ${CMAKE_SOURCE_DIR}/build)
    list(APPEND CPACK_SOURCE_IGNORE_FILES .git)
    list(APPEND CPACK_SOURCE_IGNORE_FILES ${CMAKE_SOURCE_DIR}/.idea/)
    list(APPEND CPACK_SOURCE_IGNORE_FILES ${CMAKE_SOURCE_DIR}/tests/tree/[^/]+\.out)
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}"
            CACHE STRING "The source package base name")
    if(TGZSOURCE)
        set(CPACK_SOURCE_GENERATOR TGZ)
    else()
        set(CPACK_SOURCE_GENERATOR ZIP)
    endif()
    include(CPack)
endif()
#</editor-fold>

#<editor-fold desc="tests">
if(TESTING)
    doLog(LOG_INFO "Adding unit tests")
    add_subdirectory(tests)
else()
    doLog(LOG_INFO "Skipping unit tests")
endif()
#dump("")
#</editor-fold>
