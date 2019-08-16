cmake_minimum_required (VERSION 3.4)
project (callasof_test)

find_package(PkgConfig REQUIRED)
pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)

add_executable(callasof_test test.c)

target_compile_options(callasof_test PRIVATE -Werror -Wall -Wextra)
target_link_libraries(callasof_test PkgConfig::deps)
