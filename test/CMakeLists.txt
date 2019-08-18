add_executable(callasof_test test.c)

target_compile_options(callasof_test PRIVATE -Werror -Wall -Wextra)
target_link_libraries(callasof_test callasof)
