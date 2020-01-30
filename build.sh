#! /bin/sh
set -o errexit

echo "========== FORMAT CODE =========="
find . -type f \( -name '*.c' -o -name '*.h' \) -exec clang-format -i {} \; -print
echo "============= BUILD ============="
mkdir -p build
cd build
cmake ..
make
echo "============= TEST =============="
valgrind --leak-check=full test/callasof_test
