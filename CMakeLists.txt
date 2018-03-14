#
# Copyright (C) Cybernetica
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
PROJECT(SharemindCxxHeaders VERSION "0.2.0" LANGUAGES "CXX")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers REQUIRED)

SharemindSetProjectVersion()

SET(Boost_USE_STATIC_RUNTIME OFF)
FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(SharemindCHeaders REQUIRED)


SharemindNewUniqueList(SharemindCxxHeaders_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
    ${SharemindCHeaders_INCLUDE_DIRS}
)
SharemindNewUniqueList(SharemindCxxHeaders_DEFINITIONS
    ${SharemindCHeaders_DEFINITIONS}
)
SharemindNewUniqueList(SharemindCxxHeaders_LINK_LIBRARIES
    ${SharemindCHeaders_LIBRARIES}
    "-latomic"
)

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
        ${SharemindCxxHeaders_DEFINITIONS}
    INCLUDE_DIRECTORIES
        ${SharemindCxxHeaders_INCLUDE_DIRS}
    LINK_LIBRARIES
        ${SharemindCxxHeaders_LINK_LIBRARIES}

        # The following is a workaround for multithreaded test failures with GCC
        # 4.9 and older. The error message is something like:
        #
        #   terminate called after throwing an instance of 'std::system_error'
        #     what():  Enable multithreading to use std::thread: Operation not
        #                                                        permitted
        #   Aborted
        #
        -pthread
)

# Install CMake files:
SharemindCreateCMakeFindFiles(
    INCLUDE_DIRS
        "${CMAKE_INSTALL_PREFIX}/include"
        ${SharemindCxxHeaders_INCLUDE_DIRS}
    DEFINITIONS
        ${SharemindCxxHeaders_DEFINITIONS}
    LIBRARIES
        ${SharemindCxxHeaders_LINK_LIBRARIES}
)


# Packaging:
SharemindSetupPackaging()
SharemindAddComponentPackage("dev"
    NAME "libsharemind-cxxheaders-dev"
    DESCRIPTION "C++ development headers for Sharemind"
    DEB_SECTION "libdevel"
    DEB_DEPENDS
        "libsharemind-cheaders-dev (>= 0.1.0)"
        "libboost-dev (>= 1.55)"
        "libc6-dev (>= 2.19)"
        "libstdc++-dev"
)
SharemindPackagingFinalize()
