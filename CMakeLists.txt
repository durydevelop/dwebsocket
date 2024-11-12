#####################################################################################################################
# This CMake project file is based on this guide https://github.com/robotology/how-to-export-cpp-library/tree/master #
#####################################################################################################################
cmake_minimum_required(VERSION 3.13)

#### WASM support
# CMAKE_TOOLCHAIN_FILE must be set before project() otherwise it does not take effect
if (${PLATFORM} MATCHES "Web" OR EMSCRIPTEN)
    if (NOT CMAKE_TOOLCHAIN_FILE)
        if (DEFINED ENV{EMSDK})
            set(CMAKE_TOOLCHAIN_FILE "$ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" CACHE STRING "" FORCE)
            #include(${CMAKE_TOOLCHAIN_FILE})
        else()
            message(FATAL_ERROR  "Emscripten non found, cannot compile on PLATFORM Web")
        endif()
    endif()
	
	#message(STATUS "CMAKE_HOST_LINUX=${CMAKE_HOST_LINUX}")
    #if (CMAKE_HOST_LINUX)
	
	include(os_release_info)
	get_os_release_info(OS_ID OS_VERSION OS_CODENAME)
	if (OS_ID MATCHES "ubuntu")
        # EMSCRIPTEN seems not set PLATFORM_NAME pre-processor directive
		message(STATUS "Setting PLATFORM_NAME=linux")
        add_compile_definitions(PLATFORM_NAME="linux") # ...for now emscripten build works only on linux
    endif()

	# Da provare ad attivare
	set(USE_ZLIB OFF)
    
    # Flags for WebSocket
    set(EMSDK_FLAGS "${EMSDK_FLAGS} -lwebsocket.js" CACHE STRING "" FORCE) # -s WARN_ON_UNDEFINED_SYMBOLS")

    # Add compile flags
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EMSDK_FLAGS}" CACHE STRING "" FORCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSDK_FLAGS}" CACHE STRING "" FORCE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EMSDK_FLAGS}" CACHE STRING "" FORCE)
endif()

#### Create project
project(dwebsocket
    LANGUAGES CXX C
    VERSION 0.1.0.0
)

#### Common libraries options
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS yes)

#### CMake modules helper
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list (REMOVE_DUPLICATES CMAKE_MODULE_PATH)
include(DCMakeHelper)

#### Default Options
option(${PROJECT_NAME}_BUILD_SHARED_LIBS "Build libraries as shared as opposed to static" OFF)
if (PROJECT_IS_TOP_LEVEL)
    option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" ON)
    option(${PROJECT_NAME}_INSTALL "Create install target" ON)
else()
    option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples" OFF)
    option(${PROJECT_NAME}_INSTALL "Create install target" OFF)
endif()
option(${PROJECT_NAME}_BUILD_TESTING "Build tests suite" OFF)
## USE_EXTERNAL_DMPACKET
# TODO: quando DMPacket sarà stand alone
option(USE_EXTERNAL_DMPACKET "Do not use embedded version of DMPacket library" OFF)
## USE_EXTERNAL_IXWEBSOCKET
# If USE_EXTERNAL_IXWEBSOCKET is ON one of these variables can be specified:
# Use custom git repo, i.e.:
# set(ixwebsocket_GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git)
# Use a local folder, i.e.:
# get_filename_component(ixwebsocket_ROOT ../../IXWebSocket ABSOLUTE)
# If none specified:
# First:  Try to find it in system
# Second: Load from default git repo (https://github.com/machinezone/IXWebSocket.git)
option(USE_EXTERNAL_IXWEBSOCKET "Do not use embedded version of IXWebSocket library" OFF)

#### Handle options (also set ${PROJECT_NAME}_<option> and <option>)
# EMSCRIPTEN
if (${PROJECT_NAME}_EMSCRIPTEN OR EMSCRIPTEN)
    # Setup EMSCRIPTEN
    set(${PROJECT_NAME}_EMSCRIPTEN ON)
    set(EMSCRIPTEN ON)
else()
    # Setup IWebSocket lib
    set(${PROJECT_NAME}_EMSCRIPTEN OFF)
    set(EMSCRIPTEN OFF)
endif()
# Shared / static lib
if (${PROJECT_NAME}_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS ON)
    set(BUILD_SHARED_LIBS ON)
else()
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF)
endif()
# Examples
if (${PROJECT_NAME}_BUILD_EXAMPLES OR BUILD_EXAMPLES)
    set(${PROJECT_NAME}_BUILD_EXAMPLES ON)
    set(BUILD_EXAMPLES ON)
else()
    set(${PROJECT_NAME}_BUILD_EXAMPLES OFF)
    set(BUILD_EXAMPLES OFF)
endif()
# Tests
if (${PROJECT_NAME}_BUILD_TESTING OR BUILD_TESTING)
    set(${PROJECT_NAME}_BUILD_TESTING ON)
    set(BUILD_TESTING ON)
else()
    set(${PROJECT_NAME}_BUILD_TESTING OFF)
    set(BUILD_TESTING OFF)
endif()
# Install
if (${PROJECT_NAME}_INSTALL OR ${PROJECT_NAME_UPPER}_INSTALL)
    set(${PROJECT_NAME}_INSTALL ON)
    set(${PROJECT_NAME_UPPER}_INSTALL ON)
else()
    set(${PROJECT_NAME}_INSTALL OFF)
    set(${PROJECT_NAME_UPPER}_INSTALL OFF)
endif()
# DMPacket external
if (${PROJECT_NAME}_USE_EXTERNAL_DMPACKET OR USE_EXTERNAL_DMPACKET)
    set(${PROJECT_NAME}_USE_EXTERNAL_DMPACKET ON)
    set(USE_EXTERNAL_DMPACKET ON)
else()
    set(${PROJECT_NAME}_USE_EXTERNAL_DMPACKET OFF)
    set(USE_EXTERNAL_DMPACKET OFF)
endif()
# IXWebSocket external
if(${PROJECT_NAME}_USE_EXTERNAL_IXWEBSOCKET OR USE_EXTERNAL_IXWEBSOCKET)
    set(${PROJECT_NAME}_USE_EXTERNAL_IXWEBSOCKET ON)
    set(USE_EXTERNAL_IXWEBSOCKET ON)
else()
    set(${PROJECT_NAME}_USE_EXTERNAL_IXWEBSOCKET OFF)
    set(USE_EXTERNAL_IXWEBSOCKET OFF)
endif()

#### Encourage user to specify a build type (e.g. Release, Debug, etc.), otherwise set it to Debug.
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'Debug' as none was specified.")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Debug")
    endif()
endif()

#### Add target
add_subdirectory(src)

#### Install stuff
if(${PROJECT_NAME}_INSTALL)
    # Enable RPATH support for installed binaries and libraries
    include(AddInstallRPATHSupport)
    add_install_rpath_support(
        BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"
        LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"
        INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
        USE_LINK_PATH
    )
    # Create and install CMake configuration files
    include(InstallBasicPackageFiles)
    install_basic_package_files(${PROJECT_NAME}
        VERSION ${${PROJECT_NAME}_VERSION}
        COMPATIBILITY AnyNewerVersion
        VARS_PREFIX ${PROJECT_NAME}
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )
    # Add the uninstall target
    include(AddUninstallTarget)
endif()

#### Integration tests
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

#### Examples
if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

#### Doxygen support
if (PROJECT_IS_TOP_LEVEL)
    set(DOXYGEN_THEME_NAME example)
    set(DOXYGEN_LANGUAGE Italian)
    #add_subdirectory(doxygen)
    include(doxygen/Doxygen.cmake)
endif()

## C++ Pre-processor variables
# dwebsocket_VERSION
add_compile_definitions(${PROJECT_NAME}_VERSION="${PROJECT_VERSION}")
# DWEBSOCKET_VERSION (upper-case)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
add_compile_definitions(${PROJECT_NAME_UPPER}_VERSION="${PROJECT_VERSION}")

## Print summary
message_c(${BOLD_CYAN} "Summary:")
message_c(${BOLD_CYAN} "Project name:             ${PROJECT_NAME} - Version: ${PROJECT_VERSION}")
message_c(${BOLD_CYAN} "PROJECT_IS_TOP_LEVEL:     ${PROJECT_IS_TOP_LEVEL}")
message_c(${BOLD_CYAN} "BUILD_SHARED_LIBS:        ${${PROJECT_NAME}_BUILD_SHARED_LIBS}")
message_c(${BOLD_CYAN} "Build examples:           ${${PROJECT_NAME}_BUILD_EXAMPLES}")
message_c(${BOLD_CYAN} "Build testing:            ${${PROJECT_NAME}_BUILD_TESTING}")
message_c(${BOLD_CYAN} "USE_EXTERNAL_DMPACKET:    ${USE_EXTERNAL_DMPACKET}")
if (EMSCRIPTEN)
    message_c(${BOLD_CYAN} "WebSocket backend:        WASM (EMSCRIPTEN)")
    message_c(${BOLD_CYAN} "CMAKE_TOOLCHAIN_FILE:     ${CMAKE_TOOLCHAIN_FILE}")
else()
    message_c(${BOLD_CYAN} "WebSocket backend:        IXWebSocket")
    message_c(${BOLD_CYAN} "USE_EXTERNAL_IXWEBSOCKET: ${USE_EXTERNAL_IXWEBSOCKET}")
endif()
