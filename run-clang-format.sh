#!/bin/sh
# Please use clang-format version >= 9
find -type f \( -name '*.c' -o -name '*.h' \) -exec clang-format -i {} \;
