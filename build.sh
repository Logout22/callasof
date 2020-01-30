#! /bin/sh
set -o errexit

echo "========== FORMAT CODE =========="
find . -type f \( -name '*.c' -o -name '*.h' \) -exec clang-format -i {} \; -print
if [ -n "$(git status --porcelain)" ]; then
    echo "There were errors in the formatting. Aborting build."
    exit 1
fi
echo "============= BUILD ============="
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
echo "============= TEST =============="
valgrind --leak-check=full --error-exitcode=1 test/callasof_test
