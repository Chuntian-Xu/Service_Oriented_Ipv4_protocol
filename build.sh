#!/bin/bash
# Build script for Service-Oriented IPv4 Protocol simulation
# Usage: ./build.sh [arch1|arch2|arch3|arch4|all]
set -e

# Set OMNETPP_ROOT to your OMNeT++ installation
OMNETPP_ROOT="${OMNETPP_ROOT:-/home/xct/code/omnetpp-5.6.2}"
export PATH="$OMNETPP_ROOT/bin:$PATH"
. "$OMNETPP_ROOT/setenv" -f 2>/dev/null || true

build_arch() {
    local name="$1"
    local dir="/home/xct/code/inet4.2.5_${name}"
    echo "=== Building $name ==="
    cd "$dir"
    make makefiles 2>&1 | tail -1
    make -j$(nproc) 2>&1 | tail -3
    echo "=== $name done ==="
}

case "${1:-all}" in
    arch1) build_arch architecture_1 ;;
    arch2) build_arch architecture_2 ;;
    arch3) build_arch architecture_3 ;;
    arch4) build_arch architecture_4 ;;
    all)
        for a in architecture_1 architecture_2 architecture_3 architecture_4; do
            build_arch "$a"
        done
        ;;
    *)
        echo "Usage: $0 [arch1|arch2|arch3|arch4|all]"
        exit 1
        ;;
esac
echo "All builds complete."
