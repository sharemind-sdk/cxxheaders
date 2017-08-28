#
# Copyright (C) 2015 Cybernetica
#
# Research/Commercial License Usage
# Licensees holding a valid Research License or Commercial License
# for the Software may use this file according to the written
# agreement between you and Cybernetica.
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU General Public License version 3.0 requirements will be
# met: http://www.gnu.org/copyleft/gpl-3.0.html.
#
# For further information, please contact us at sharemind@cyber.ee.
#

CMAKE_MINIMUM_REQUIRED(VERSION "3.0.0")
PROJECT(SharemindCxxHeaders VERSION "0.1.0" LANGUAGES "CXX")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers REQUIRED)

SharemindSetProjectVersion()

SET(Boost_USE_STATIC_RUNTIME OFF)
FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(SharemindCHeaders REQUIRED)


SharemindNewList(SharemindCxxHeaders_EXTERNAL_LINK_LIBRARIES)
SharemindNewList(SharemindCxxHeaders_EXTERNAL_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
    ${SharemindCHeaders_INCLUDE_DIRS})
SharemindNewList(SharemindCxxHeaders_EXTERNAL_DEFINITIONS
    ${SharemindCHeaders_DEFINITIONS})
LIST(REMOVE_DUPLICATES SharemindCxxHeaders_EXTERNAL_LINK_LIBRARIES)
LIST(REMOVE_DUPLICATES SharemindCxxHeaders_EXTERNAL_INCLUDE_DIRS)
LIST(REMOVE_DUPLICATES SharemindCxxHeaders_EXTERNAL_DEFINITIONS)

SharemindNewList(SharemindCxxHeaders_INSTALL_LINK_LIBRARIES
        ${SharemindCxxHeaders_EXTERNAL_LINK_LIBRARIES})
SET(SharemindCxxHeaders_INSTALL_INCLUDE_DIRS
    "${CMAKE_INSTALL_PREFIX}/include"
    ${SharemindCxxHeaders_EXTERNAL_INCLUDE_DIRS})
SharemindNewList(SharemindCxxHeaders_INSTALL_DEFINITIONS
        ${SharemindCxxHeaders_EXTERNAL_DEFINITIONS})
LIST(REMOVE_DUPLICATES SharemindCxxHeaders_INSTALL_INCLUDE_DIRS)

FILE(GLOB_RECURSE SharemindCxxHeaders_HEADERS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
ADD_CUSTOM_TARGET("include_SharemindCxxHeaders_HEADERS_in_IDE"
                  SOURCES ${SharemindCxxHeaders_HEADERS})

INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/"
        DESTINATION "include/sharemind"
        COMPONENT "dev")


# Tests:
SharemindSetCxx11CompileOptions()
SharemindAddSimpleTests("${CMAKE_CURRENT_SOURCE_DIR}/tests/Test*.cpp"
    LEGACY_DEFINITIONS
        ${SharemindCxxHeaders_EXTERNAL_DEFINITIONS}
    INCLUDE_DIRECTORIES
        ${SharemindCxxHeaders_EXTERNAL_INCLUDE_DIRS}
    LINK_LIBRARIES
        "pthread"
)

# Install CMake files:
SharemindUseCMakeFindFiles()


# Packaging:
SharemindSetupPackaging()
SharemindAddComponentPackage("dev"
    NAME "libsharemind-cxxheaders-dev"
    DESCRIPTION "C++ development headers for Sharemind"
    DEB_SECTION "libdevel"
    DEB_DEPENDS
        "libsharemind-cheaders-dev (>= 0.1.0)"
        "libboost-dev (>= 1.5.5)"
        "libc6-dev (>= 2.19)"
        "libstdc++-dev"
)
SharemindPackagingFinalize()
