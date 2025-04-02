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

# build functionality
set(LOG_NONE 0)
set(LOG_INFO 1)
set(LOG_VERB 2)
function (doLog level msg)
    if (${logLevel} EQUAL ${level} OR ${logLevel} GREATER ${level})
        message(${msg})
    endif()
endfunction()

#function(docopy srcDir destDir fileglob)
#    file(GLOB includes RELATIVE ${srcDir} ${srcDir}/${fileglob})
#    foreach(file ${includes})
#        if(NOT IS_DIRECTORY ${srcDir}/${file})
#            configure_file(${srcDir}/${file} ${destDir}/${file} COPYONLY)
#        endif()
#    endforeach()
#endfunction()

function(dojoin mylist myseparator myoutvar)
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

function(dump match)
    if(match EQUAL "")
        set(all 1)
        message("VARIABLES:")
    else()
        set(all 0)
        message("VARIABLES matching: ${match}")
    endif()

    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        if(all OR "${_variableName}" MATCHES "${match}")
            string(SUBSTRING "${${_variableName}}" 0 200 VAL)
            string(REGEX REPLACE \n "\\\\n" VAL "${VAL}")
            message("${_variableName}: ${VAL}")
        endif()
    endforeach()
endfunction()

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12)
    cmake_policy(SET CMP0074 NEW)
endif()
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.14)
    cmake_policy(SET CMP0082 NEW)
endif()

function(extractdebug target)
    add_custom_target(${target}.dbg ALL
            COMMAND ${CMAKE_OBJCOPY} --only-keep-debug
            $<TARGET_FILE:${target}>
            $<TARGET_FILE:${target}>.debug
            COMMAND ${CMAKE_STRIP} --strip-debug
            $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink
            $<TARGET_FILE:${target}>.debug
            $<TARGET_FILE:${target}>
            DEPENDS ${target}
            COMMENT "Extracting debug symbols"
            VERBATIM
            )
    install(FILES $<TARGET_FILE:${target}>.debug
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/debug
            RENAME $<TARGET_FILE_NAME:${target}>)
endfunction()

macro (installLicensePath var name newname path)
    file(GLOB DEF_LICENSE_PATH ${path})
    set(${var}_LICENSE_PATH ${DEF_LICENSE_PATH} CACHE STRING
            "Where the ${name} License file for packaging can be found")
    if(${var}_LICENSE_PATH MATCHES "$^")
        # make this fail on install
        install(FILES ${path} DESTINATION ${LIC_FOLDER})
    else()
        message("Bundling ${name} License ${${var}_LICENSE_PATH}")
        install(FILES ${${var}_LICENSE_PATH}
                DESTINATION ${LIC_FOLDER}
                RENAME ${newname})
    endif()
endmacro()

if(MSVC AND MSVC_STATIC_RUNTIME)
    foreach(flag_var
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_C_COMPILE_OPTIONS_MSVC_RUNTIME_LIBRARY_MultiThreadedDLL
            CMAKE_C_COMPILE_OPTIONS_MSVC_RUNTIME_LIBRARY_MultiThreadedDebugDLL
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_COMPILE_OPTIONS_MSVC_RUNTIME_LIBRARY_MultiThreadedDLL
            CMAKE_CXX_COMPILE_OPTIONS_MSVC_RUNTIME_LIBRARY_MultiThreadedDebugDLL
            )
        string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
        string(REGEX REPLACE "-MD" "-MT" ${flag_var} "${${flag_var}}")
        string(REGEX REPLACE "/Zi" "/Z7" ${flag_var} "${${flag_var}}")
        string(REGEX REPLACE "-Zi" "-Z7" ${flag_var} "${${flag_var}}")
    endforeach(flag_var)
    message("Using static runtime")
endif()
