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
PROJECT(SharemindCxxHeaders VERSION "0.8.0" LANGUAGES "CXX")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers 1.6 REQUIRED)

FIND_PACKAGE(Boost 1.62 REQUIRED)
FIND_PACKAGE(SharemindCHeaders 1.3.0 REQUIRED)
FIND_PACKAGE(Threads REQUIRED)


# CxxHeaders:
FILE(GLOB_RECURSE SharemindCxxHeaders_HEADERS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
SharemindAddInterfaceLibrary(CxxHeaders
    EXPOSE_FILES ${SharemindCxxHeaders_HEADERS})
TARGET_INCLUDE_DIRECTORIES(CxxHeaders
    INTERFACE
        # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src> # TODO
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
    )
TARGET_LINK_LIBRARIES(CxxHeaders
    INTERFACE
        atomic
        Boost::boost
        Sharemind::CHeaders
        Threads::Threads
    )
INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/"
        DESTINATION "include/sharemind"
        COMPONENT "dev")
SharemindCreateCMakeFindFilesForTarget(CxxHeaders
    DEPENDENCIES
        "Boost 1.62"
        "SharemindCHeaders 1.3.0"
        Threads
    )


# Tests:
SharemindSetCxx14CompileOptions()
FILE(GLOB CxxHeaders_TESTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test*.cpp")
FOREACH(testFile IN LISTS CxxHeaders_TESTS)
    GET_FILENAME_COMPONENT(testName "${testFile}" NAME_WE)
    SharemindAddTest("${testName}" SOURCES "${testFile}")
    TARGET_LINK_LIBRARIES("${testName}" PRIVATE CxxHeaders)
ENDFOREACH()


# Packaging:
SharemindSetupPackaging()
SharemindAddComponentPackage("dev"
    NAME "libsharemind-cxxheaders-dev"
    DESCRIPTION "C++ development headers for Sharemind"
    DEB_SECTION "libdevel"
    DEB_DEPENDS
        "libsharemind-cheaders-dev (>= 1.3.0)"
        "libboost-dev (>= 1.62)"
        "libc6-dev (>= 2.19)"
        "libstdc++-dev"
)
SharemindPackagingFinalize()
