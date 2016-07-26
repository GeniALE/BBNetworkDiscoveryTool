#
# CMakeLists.txt
#
# Copyright (C) 2016 par GeniALE.
# Marc-Andre Guimond <guimond.marcandre@gmail.com>.
#
# Ce fichier est encode en UTF-8.

# TODO: CMake on BBB is 2.8.9 (should be 2.8.12).
cmake_minimum_required (VERSION 2.8.9)

project (BBNetworkDiscoveryTool)

# Set binary name.
set (VersionMaj "1")
set (VersionMin "0")
set (VersionAPI "1")
set (BINARY_NAME "bbnettool")
#FIXME: Stack overflow in release.
set (CMAKE_BUILD_TYPE "Debug")

# Set project paths.
set (REPOS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set (COMMON_RELATIVE_PATH ../Commun)

# Include compiler options.
#TODO: -std issues with BLE libs. When lib/CACClient included as lib, can remove.
include (${REPOS_ROOT}/Commun/CMake/CMakeOptions.cmake)

# Source file paths relative to source directory.
set ( BBCONTROLLER_SOURCE_FILES
      main.cpp
      ${COMMON_RELATIVE_PATH}/lib/CACClient/CACClient.cpp
      ${COMMON_RELATIVE_PATH}/lib/NetworkClient/UDPClient.cpp
      ${COMMON_RELATIVE_PATH}/lib/NetworkClient/HTTPClient.cpp
      ${COMMON_RELATIVE_PATH}/Code/version.c
      ${COMMON_RELATIVE_PATH}/Code/uString/uString.c
      ${COMMON_RELATIVE_PATH}/Code/bbb/utils.cpp )

include_directories ( .
                      ${COMMON_RELATIVE_PATH}/lib/CACClient
                      ${COMMON_RELATIVE_PATH}/lib/NetworkClient
                      ${COMMON_RELATIVE_PATH}/Code
                      ${COMMON_RELATIVE_PATH}/Code/uString
                      ${COMMON_RELATIVE_PATH}/Code/bbb )

add_executable ( ${BINARY_NAME}
                 ${BBCONTROLLER_SOURCE_FILES} )


