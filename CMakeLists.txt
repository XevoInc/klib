cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

include(GNUInstallDirs)
include(CMakeDependentOption)

set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)
set(PATCH_VERSION 0)

set(PROJECT_DESCRIPTION "A generic, type-safe C containers library")

project(xlib
        VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}
        DESCRIPTION ${PROJECT_DESCRIPTION}
        LANGUAGES C)

set(CMAKE_C_STANDARD_DEFAULT 11)
add_compile_options(-Wall -Werror -Wextra -Wshadow -Wpedantic -Wshadow)

option(BUILD_XARGPARSE "Build xargparse" ON)
cmake_dependent_option(
    BUILD_XARGPARSE_TESTS
    "Build xargparse tests" ON
    "BUILD_XARGPARSE" OFF
)

set(HDRS
    include/xlib/error.h
    include/xlib/xassert.h
    include/xlib/xhash.h
    include/xlib/xvec.h
    include/xlib/xlog.h
)
set(SRCS src/xlog.c)

if (BUILD_XARGPARSE)
    list(APPEND HDRS include/xlib/xargparse.h)
    list(APPEND SRCS src/xargparse.c)
endif()

add_library(xlib SHARED ${SRCS} ${HDRS})
target_include_directories(xlib PRIVATE ${PROJECT_SOURCE_DIR} include)
set_target_properties(xlib PROPERTIES VERSION ${MAJOR_VERSION}.${MINOR_VERSION})
set_target_properties(xlib PROPERTIES SOVERSION ${MAJOR_VERSION})
install(TARGETS xlib LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if (BUILD_XARGPARSE_TESTS)
    add_executable(xargtest test/test-argparse.c)
    target_include_directories(xargtest PRIVATE ${PROJECT_SOURCE_DIR} include)
    target_link_libraries(xargtest PRIVATE xlib)
endif()

install(FILES ${HDRS} DESTINATION include/xlib)

configure_file(xlib.pc.in ${CMAKE_CURRENT_BINARY_DIR}/xlib.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/xlib.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
