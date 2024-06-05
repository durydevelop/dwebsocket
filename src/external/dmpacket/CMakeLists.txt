set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/dmpacket.cpp
)

set(HDR
    ${CMAKE_CURRENT_SOURCE_DIR}/dmpacket
    ${CMAKE_CURRENT_SOURCE_DIR}/dmpacket.h
)

#### PUBLIC_HEADER
add_library(dmpacket STATIC ${SRC} ${HDR})
add_library(dmpacket::dmpacket ALIAS dmpacket)
set_target_properties(dmpacket PROPERTIES PUBLIC_HEADER "${HDR}")
target_include_directories(dmpacket PUBLIC
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>"
)

#### FILE_SET HEADRES
#[[
add_library(dmpacket STATIC ${SRC})
add_library(dmpacket::dmpacket ALIAS dmpacket)
target_sources(dmpacket PUBLIC
    FILE_SET HEADERS FILES "${HDR}"
)
target_include_directories(dmpacket PUBLIC
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>"
)
install(TARGETS dmpacket
    EXPORT dmpacket
    FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
)
]]#

#print_target_prop(dmpacket)
#message_c("CMAKE_INSTALL_INCLUDEDIR=${CMAKE_INSTALL_INCLUDEDIR}")
#set_target_properties(dmpacket PROPERTIES LINKER_LANGUAGE CXX)
