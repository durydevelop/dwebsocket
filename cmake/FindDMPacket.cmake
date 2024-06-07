##
if (USE_EXTERNAL_DMPACKET)
    # Do not use internal IXWebSocket
    message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} is finding <${BOLD_CYAN}dmpacket${BOLD_MAGENTA}>")
    message(FATAL "Not Yet")
    # TODO
else()
    # Use embedded one
    message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} use <${BOLD_CYAN}dmpacket${BOLD_MAGENTA}> embedded")
    add_subdirectory(external/dmpacket)
endif()

#get_target_property(dmpacket_SOURCE_DIR dmpacket SOURCE_DIR)
get_target_property(dmpacket_INCLUDE_DIRECTORIES dmpacket INTERFACE_INCLUDE_DIRECTORIES)
