SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# FNR-tree (2D R-tree + 1D R-tree)
g++ -std=c++11 "$SCRIPT_DIR"/../src/FNRTest.cpp -o "$SCRIPT_DIR"/../fnrtest.out

# 2D R-tree + Interval-tree
g++ -std=c++11 "$SCRIPT_DIR"/../src/XTest.cpp -o "$SCRIPT_DIR"/../xtest.out

# 2D R-tree + Compressed (EN DESARROLLO..)
g++ -std=c++11 -O3 -DNDEBUG -I ~/include -L ~/lib "$SCRIPT_DIR"/../src/CTest.cpp -o "$SCRIPT_DIR"/../ctest.out -lsdsl -ldivsufsort -ldivsufsort64