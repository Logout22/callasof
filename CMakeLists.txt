cmake_minimum_required (VERSION 3.4)
project (callasof)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)

add_library(callasof
    src/callasof.c)
set_property(TARGET callasof PROPERTY C_STANDARD 11)
target_compile_options(callasof PRIVATE -Werror -Wall -Wextra)
target_include_directories(callasof PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(callasof PkgConfig::deps)

add_subdirectory(test)
