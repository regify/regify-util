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

function (doLink lib shared)

    if (INCLUDE_ICU AND shared)
        set(icu_scope PRIVATE)
        set(icu_libs ${ICU_ARCHIVES})
    else()
        set(icu_scope PUBLIC)
        set(icu_libs ${ICU_LIBRARIES})
    endif()

    target_link_libraries(${lib} ${icu_scope} ${icu_libs})

    if(WIN AND NOT MINGW)
        target_compile_definitions(${lib} PRIVATE _CRT_SECURE_NO_DEPRECATE)
        target_compile_options(${lib}
                PRIVATE /MP /Wall /WX /wd4100 /wd4204 /wd4255 /wd4464 /wd4625
                /wd4626 /wd4668 /wd4710 /wd4711 /wd4800 /wd4820 /wd5039 /wd5045
                )

    else()
        if(LINUX)
            target_compile_definitions(${lib} PRIVATE _GNU_SOURCE)
        endif()

        if (NOT ANDROID)
            target_link_libraries(${lib} PUBLIC pthread)
        endif()
        target_compile_options(${lib}
                PRIVATE -Wall -Werror --pedantic
                ${CMAKE_C_COMPILE_OPTIONS_PIC}
                -ffunction-sections -fdata-sections
        )
        if(shared)
            if(LINUX)
                target_link_libraries(${lib} ${icu_scope} -ldl)
            endif()
            if(ANDROID)
                target_link_libraries(${lib} ${icu_scope} -lc++_shared)
            endif()
            target_link_options(${lib}
                    PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden
                    )
            if (CMAKE_C_COMPILER_ID MATCHES "AppleClang")
                target_link_options(${lib}
                        PRIVATE -Wl,-dead_strip
                        )
            else()
                target_link_options(${lib}
                        PRIVATE -Wl,--gc-sections -Wl,--exclude-libs,ALL
                        )
            endif()

            set_target_properties(${lib} PROPERTIES SOVERSION ${SOVERSION})
        endif()
    endif()

    if(shared)
        if (NOT INCLUDE_ICU)
            target_link_directories(${lib} PRIVATE ${ICU_LIBRARY_DIRS})
        endif()
    else()
        target_link_directories(${lib} PUBLIC ${ICU_LIBRARY_DIRS})
    endif()


    target_include_directories(${lib}
            PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
            # make sure we're using the right version
            PRIVATE ${ICU_INCLUDE_DIRS}
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            )

endfunction()

if(INCLUDE_ICU)
    foreach(lib IN LISTS ICU_LIBRARIES)
        if("${lib}" MATCHES "^-licu")
            string(SUBSTRING ${lib} 2 -1 base)
            list(APPEND ICU_ARCHIVES ${ICU_LIBRARY_DIRS}/${CMAKE_STATIC_LIBRARY_PREFIX}${base}${CMAKE_STATIC_LIBRARY_SUFFIX})
        endif()
        if("${lib}" MATCHES "^s?icu")
            list(APPEND ICU_ARCHIVES ${ICU_LIBRARY_DIRS}/${CMAKE_STATIC_LIBRARY_PREFIX}${lib}${CMAKE_STATIC_LIBRARY_SUFFIX})
        endif()
    endforeach()
endif()

# icu.cpp compiled as C++ so we can use thread_local on ios9+
# And also to cope with C++ symbols stemming from ICU
file(GLOB SRCS cleaner.c icu.cpp ini.c io.c kvstore.c lib.c list.c logging.c map.c
        regex.c string.c thread.c regify-util.c)


if(NOT WASM)
    # pwcleaner.a
    add_library(${CLEANER} STATIC cleaner.c)
    target_compile_definitions(${CLEANER} PRIVATE CLEANER_ONLY )
    install(TARGETS ${CLEANER}
            EXPORT ${NAME}Targets
            DESTINATION lib
            )
endif()

# regify-util.a
add_library(${staticlib} STATIC ${SRCS})
doLink(${staticlib} OFF)
if(NOT WIN OR MINGW)
    set_target_properties(${staticlib} PROPERTIES OUTPUT_NAME ${NAME})
endif()
install(TARGETS ${staticlib}
        EXPORT ${NAME}Targets
        DESTINATION lib
        )
if(INSTALL_ICU AND INCLUDE_ICU)
    install(FILES ${ICU_ARCHIVES} DESTINATION lib)
endif()

# regify-util.so
if(BUILD_SHARED_LIBS)
    add_library(${sharedlib} SHARED ${SRCS})
    doLink(${sharedlib} ON)
    install(TARGETS ${sharedlib}
            EXPORT ${NAME}Targets
            DESTINATION lib
            )
endif()
