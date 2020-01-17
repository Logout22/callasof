add_executable(callasof_test test.c parser_test.c)

set_property(TARGET callasof_test PROPERTY C_STANDARD 11)
target_compile_options(callasof_test PRIVATE -Werror -Wall -Wextra)
target_link_libraries(callasof_test callasof)
