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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(SharemindCxxHeaders CXX)

SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(SHAREMIND_CXXHEADERS_VERSION
    "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

SET(Boost_USE_STATIC_RUNTIME OFF)
FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(SharemindCHeaders REQUIRED)

MACRO(SetList name)
  SET("${name}" "")
  LIST(APPEND "${name}" ${ARGN})
ENDMACRO()

SetList(SHAREMIND_CXXHEADERS_EXTERNAL_LINK_LIBRARIES)
SetList(SHAREMIND_CXXHEADERS_EXTERNAL_INCLUDE_DIRS
    ${Boost_INCLUDE_DIRS}
    ${SharemindCHeaders_INCLUDE_DIRS})
SetList(SHAREMIND_CXXHEADERS_EXTERNAL_DEFINITIONS
    ${SharemindCHeaders_DEFINITIONS})
LIST(REMOVE_DUPLICATES SHAREMIND_CXXHEADERS_EXTERNAL_LINK_LIBRARIES)
LIST(REMOVE_DUPLICATES SHAREMIND_CXXHEADERS_EXTERNAL_INCLUDE_DIRS)
LIST(REMOVE_DUPLICATES SHAREMIND_CXXHEADERS_EXTERNAL_DEFINITIONS)

SetList(SHAREMIND_CXXHEADERS_INSTALL_LINK_LIBRARIES
        ${SHAREMIND_CXXHEADERS_EXTERNAL_LINK_LIBRARIES})
SET(SHAREMIND_CXXHEADERS_INSTALL_INCLUDE_DIRS
    "${CMAKE_INSTALL_PREFIX}/include"
    ${SHAREMIND_CXXHEADERS_EXTERNAL_INCLUDE_DIRS})
SetList(SHAREMIND_CXXHEADERS_INSTALL_DEFINITIONS
        ${SHAREMIND_CXXHEADERS_EXTERNAL_DEFINITIONS})
LIST(REMOVE_DUPLICATES SHAREMIND_CXXHEADERS_INSTALL_INCLUDE_DIRS)

FILE(GLOB_RECURSE SHAREMIND_CXXHEADERS_HEADERS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
ADD_CUSTOM_TARGET("include_SHAREMIND_CXXHEADERS_HEADERS_in_IDE"
                  SOURCES ${SHAREMIND_CXXHEADERS_HEADERS})

INSTALL(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/"
        DESTINATION "include/sharemind")

FILE(GLOB_RECURSE SHAREMIND_CXXHEADERS_INFILES
     "${CMAKE_CURRENT_SOURCE_DIR}/*.in")
ADD_CUSTOM_TARGET("include_SHAREMIND_CXXHEADERS_INFILES_in_IDE"
                  SOURCES ${SHAREMIND_CXXHEADERS_INFILES})

# Tests:
ENABLE_TESTING()
ADD_CUSTOM_TARGET(check COMMAND "${CMAKE_CTEST_COMMAND}")
ADD_COMPILE_OPTIONS(
  "-std=c++11" "-Wall" "-Wextra" "-O2"
  "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-ggdb>"
  "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-fno-omit-frame-pointer>"
  "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DNDEBUG>"
  "$<$<STREQUAL:$<CONFIGURATION>,Release>:-fomit-frame-pointer>"
)
INCLUDE(CheckCXXCompilerFlag)
FUNCTION(CHECK_ADD_COMPILER_FLAG flag)
    STRING(SUBSTRING "${flag}" 1 -1 FlagName)
    STRING(REPLACE "+" "--plus--" TestName "${FlagName}")
    CHECK_CXX_COMPILER_FLAG("${flag}" "${TestName}")
    IF(${TestName})
        ADD_COMPILE_OPTIONS("${flag}")
    ENDIF()
ENDFUNCTION()
CHECK_ADD_COMPILER_FLAG("-Weverything")
CHECK_ADD_COMPILER_FLAG("-Wlogical-op")
CHECK_ADD_COMPILER_FLAG("-Wno-c++98-compat")
CHECK_ADD_COMPILER_FLAG("-Wno-c++98-compat-pedantic")
CHECK_ADD_COMPILER_FLAG("-Wno-float-equal")
CHECK_ADD_COMPILER_FLAG("-Wno-covered-switch-default")
CHECK_ADD_COMPILER_FLAG("-Wno-packed")
CHECK_ADD_COMPILER_FLAG("-Wno-padded")
CHECK_ADD_COMPILER_FLAG("-Wno-weak-vtables")
ADD_DEFINITIONS(${SHAREMIND_CXXHEADERS_EXTERNAL_DEFINITIONS})
INCLUDE_DIRECTORIES(${SHAREMIND_CXXHEADERS_EXTERNAL_INCLUDE_DIRS})

FILE(GLOB_RECURSE SHAREMIND_CXXHEADERS_TESTS
     "${CMAKE_CURRENT_SOURCE_DIR}/tests/Test*.cpp")
LIST(SORT SHAREMIND_CXXHEADERS_TESTS)
ADD_CUSTOM_TARGET("include_SHAREMIND_CXXHEADERS_TESTS_in_IDE"
                  SOURCES ${SHAREMIND_CXXHEADERS_TESTS})
FOREACH(test IN LISTS SHAREMIND_CXXHEADERS_TESTS)
    GET_FILENAME_COMPONENT(testName "${test}" NAME_WE)
    ADD_EXECUTABLE("testImpl_${testName}" EXCLUDE_FROM_ALL "${test}")
    TARGET_LINK_LIBRARIES("testImpl_${testName}" "pthread")
    ADD_DEPENDENCIES(check "testImpl_${testName}")
    ADD_TEST(NAME "test_${testName}" COMMAND "testImpl_${testName}")
ENDFOREACH()

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/SharemindCxxHeadersConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/SharemindCxxHeadersConfig.cmake"
  @ONLY)
INSTALL(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/SharemindCxxHeadersConfig.cmake"
  DESTINATION "lib/SharemindCxxHeaders")
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/SharemindCxxHeadersConfigVersion.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/SharemindCxxHeadersConfigVersion.cmake"
  @ONLY)
INSTALL(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/SharemindCxxHeadersConfigVersion.cmake"
  DESTINATION "lib/SharemindCxxHeaders")


# Packaging:
SET(CPACK_PACKAGE_VENDOR "Cybernetica AS")
SET(CPACK_PACKAGE_CONTACT "Sharemind packaging <sharemind-packaging@cyber.ee>")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ development headers for Sharemind")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_NAME "libsharemind-cxxheaders-dev")
SET(CPACK_DEBIAN_PACKAGE_RELEASE "cyber1")
SET(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libsharemind-cheaders-dev (>= 0.1.0), libboost-dev (>= 1.5.5), libc6-dev (>= 2.19), libstdc++-dev")
SET(CPACK_DEBIAN_PACKAGE_SECTION "libdevel")
SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "any")
SET(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
INCLUDE(CPack)
