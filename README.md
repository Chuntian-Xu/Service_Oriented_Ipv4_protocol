# Service-Oriented IPv4 Protocol for Maritime IoT MTC

Simulation code for the thesis: *Research and Simulation of Service-Oriented Machine Type Communication Network for Maritime IoT*, Southeast University, 2022.

## Architectures

Four self-contained INET 4.2.5 source overlays, each implementing a different solution:

| Directory | Thesis | Scenario | Service Header | Size |
|-----------|--------|----------|----------------|------|
| `architecture_1` | §3.2.1 | Client-Server | SID + CID | 3 bytes |
| `architecture_2` | §3.2.2 | Client-Server | SID only | 2 bytes |
| `architecture_3` | §3.3.1 | Peer-to-Peer | SID + CID | 3 bytes |
| `architecture_4` | §3.3.2 | Peer-to-Peer | SID only | 2 bytes |

Key idea: replace IP-address-based routing with service-identifier-based (SID/CID) routing on the bandwidth-constrained maritime VHF air interface, reducing header overhead from 28 bytes (IPv4+UDP) to 2-3 bytes.

## Verified Environment

The following environment has been fully tested — all 13 simulation scenarios pass.

| Component | Version | Notes |
|-----------|---------|-------|
| OS | Ubuntu 22.04.5 LTS (WSL2) | Kernel 6.6.114.1-microsoft-standard-WSL2 |
| OMNeT++ | 5.6.2 | Built from source: `PREFER_CLANG=no`, `WITH_OSGEARTH=no` |
| INET | 4.2.5 | Modified per architecture |
| GCC | 11.4.0 | Ubuntu 11.4.0-1ubuntu1~22.04.3 |
| OpenJDK | 11.0.30 | Required for OMNeT++ IDE only |
| CPU / RAM | 16 cores / 7.8 GB | WSL2 defaults |
| IDE | OMNeT++ Eclipse | Requires `dmz-cursor-theme` on WSLg |

**Original author's environment (from README):**
- Windows 10 64-bit / Ubuntu 18.04.6 LTS
- OMNeT++ 5.6.2, INET 4.2.5

## Quick Start

### 1. Install OMNeT++

```bash
git clone --depth 1 --branch omnetpp-5.6.2 https://github.com/omnetpp/omnetpp.git
cd omnetpp-5.6.2
cp configure.user.dist configure.user
# Edit configure.user: PREFER_CLANG=no, WITH_OSGEARTH=no
./configure && make -j$(nproc)
```

### 2. Install INET and overlay architectures

```bash
git clone --depth 1 --branch v4.2.5 https://github.com/inet-framework/inet.git inet4.2.5
cd inet4.2.5 && . /path/to/omnetpp/setenv -f && make makefiles && make -j$(nproc)

# For each architecture, copy INET and overlay modified source files
for arch in architecture_1 architecture_2 architecture_3 architecture_4; do
    cp -r inet4.2.5 inet4.2.5_${arch}
    cp -r Service_Oriented_Ipv4_protocol/${arch}/src/inet/* inet4.2.5_${arch}/src/
    cp -r Service_Oriented_Ipv4_protocol/${arch}/tutorials/* inet4.2.5_${arch}/tutorials/
done
```

### 3. Build

```bash
# Set OMNeT++ path (or export OMNETPP_ROOT)
export OMNETPP_ROOT=/path/to/omnetpp-5.6.2

# Build all architectures
./build.sh all

# Or build a single one
./build.sh arch1
```

### 4. Run Simulations

```bash
# Run all 13 simulation scenarios
./run.sh all

# Run specific scenarios
./run.sh arch1                 # Client-Server SID+CID, config v3_1
./run.sh arch1 SoI_Architecture_v3_2   # specific config (v3_1 ~ v3_8)
./run.sh arch2                 # Client-Server SID only
./run.sh arch3_2m              # P2P SID+CID, 2 mobile stations
./run.sh arch3_3m              # P2P SID+CID, 3 mobile stations
./run.sh arch4_2m              # P2P SID only, 2 mobile stations
./run.sh arch4_3m              # P2P SID only, 3 mobile stations
```

### 5. View Results

```bash
# Scalar statistics (packet sizes, counts)
scavetool q -p results/*.sca

# Export to CSV
scavetool x -F CSV-S -o output.csv results/*.sca

# Event log (message flow trace)
eventlogtool echo results/*.elog | less
```

Key metrics for header load analysis:
- Wireless interface: `wmtc[0].mac.rcvdPkFromHl` — service-oriented PDU size
- Wired interface: `eth[0].encap.decapPk` — traditional IPv4+UDP PDU size

Typical result: ~13 bytes/pkt (wireless, service header) vs ~46 bytes/pkt (wired, IP header).

### 6. OMNeT++ IDE

```bash
omnetpp -data /path/to/workspace &

# IDE requires the pre-built Eclipse bundle (not in git).
# Download omnetpp-5.6.2-src-linux.tgz and extract the ide/ directory.
```

After importing the INET project:
- Right-click `.ini` -> Run As -> OMNeT++ Simulation
- Right-click `.elog` -> Sequence Chart (thesis §5.3, message flow analysis)
- Right-click `.sca` -> Analysis Tool (thesis §5.4, header load analysis)

## Code Modifications from INET 4.2.5

Each architecture modifies these INET components:

- `src/inet/networklayer/ipv4/` — `Ipv4_MobileStation`, `Ipv4_ShoreStation`, custom `Ipv4NetworkLayer` variants, service header messages (`Ipv4ServiceHeader`), SID/CID tables
- `src/inet/networklayer/configurator/` — Extended configurator to parse `<sid>`, `<cid>`, `<cidshore>` XML elements
- `src/inet/networklayer/contract/` — New interfaces: `ISid`, `ICid`, `ISidTable`, `ICidTable`
- `src/inet/applications/udpapp/` — Custom `UdpApp`, `UdpApp_1` with broadcast receive support
- `src/inet/node/` — Custom node types with service-oriented network layers

## Limitations

- CID is 8-bit (max 256 clients per local network)
- SID-only solutions rely on local-network broadcast for reply delivery; broadcast overhead is not quantified
- Physical layer uses simplified `UnitDiskRadio` — no actual VHF channel modeling
- `architecture_2` was originally only tested on Ubuntu 18.04 LTS

## Author

Chuntian Xu, Southeast University, Nanjing, China — chuntianxu2020@163.com
