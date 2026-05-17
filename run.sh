#!/bin/bash
# Run script for Service-Oriented IPv4 Protocol simulation
# Usage: ./run.sh <arch> <config>
# Examples:
#   ./run.sh arch1 SoI_Architecture_v3_1
#   ./run.sh arch2
#   ./run.sh arch3_2m
#   ./run.sh all         (run all 13 configurations)

# Set OMNETPP_ROOT to your OMNeT++ installation
OMNETPP_ROOT="${OMNETPP_ROOT:-/home/xct/code/omnetpp-5.6.2}"
export PATH="$OMNETPP_ROOT/bin:$PATH"

run_sim() {
    local arch="$1"
    local ini="$2"
    local config="$3"
    local dir="/home/xct/code/inet4.2.5_${arch}/tutorials/configurator"

    rm -rf "$dir/results"/*
    cd "$dir"
    echo "  [$config]"
    opp_run -u Cmdenv -n ../../src:. -l ../../src/libINET.so \
        -c "$config" "$ini" 2>&1 | grep -E "Simulation time|Error"
}

case "${1:-all}" in
    arch1)
        c="${2:-SoI_Architecture_v3_1}"
        echo "=== architecture_1 (Client-Server SID+CID): $c ==="
        run_sim architecture_1 SoI_Architecture.ini "$c"
        ;;
    arch2)
        echo "=== architecture_2 (Client-Server SID only) ==="
        run_sim architecture_2 SoI_Architecture.ini SoI_Architecture_v3_1
        ;;
    arch3_2m)
        echo "=== architecture_3 (P2P SID+CID, 2 mobile stations) ==="
        run_sim architecture_3 SoI_Architecture_v3_m2m_2m.ini SoI_Architecture_v3_m2m_2m_1
        ;;
    arch3_3m)
        echo "=== architecture_3 (P2P SID+CID, 3 mobile stations) ==="
        run_sim architecture_3 SoI_Architecture_v3_m2m_3m.ini SoI_Architecture_v3_m2m_3_1
        ;;
    arch4_2m)
        echo "=== architecture_4 (P2P SID only, 2 mobile stations) ==="
        run_sim architecture_4 SoI_Architecture_v3_m2m_2m.ini SoI_Architecture_v3_m2m_2m_1
        ;;
    arch4_3m)
        echo "=== architecture_4 (P2P SID only, 3 mobile stations) ==="
        run_sim architecture_4 SoI_Architecture_v3_m2m_3m.ini SoI_Architecture_v3_m2m_3_1
        ;;
    all)
        echo "===== Running all 13 scenarios ====="
        echo ""
        echo "--- architecture_1 (Client-Server SID+CID, 8 configs) ---"
        for i in 1 2 3 4 5 6 7 8; do
            run_sim architecture_1 SoI_Architecture.ini "SoI_Architecture_v3_$i"
        done
        echo ""
        echo "--- architecture_2 (Client-Server SID only) ---"
        run_sim architecture_2 SoI_Architecture.ini SoI_Architecture_v3_1
        echo ""
        echo "--- architecture_3 (P2P SID+CID) ---"
        run_sim architecture_3 SoI_Architecture_v3_m2m_2m.ini SoI_Architecture_v3_m2m_2m_1
        run_sim architecture_3 SoI_Architecture_v3_m2m_3m.ini SoI_Architecture_v3_m2m_3_1
        echo ""
        echo "--- architecture_4 (P2P SID only) ---"
        run_sim architecture_4 SoI_Architecture_v3_m2m_2m.ini SoI_Architecture_v3_m2m_2m_1
        run_sim architecture_4 SoI_Architecture_v3_m2m_3m.ini SoI_Architecture_v3_m2m_3_1
        echo ""
        echo "===== All done ====="
        ;;
    *)
        echo "Usage: $0 <arch> [config]"
        echo ""
        echo "Available architectures:"
        echo "  arch1              Client-Server SID+CID (8 configs)"
        echo "  arch2              Client-Server SID only"
        echo "  arch3_2m           P2P SID+CID, 2 mobile stations"
        echo "  arch3_3m           P2P SID+CID, 3 mobile stations"
        echo "  arch4_2m           P2P SID only, 2 mobile stations"
        echo "  arch4_3m           P2P SID only, 3 mobile stations"
        echo "  all                Run all 13 scenarios"
        echo ""
        echo "Optional config name for arch1: SoI_Architecture_v3_1 .. v3_8"
        exit 1
        ;;
esac
