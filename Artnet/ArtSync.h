#pragma once
#ifndef ARTNET_ART_SYNC_H
#define ARTNET_ART_SYNC_H

#include "Common.h"
#include <stdint.h>
#include <stddef.h>

namespace art_net {
namespace art_sync {

enum Index : uint16_t
{
    ID = 0,
    OP_CODE_L = 8,
    OP_CODE_H = 9,
    PROTOCOL_VER_H = 10,
    PROTOCOL_VER_L = 11,
    AUX1 = 12,
    AUX2 = 13,

    PACKET_SIZE = 14,
};

using CallbackType = std::function<void(const ArtNetRemoteInfo &remote)>;

inline void setMetadataTo(uint8_t *packet)
{
    for (size_t i = 0; i < ID_LENGTH; i++) {
        packet[i] = static_cast<uint8_t>(ARTNET_ID[i]);
    }
    packet[OP_CODE_L] = (static_cast<uint16_t>(OpCode::Sync) >> 0) & 0x00FF;
    packet[OP_CODE_H] = (static_cast<uint16_t>(OpCode::Sync) >> 8) & 0x00FF;
    packet[PROTOCOL_VER_H] = (PROTOCOL_VER >> 8) & 0x00FF;
    packet[PROTOCOL_VER_L] = (PROTOCOL_VER >> 0) & 0x00FF;
    packet[AUX1] = 0;
    packet[AUX2] = 0;
}

} // namespace art_sync
} // namespace art_net

using ArtSyncCallback = art_net::art_sync::CallbackType;

#endif // ARTNET_ART_SYNC_H
