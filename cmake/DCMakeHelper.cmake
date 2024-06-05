############################# Colors defines ##############################
string(ASCII 27 Esc)
set(END_COLOR   "${Esc}[m")
set(BOLD        "${Esc}[1m")
set(RED         "${Esc}[31m")
set(GREEN       "${Esc}[32m")
set(YELLOW      "${Esc}[33m")
set(BLUE        "${Esc}[34m")
set(MAGENTA     "${Esc}[35m")
set(CYAN        "${Esc}[36m")
set(WHITE       "${Esc}[37m")
set(BOLD_RED     "${Esc}[1;31m")
set(BOLD_GREEN   "${Esc}[1;32m")
set(BOLD_YELLOW  "${Esc}[1;33m")
set(BOLD_BLUE    "${Esc}[1;34m")
set(BOLD_MAGENTA "${Esc}[1;35m")
set(BOLD_CYAN    "${Esc}[1;36m")
set(BOLD_WHITE   "${Esc}[1;37m")

set(DCOLOR TRUE CACHE BOOL "Colors enabled")
message(${BOLD_MAGENTA} "Colors enabled: this is BOLD_MAGENTA${END_COLOR}")

############################### message_c() ################################
## Colored log
# ARGV0 is color
# ARGV1 is text
function(message_c)
  message(${DSTATUS} ${ARGV0} ${ARGV1} ${END_COLOR})
endfunction()

############################### print_var() ###############################
## Print all cmake VARIABLES.
# If an argument is given, only varialbles containing ARGV0 are printed.
function(print_var)
    message("#### Variables founds containing " ${ARGV0} " ####")
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    list(REMOVE_DUPLICATES _variableNames)
    foreach (_variableName ${_variableNames})
        if (ARGV0)
            unset(MATCHED)
            string(REGEX MATCH "[a-zA-Z]*${ARGV0}[a-zA-Z]*" MATCHED ${_variableName})
            if (NOT MATCHED)
                continue()
            endif()
        endif()
        set(FOUND TRUE)
        message("## ${_variableName}=${${_variableName}}")
    endforeach()
    if (NOT FOUND)
        message("## none founds")
    endif()
endfunction()

########################### print_target_prop() ###########################
## Print all TARGET VARIABLES
# Varialble are listed using COMMAND cmake --help-property-list
# If an argument is given, only varialbles containing ARGV0 are printed.
function(print_target_prop tgt)
    message("#### Variables founds for TARGET " ${tgt} " ####")
    if (ARGV1)
        message("## containing " ${ARGV1})
    endif()
    if(NOT TARGET ${tgt})
        message("## There is no target named '${tgt}'")
        return()
    endif()

    ## Get all properties that cmake supports
    execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)
    ## Convert command output into a CMake list
    STRING(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    STRING(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)
    foreach (prop ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "${CMAKE_BUILD_TYPE}" prop ${prop})
        if (${prop} MATCHES "(LOCATION)")
            # Skip prop contains LOCATION due to the POLICY restriction
            continue()
        endif()
        if (ARGV1)
            unset(MATCHED)
            string(REGEX MATCH "[a-zA-Z]*${ARGV1}[a-zA-Z]*" MATCHED ${prop})
            if (NOT MATCHED)
                continue()
            endif()
        endif()
        set(FOUND TRUE)
        get_target_property(propval ${tgt} ${prop})
        if (propval)
            message ("## ${tgt}::${prop} = ${propval}")
        endif()
    endforeach(prop)
    if (NOT FOUND)
        message("## none founds")
    endif()
endfunction()

########################### print_target_set() ############################
## Print all TARGET VARIABLES
# Varialble are listed using get_cmake_property(CMAKE_PROPERTY_LIST VARIABLES)
# If an argument is given, only varialbles containing ARGV0 are printed.
function(print_target_set tgt)
    message("#### Variables founds for TARGET " ${tgt} " ####")
    if(NOT TARGET ${tgt})
        message("## There is no target named '${tgt}'")
        return()
    endif()

    ## Get all properties that cmake supports
    execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)
    ## Convert command output into a CMake list
    STRING(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    STRING(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)
    foreach(prop ${CMAKE_PROPERTY_LIST})
        if (${prop} MATCHES "(LOCATION)")
            # Skip prop contains LOCATION due to the POLICY restriction
            continue()
        endif()
        # v for value, d for defined, s for set
        get_property(v TARGET ${tgt} PROPERTY ${prop})
        get_property(d TARGET ${tgt} PROPERTY ${prop} DEFINED)
        get_property(s TARGET ${tgt} PROPERTY ${prop} SET)
        # only produce output for values that are set
        if(s)
            message("## ${tgt}::${prop}")
            message("## Value=${v}")
            message("## Defined=${d}")
            message("## Set=${s}")
            message("")
        endif()
    endforeach()
endfunction()

############################### Entrypoint ################################
# Set stuff for stand-alone or lib project
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Stand-alone project
    set(DSTATUS ">> ")
	# Disable console popup
	if(WIN32)
		set(GUI_TYPE WIN32)
		message(${DSTATUS} "Set WIN32 gui")
	elseif(APPLE)
		set(GUI_TYPE MACOSX_BUNDLE)
		message(${DSTATUS} "Set APPLE gui")
	endif()
else()
    # Part of other project
    set(DSTATUS "${DSTATUS}>>>> ")
    message_c(${BOLD_MAGENTA} "Library <${BOLD_CYAN}${PROJECT_NAME}${BOLD_MAGENTA}> included as external project <<<<")
endif()

#include(generate_product_version)

message_c(${BOLD_GREEN} "CMake version: ${CMAKE_VERSION}")
message_c(${BOLD_GREEN} "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
if (CMAKE_CXX_STANDARD)
	message_c(${BOLD_GREEN} "C++ std: ${CMAKE_CXX_STANDARD}")
endif()

## Debug/Release config
add_custom_target(debug
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Creating the executable in the debug mode.")

add_custom_target(release
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Creating the executable in the release mode.")
