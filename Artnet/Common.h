#pragma once
#ifndef ARTNET_COMMON_H
#define ARTNET_COMMON_H

#include <ArxTypeTraits.h>
#include <ArxContainer.h>
#include <stdint.h>
#include <stddef.h>

namespace art_net {
// Packet Summary : https://art-net.org.uk/structure/packet-summary-2/
// Packet Definition : https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/

enum class OpCode : uint16_t {
    // Device Discovery
    Poll = 0x2000,
    PollReply = 0x2100,
    // Device Configuration
    Address = 0x6000,
    Input = 0x7000,
    IpProg = 0xF800,
    IpProgReply = 0xF900,
    Command = 0x2400,
    // Streaming Control
    Dmx = 0x5000,
    Nzs = 0x5100,
    Sync = 0x5200,
    // RDM
    TodRequest = 0x8000,
    TodData = 0x8100,
    TodControl = 0x8200,
    Rdm = 0x8300,
    RdmSub = 0x8400,
    // Time-Keeping
    TimeCode = 0x9700,
    TimeSync = 0x9800,
    // Triggering
    Trigger = 0x9900,
    // Diagnostics
    DiagData = 0x2300,
    // File Transfer
    FirmwareMaster = 0xF200,
    FirmwareReply = 0xF300,
    Directory = 0x9A00,
    DirectoryReply = 0x9B00,
    FileTnMaster = 0xF400,
    FileFnMaster = 0xF500,
    FileFnReply = 0xF600,
    // Others
    NoPacket = 0x0000,
    Unsupported = 0xFFFE,
    ParseFailed = 0xFFFF,
};

constexpr uint16_t DEFAULT_PORT {6454};  // 0x1936
constexpr uint16_t PROTOCOL_VER {14}; // 0x000E
constexpr uint8_t ID_LENGTH {8};
constexpr char ARTNET_ID[ID_LENGTH] {"Art-Net"};
constexpr float DEFAULT_FPS {40.};
constexpr double DEFAULT_INTERVAL_MS {1000. / (double)DEFAULT_FPS};

// ArtDmx, ArtTrigger has same structure
constexpr uint16_t HEADER_SIZE {18};
constexpr uint16_t PACKET_SIZE {530};

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
template <uint16_t SIZE, typename T = uint8_t>
using Array = std::array<T, SIZE>;
#else
constexpr size_t FIXED_CONTAINER_CAPACITY = 3;
template <uint16_t SIZE, typename T = uint8_t>
using Array = arx::stdx::vector<T, SIZE>;
#endif

struct RemoteInfo
{
    IPAddress ip;
    uint16_t port;
};

struct Destination
{
    String ip;
    uint8_t net;
    uint8_t subnet;
    uint8_t universe;
};


inline bool operator<(const Destination &rhs, const Destination &lhs)
{
    if (rhs.ip < lhs.ip) {
        return true;
    }
    if (rhs.ip > lhs.ip) {
        return false;
    }
    if (rhs.net < lhs.net) {
        return true;
    }
    if (rhs.net > lhs.net) {
        return false;
    }
    if (rhs.subnet < lhs.subnet) {
        return true;
    }
    if (rhs.subnet > lhs.subnet) {
        return false;
    }
    if (rhs.universe < lhs.universe) {
        return true;
    }
    if (rhs.universe > lhs.universe) {
        return false;
    }
    return false;
}

inline bool operator==(const Destination &rhs, const Destination &lhs)
{
    return rhs.ip == lhs.ip && rhs.net == lhs.net && rhs.subnet == lhs.subnet && rhs.universe == lhs.universe;
}

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
// sender
using LastSendTimeMsMap = std::map<Destination, double>;
using SequenceMap = std::map<Destination, uint8_t>;
#else
// sender
using LastSendTimeMsMap = arx::stdx::map<Destination, double, FIXED_CONTAINER_CAPACITY>;
using SequenceMap = arx::stdx::map<Destination, uint8_t, FIXED_CONTAINER_CAPACITY>;
#endif

}  // namespace art_net

using ArtNetRemoteInfo = art_net::RemoteInfo;

#endif  // ARTNET_COMMON_H
