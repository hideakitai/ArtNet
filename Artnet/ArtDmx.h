#pragma once
#ifndef ARTNET_ARTDMX_H
#define ARTNET_ARTDMX_H

#include "Common.h"
#include <stdint.h>
#include <stddef.h>

namespace art_net {
namespace art_dmx {

enum Index : uint16_t
{
    ID = 0,
    OP_CODE_L = 8,
    OP_CODE_H = 9,
    PROTOCOL_VER_H = 10,
    PROTOCOL_VER_L = 11,
    SEQUENCE = 12,
    PHYSICAL = 13,
    SUBUNI = 14,
    NET = 15,
    LENGTH_H = 16,
    LENGTH_L = 17,
    DATA = 18
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

struct Metadata
{
    uint8_t sequence;
    uint8_t physical;
    uint8_t net;
    uint8_t subnet;
    uint8_t universe;
};

using CallbackType = std::function<void(const uint8_t *data, uint16_t size, const Metadata &metadata, const RemoteInfo &remote)>;
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
// sender
using LastSendTimeMsMap = std::map<Destination, uint32_t>;
using SequenceMap = std::map<Destination, uint8_t>;
// receiver
using CallbackMap = std::map<uint16_t, CallbackType>;
#else
// sender
using LastSendTimeMsMap = arx::map<Destination, uint32_t>;
using SequenceMap = arx::map<Destination, uint8_t>;
// receiver
using CallbackMap = arx::map<uint16_t, CallbackType>;
#endif

inline Metadata generateMetadataFrom(const uint8_t *packet)
{
    Metadata metadata;
    metadata.sequence = packet[SEQUENCE];
    metadata.physical = packet[PHYSICAL];
    metadata.net = packet[NET];
    metadata.subnet = (packet[SUBUNI] >> 4) & 0x0F;
    metadata.universe = (packet[SUBUNI] >> 0) & 0x0F;
    return metadata;
}

inline void setMetadataTo(uint8_t *packet, uint8_t sequence, uint8_t physical, uint8_t net, uint8_t subnet, uint8_t universe)
{
    for (size_t i = 0; i < ID_LENGTH; i++) {
        packet[i] = static_cast<uint8_t>(ARTNET_ID[i]);
    }
    packet[OP_CODE_L] = (static_cast<uint16_t>(OpCode::Dmx) >> 0) & 0x00FF;
    packet[OP_CODE_H] = (static_cast<uint16_t>(OpCode::Dmx) >> 8) & 0x00FF;
    packet[PROTOCOL_VER_H] = (PROTOCOL_VER >> 8) & 0x00FF;
    packet[PROTOCOL_VER_L] = (PROTOCOL_VER >> 0) & 0x00FF;
    packet[SEQUENCE] = sequence;
    packet[PHYSICAL] = physical & 0x03;
    packet[NET] = net & 0x7F;
    packet[SUBUNI] = ((subnet & 0x0F) << 4) | (universe & 0x0F);
    packet[LENGTH_H] = (512 >> 8) & 0xFF;
    packet[LENGTH_L] = (512 >> 0) & 0xFF;
}

inline void setDataTo(uint8_t *packet, const uint8_t* const data, uint16_t size)
{
    memcpy(packet + art_dmx::DATA, data, size);
}
inline void setDataTo(uint8_t *packet, const uint16_t ch, const uint8_t data)
{
    packet[art_dmx::DATA + ch] = data;
}

} // namespace art_dmx
} // namespace art_net

using ArtDmxMetadata = art_net::art_dmx::Metadata;
using ArtDmxCallback = art_net::art_dmx::CallbackType;

#endif // ARTNET_ARTDMX_H
