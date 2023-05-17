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

# options
set(VERSION 2.0.0 CACHE STRING "The package version")
set(REVISION 0 CACHE STRING "The build revision")
set(ICU_PREFIX "" CACHE STRING "Prefix path under which to find ICU")
set(LIB_FOLDER "lib" CACHE STRING "Folder under prefix where the libs are stored")
option(BUILD_SHARED_LIBS "Whether to build dynamic libraries" OFF)
option(INCLUDE_ICU "Whether to include ICU statically" ON)
option(INSTALL_ICU "Whether this configuration installs its included ICU dependency. Used for packaging" OFF)
option(DOCS "Whether to generate the documentation" OFF)
option(TESTING "Whether to build the tests" OFF)
option(TGZSOURCE "Whether to generate a source tar file instead of zip" OFF)

option(MSVC_STATIC_RUNTIME "Whether to link against the static runtime library" ON)

# general
cmake_minimum_required(VERSION 3.13)
set(NAME regify-util)
set(staticlib ${NAME}_static)
set(sharedlib ${NAME})
project(${NAME}
        LANGUAGES C CXX
        VERSION ${VERSION}
        )
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

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
set(SOVERSION 1.0.0)

include(resources/helpers.cmake)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(logLevel LOG_VERB)
    doLog(${logLevel} "Log Level ${logLevel}")
else()
    set(logLevel LOG_INFO)
endif()

doLog(LOG_VERB "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

string(TOLOWER ${CMAKE_SYSTEM_NAME} NAME_OS)
string(SUBSTRING ${NAME_OS} 0 3 NAME_OS)
# platform switches
if(CMAKE_SYSTEM_NAME MATCHES "^Linux$")
    set(LINUX 1)

elseif(CMAKE_SYSTEM_NAME MATCHES "^Emscripten$")
    set(LINUX 1)
    set(WASM 1)

elseif(CMAKE_SYSTEM_NAME MATCHES "^Android$")
    set(ANDROID 1)

elseif(CMAKE_SYSTEM_NAME MATCHES "^Darwin$")
    set(MAC 1)
    set(NAME_OS mac)
    add_definitions (-DITS_OSX)

elseif(CMAKE_SYSTEM_NAME MATCHES "^Windows$")
    set(WIN 1)
    if(NAME_ARCH MATCHES "^mingw")
        set(MINGW 1)
    endif()

elseif(CMAKE_SYSTEM_NAME MATCHES "^iOS$")
    set(IOS 1)
    add_definitions (-DITS_IOS)

else()
    message(FATAL_ERROR "Unknown platform")
endif()

if (MAC OR LINUX)
    if(NOT NAME_ARCH)
        set(NAME_ARCH x86_64)
    endif()
endif()

if(NOT NAME_ARCH)
    message(FATAL_ERROR "NAME_ARCH not set")
endif()

# dependencies
if(LINUX OR MAC OR MINGW)
    find_package(Threads REQUIRED)
endif()

if ("${ICU_PREFIX}" MATCHES "^$")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(ICU REQUIRED icu-io)
else()
    # where static dependencies are found
    set(ICU_INCLUDE_DIRS ${ICU_PREFIX}/include)
    set(ICU_LIBRARY_DIRS ${ICU_PREFIX}/${LIB_FOLDER})
    if(WIN AND NOT MINGW)
        if(BUILD_SHARED_LIBS AND NOT INCLUDE_ICU)
            set(ICU_LIBRARIES icuio icuin icuuc icudt)
        else()
            set(ICU_LIBRARIES sicuio sicuin sicuuc sicudt)
        endif()
    else()
        set(ICU_LIBRARIES -licuio -licui18n -licuuc -licudata -lm)
    endif()
endif()

if(INCLUDE_ICU AND DOCS)
    file(GLOB DEF_ICU_LICENSE_PATH ${ICU_PREFIX}/share/icu/LICENSE)
    set(ICU_LICENSE_PATH ${DEF_ICU_LICENSE_PATH} CACHE STRING
            "Where the ICU License file for packaging can be found")
    if(ICU_LICENSE_PATH MATCHES "^$")
        message(FATAL_ERROR "We need the ICU License file for packaging set ICU_LICENSE_PATH to its location")
    endif()
endif()

add_compile_definitions(BUILD_VERSION=\"${VERSION}-${REVISION}\")
add_subdirectory(lib)

# installation
include(CMakePackageConfigHelpers)
# generate the config file that is includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/resources/Config.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/${NAME}Config.cmake"
        INSTALL_DESTINATION "${LIB_FOLDER}/cmake/${NAME}"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO )
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${NAME}ConfigVersion.cmake"
        VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
        COMPATIBILITY AnyNewerVersion
)
install(DIRECTORY include/ DESTINATION include)
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${NAME}ConfigVersion.cmake
        DESTINATION ${LIB_FOLDER}/cmake/${NAME}
        )
install(EXPORT ${NAME}Targets
        FILE ${NAME}Targets.cmake
        NAMESPACE RegifyUtil::
        DESTINATION ${LIB_FOLDER}/cmake/${NAME} )
if(NOT MSVC AND NOT WASM)
    if(NOT INCLUDE_ICU)
        dojoin("-l${ICU_LIBRARIES}" " -l" ICU_LIBS)
    endif()
    configure_file (
            "${PROJECT_SOURCE_DIR}/resources/${NAME}.pc.in"
            "${PROJECT_BINARY_DIR}/${NAME}.pc"
            @ONLY)
    install(FILES ${PROJECT_BINARY_DIR}/${NAME}.pc DESTINATION ${LIB_FOLDER}/pkgconfig)
endif()

# documentation & packaging
if(DOCS)
    install(FILES LICENSE DESTINATION share/${NAME})
    if(INCLUDE_ICU)
        message("Bundling ICU License ${ICU_LICENSE_PATH}")
        install(FILES ${ICU_LICENSE_PATH}
                DESTINATION share/${NAME}
                RENAME ICU-LICENSE)
    endif()

    # add a target to generate API documentation with Doxygen
    # add these to your PATH id they're not found even though they are installed
    find_package(Doxygen REQUIRED)
    doLog(LOG_VERB "DOXYGEN_EXECUTABLE ${DOXYGEN_EXECUTABLE}")
    # add a target to generate API documentation with Doxygen
    if (WIN)
        set(PREDEFS "WINDOWS")
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
            DESTINATION share/${NAME}/doc)

    # packaging
    include(InstallRequiredSystemLibraries)
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
    set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION}-${REVISION})
    set(CPACK_SYSTEM_NAME ${NAME_OS}.${NAME_ARCH})
    set(CPACK_GENERATOR ZIP)
    list(APPEND CPACK_SOURCE_IGNORE_FILES ${CMAKE_SOURCE_DIR}/cmake-.*)
    list(APPEND CPACK_SOURCE_IGNORE_FILES ${CMAKE_SOURCE_DIR}/build.*)
    list(APPEND CPACK_SOURCE_IGNORE_FILES ${CMAKE_SOURCE_DIR}/.git/)
    list(APPEND CPACK_SOURCE_IGNORE_FILES ${CMAKE_SOURCE_DIR}/.idea/)
    list(APPEND CPACK_SOURCE_IGNORE_FILES ${CMAKE_SOURCE_DIR}/tests/tree/.*out)
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
    if(TGZSOURCE)
        set(CPACK_SOURCE_GENERATOR TGZ)
    else()
        set(CPACK_SOURCE_GENERATOR ZIP)
    endif()
    include(CPack)

endif()

# tests
if(TESTING)
    doLog(LOG_INFO "Adding unit tests")
    add_subdirectory(tests)
else()
    doLog(LOG_INFO "Skipping unit tests")
endif()
#dump("")
