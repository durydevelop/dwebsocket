#### Find IxWebSocket lib

set(IXWEBSOCKET_INSTALL FALSE) # disable install target
function(get_package_from_git)
    include(FetchContent)
    set(FETCHCONTENT_QUIET OFF)
    set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
    set(FETCHCONTENT_BASE_DIR _deps)
    FetchContent_Declare(
        ${ARGV0}
        GIT_REPOSITORY ${ARGV1}
        #GIT_TAG main
        #GIT_PROGRESS TRUE
    )
    # Check if population has already been performed
    FetchContent_GetProperties(${ARGV0})
    if(NOT ${ARGV0}_POPULATED)
        FetchContent_Populate(${ARGV0})
        add_subdirectory(${${ARGV0}_SOURCE_DIR} ${${ARGV0}_BINARY_DIR})
    endif()
    FetchContent_MakeAvailable(${ARGV0})
endfunction()

if (USE_EXTERNAL_IXWEBSOCKET)
    # Do not use internal IXWebSocket
    message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} is finding <${BOLD_CYAN}ixwebsocket${BOLD_MAGENTA}>")
    if (ixwebsocket_GIT_REPOSITORY)
        # GIT forced
        get_package_from_git(ixwebsocket ${ixwebsocket_GIT_REPOSITORY})
        if (ixwebsocket_FOUND)
            message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}ixwebsocket${BOLD_MAGENTA}> loaded custom repo")
        endif()
    elseif(ixwebsocket_ROOT)
        # Local FORCED
        add_subdirectory(${ixwebsocket_ROOT} _deps/ixwebsocket)
    else()
        # Try to find in system
        find_package(ixwebsocket QUIET CONFIG)
        if (ixwebsocket_FOUND)
            if (ZLIB_FOUND)
                link_libraries(ZLIB::ZLIB)
            endif()
            get_target_property(ixwebsocket_INCLUDE_DIRECTORIES ixwebsocket::ixwebsocket INTERFACE_INCLUDE_DIRECTORIES)
            message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}ixwebsocket${BOLD_MAGENTA}> found in system")
        else()
            # Load from official repo
            get_package_from_git(ixwebsocket https://github.com/machinezone/IXWebSocket.git)
            set(ixwebsocket_FOUND TRUE)
            message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}ixwebsocket${BOLD_MAGENTA}> loaded from official repo")
        endif()
    endif()
else()
    # Use embedded one
    message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} use <${BOLD_CYAN}ixwebsocket${BOLD_MAGENTA}> embedded")
    add_subdirectory(external/IXWebSocket)
    install(
        DIRECTORY ${ixwebsocket_SOURCE_DIR}/ixwebsocket
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_TARGET_NAME}"
        FILES_MATCHING PATTERN "*.h"
    )
endif()

#get_target_property(dmpacket_SOURCE_DIR dmpacket SOURCE_DIR)
get_target_property(ixwebsocket_INCLUDE_DIRECTORIES ixwebsocket::ixwebsocket INTERFACE_INCLUDE_DIRECTORIES)
