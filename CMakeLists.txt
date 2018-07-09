# Copyright (c) 2018 Daniel L. Robertson
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.5)
project(
    warc-c
    VERSION 0.0.0
    LANGUAGES C CXX
    )

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(TEST_DIR "${PROJECT_SOURCE_DIR}/tests")

include(CheckIncludeFile)

# Additional options

# Enable features
set(ENABLE_STATIC CACHE STRING "Statically link the executable")
set_property(CACHE ENABLE_STATIC PROPERTY STRINGS ON OFF)

# Build extras
set(BUILD_TESTS OFF CACHE STRING "Build tests")
set_property(CACHE BUILD_TESTS PROPERTY STRINGS ON OFF)
set(BUILD_DOCS OFF CACHE STRING "Build documentation")
set_property(CACHE BUILD_DOCS PROPERTY STRINGS ON OFF)

# Basic build

include_directories(${INCLUDE_DIR})

file(GLOB INCLUDE_FILES ${INCLUDE_DIR}/*.h)

file(GLOB LIB_SRCS ${SRC_DIR}/*.c)

# CFLAGS

set(DEFAULT_DEBUG "-g -O0")
set(DEV_WARNINGS "-Wall -Wshadow -Wstrict-overflow -fno-strict-aliasing")
set(CMAKE_C_FLAGS_DEBUG
    "${CMAKE_C_FLAGS_DEBUG} ${DEFAULT_DEBUG} ${DEV_WARNINGS}")
set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} ${DEFAULT_DEBUG} ${DEV_WARNINGS}")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -Wall")

# Add custom build type

# Add new build types
set(CMAKE_C_FLAGS_ASAN
    "${DEFAULT_DEBUG} ${DEV_WARNINGS} -fsanitize=address -fno-omit-frame-pointer"
    CACHE STRING "Flags used by the C compiler during address sanitizer builds."
    FORCE )
set(CMAKE_CXX_FLAGS_ASAN
    "${DEFAULT_DEBUG} ${DEV_WARNINGS} -fsanitize=address -fno-omit-frame-pointer"
    CACHE STRING "Flags used by the C++ compiler during address sanitizer builds."
    FORCE )
set(CMAKE_EXE_LINKER_FLAGS_ASAN
    ""
    CACHE STRING "Flags used for linking binaries during address sanitizer builds."
    FORCE )
set(CMAKE_SHARED_LINKER_FLAGS_ASAN
    ""
    CACHE STRING "Flags used by the shared libraries linker during address
    sanitizer builds."
    FORCE )
mark_as_advanced(
    CMAKE_CXX_FLAGS_ASAN
    CMAKE_C_FLAGS_ASAN
    CMAKE_EXE_LINKER_FLAGS_ASAN
    CMAKE_SHARED_LINKER_FLAGS_ASAN
    )

# Outputs
find_package(FLEX)
FLEX_TARGET(WarcLexer ${SRC_DIR}/warc.lex ${SRC_DIR}/warc.lex.c)
find_package(BISON)
BISON_TARGET(WarcParser ${SRC_DIR}/warc.y ${SRC_DIR}/warc.tab.c)

add_library(${PROJECT_NAME}-shared SHARED ${LIB_SRCS}
    ${BISON_WarcParser_OUTPUTS} ${FLEX_WarcLexer_OUTPUTS})
set_target_properties(${PROJECT_NAME}-shared PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME}-shared PROPERTIES
    SOVERSION ${PROJECT_VERSION_MAJOR})
set_property(TARGET ${PROJECT_NAME}-shared PROPERTY CXX_STANDARD 14)

add_library(${PROJECT_NAME}-static STATIC ${LIB_SRCS}
    ${BISON_WarcParser_OUTPUTS} ${FLEX_WarcLexer_OUTPUTS})
set_target_properties(${PROJECT_NAME}-static PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME})
set_property(TARGET ${PROJECT_NAME}-static PROPERTY CXX_STANDARD 14)

add_executable(${PROJECT_NAME}-bin main.c)
set_target_properties(${PROJECT_NAME}-bin PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}-bin PRIVATE
    ${PROJECT_NAME}-static)

# Lints

add_custom_target(clang-tidy COMMAND clang-tidy ${SRC_DIR}/*.c
    -- -std=c++14 -I${INCLUDE_DIR})

add_custom_target(clang-format COMMAND clang-format -i -style=file
    ${SRC_DIR}/*.c ${TEST_DIR}/*.c ${INCLUDE_DIR}/*.h)

# Docs

if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOC_FILES "")
        foreach(str ${LIB_SRCS})
            set(DOC_FILES "${DOC_FILES} ${str}")
        endforeach(str)
        foreach(str ${INCLUDE_FILES})
            set(DOC_FILES "${DOC_FILES} ${str}")
        endforeach(str)
        configure_file(${PROJECT_SOURCE_DIR}/Doxyfile.in
            ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        add_custom_target(doc
            ${DOXYGEN_EXECUTABLE}
            ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            WORKING DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            )
    else(DOXYGEN_FOUND)
        message(FATAL_ERROR "Must have doxygen install to generate docs")
    endif(DOXYGEN_FOUND)
endif(BUILD_DOCS)

# Tests

if(BUILD_TESTS)
    enable_testing()

    file(GLOB TEST_SRCS ${TEST_DIR}/test_*.cpp)

    add_executable(${PROJECT_NAME}_test ${TEST_DIR}/runner.cpp ${TEST_SRCS} ${EXE_SRCS}
        ${LIB_SRCS})
    set_target_properties(${PROJECT_NAME}_test PROPERTIES LINKER_LANGUAGE CXX)
    target_include_directories(${PROJECT_NAME}_test PRIVATE ${EXE_DIR}/include)

    # From the gtest README.md

    # Download and unpack googletest at configure time
    set(GTEST_VERSION "1.8.0")
    configure_file(CMake/GTest.txt.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
    if(result)
      message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
      RESULT_VARIABLE result
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
    if(result)
      message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    # Prevent overriding the parent project's compiler/linker
    # settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                     ${CMAKE_BINARY_DIR}/googletest-build)

    # Link against gtest
    target_link_libraries(${PROJECT_NAME}_test gtest_main)

    foreach(test ${TEST_SRCS})
        get_filename_component(TEST_NAME ${test} NAME_WE)
        add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test ${TEST_NAME})
    endforeach(test)

    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
    set_property(TARGET ${PROJECT_NAME}_test PROPERTY C_STANDARD 11)
    set_property(TARGET ${PROJECT_NAME}_test PROPERTY CXX_STANDARD 11)
endif(BUILD_TESTS)

# Install
install(
    TARGETS ${PROJECT_NAME}-shared ${PROJECT_NAME}-static
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    )

install(
    FILES ${INCLUDE_FILES}
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME}
    PERMISSIONS ${DEFAULT_READ_PERMISSIONS}
    )
