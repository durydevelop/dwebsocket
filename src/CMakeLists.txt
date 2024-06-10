set(LIBRARY_TARGET_NAME ${PROJECT_NAME})

#### Sources
# Source library files
set(${LIBRARY_TARGET_NAME}_SRC
    dwebsocket_client.cpp
    dwebsocket_em.cpp
    dwebsocket_endpoint.cpp
    dwebsocket_ix.cpp
    dwebsocket_log.cpp
    dwebsocket_server_ix.cpp
)
# Header library files
set(${LIBRARY_TARGET_NAME}_HDR
    dwebsocket
    dwebsocket_base.h
    dwebsocket_client
    dwebsocket_client.h
    dwebsocket_common.h
    dwebsocket_em.h
    dwebsocket_endpoint
    dwebsocket_ix.h
    dwebsocket_log
    dwebsocket_log.h
    dwebsocket_server
    dwebsocket_server_ix.h
)
# Include base dir
set(${LIBRARY_TARGET_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

#### Dependencies
find_package(IXWebSocket REQUIRED)
set(${LIBRARY_TARGET_NAME}_INCLUDE_DIRS ${${LIBRARY_TARGET_NAME}_INCLUDE_DIRS} ${ixwebsocket_INCLUDE_DIRECTORIES})

if (NOT TARGET dmpacket)
    find_package(DMPacket REQUIRED)
    set(${LIBRARY_TARGET_NAME}_INCLUDE_DIRS ${${LIBRARY_TARGET_NAME}_INCLUDE_DIRS} ${dmpacket_INCLUDE_DIRECTORIES})
endif()

#message("${LIBRARY_TARGET_NAME}_SRC=${${LIBRARY_TARGET_NAME}_SRC}")
#message("${LIBRARY_TARGET_NAME}_HDR=${${LIBRARY_TARGET_NAME}_HDR}")
#message("${LIBRARY_TARGET_NAME}_INCLUDE_DIRS=${${LIBRARY_TARGET_NAME}_INCLUDE_DIRS}")

#### Setup target
add_library(${LIBRARY_TARGET_NAME}
    ${${LIBRARY_TARGET_NAME}_SRC}
    ${${LIBRARY_TARGET_NAME}_HDR}
)
# Alias
add_library(${PROJECT_NAME}::${LIBRARY_TARGET_NAME} ALIAS ${LIBRARY_TARGET_NAME})

# Set two minimum target properties for the library.
# See https://cmake.org/cmake/help/latest/command/set_target_properties.html
# Properties are: 1) Library version number 2) list of corresponding public headers
set_target_properties(${LIBRARY_TARGET_NAME}
    PROPERTIES VERSION ${${PROJECT_NAME}_VERSION}
    PUBLIC_HEADER "${${LIBRARY_TARGET_NAME}_HDR}"
)

# Specify include directories for both compilation and installation process.
# The $<INSTALL_PREFIX> generator expression is useful to ensure to create
# relocatable configuration files, see https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#creating-relocatable-packages
target_include_directories(${LIBRARY_TARGET_NAME}
    PUBLIC 
        "$<BUILD_INTERFACE:${${LIBRARY_TARGET_NAME}_INCLUDE_DIRS}>"
        "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
)

target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC ixwebsocket::ixwebsocket)
target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC dmpacket)
target_link_libraries(${LIBRARY_TARGET_NAME} PUBLIC atomic) # Needs on Raspbian 11 (bullseye)

if (${PROJECT_NAME}_INSTALL)
    # Specify installation targets, typology and destination folders.
    install(TARGETS ${LIBRARY_TARGET_NAME} dmpacket
            EXPORT  ${PROJECT_NAME}
            LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT shlib
            ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT lib
            RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"                            COMPONENT bin
            PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_TARGET_NAME}" COMPONENT dev
    )

    set_target_properties(ixwebsocket PROPERTIES PUBLIC_HEADER "")
    install(TARGETS ixwebsocket
            EXPORT  ${PROJECT_NAME}
            LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT shlib
            ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                            COMPONENT lib
            RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"                            COMPONENT bin
            PUBLIC_HEADER 
    )
endif()

message_c("Created target ${PROJECT_NAME}::${LIBRARY_TARGET_NAME}")
