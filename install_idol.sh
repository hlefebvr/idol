#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(pwd)"
if command -v nproc >/dev/null 2>&1; then
    NPROC=$(nproc)
elif sysctl -n hw.ncpu >/dev/null 2>&1; then
    NPROC=$(sysctl -n hw.ncpu)
else
    NPROC=1
fi

echo "============================================================"
echo " Building MibS (idol custom version)"
echo "============================================================"

mkdir -p coin-or
(
    cd coin-or

    wget https://raw.githubusercontent.com/coin-or/coinbrew/master/coinbrew
    chmod +x coinbrew

    ./coinbrew fetch https://github.com/hlefebvr/MibS@idol

    # Replace deprecated std::unary_function usage.
    sed -i '' \
        's/std::unary_function/std::__unary_function/g' \
        Alps/Alps/src/AlpsHelperFunctions.h || echo "Skipping replacing std::__unary_function."

    ./coinbrew build \
        https://github.com/hlefebvr/MibS@idol \
        --no-prompt \
        --disable-cplex-libcheck \
        --tests=none
)

echo "============================================================"
echo " Cloning idol"
echo "============================================================"

git clone https://github.com/hlefebvr/idol.git

echo "============================================================"
echo " Building idol"
echo "============================================================"

cd idol

mkdir -p build
(
    cd build

    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=../dist \
        -DCOIN_OR_DIR="${ROOT_DIR}/coin-or/dist" \
        -DUSE_MIBS=YES
#       -DCMAKE_C_COMPILER=/Library/Developer/CommandLineTools/usr/bin/cc \
#       -DCMAKE_CXX_COMPILER=/Library/Developer/CommandLineTools/usr/bin/c++

    make -j"${NPROC}"
    make install
)

echo "============================================================"
echo " idol successfully installed"
echo "============================================================"
echo
echo "Installation directory:"
echo "  ${ROOT_DIR}/idol/dist"