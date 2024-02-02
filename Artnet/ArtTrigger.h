#pragma once
#ifndef ARTNET_ART_TRIGGER_H
#define ARTNET_ART_TRIGGER_H

#include "Common.h"
#include <stdint.h>
#include <stddef.h>

namespace art_net {
namespace art_trigger {

enum Index : uint16_t
{
    ID = 0,
    OP_CODE_L = 8,
    OP_CODE_H = 9,
    PROTOCOL_VER_H = 10,
    PROTOCOL_VER_L = 11,
    FILTER_1 = 12,
    FILTER_2 = 13,
    OEM_H = 14,
    OEM_L = 15,
    KEY = 16,
    SUB_KEY = 17,
    PAYLOAD = 18
};

struct Metadata
{
    uint16_t oem;
    uint8_t key;
    uint8_t sub_key;
    const uint8_t *payload;
    uint16_t size;
};

using CallbackType = std::function<void(const Metadata &metadata, const RemoteInfo &remote)>;

inline void setDataTo(uint8_t *packet, uint16_t oem, uint8_t key, uint8_t subkey, const uint8_t* const payload, uint16_t size)
{
    for (size_t i = 0; i < ID_LENGTH; i++) {
        packet[i] = static_cast<uint8_t>(ARTNET_ID[i]);
    }
    packet[OP_CODE_L] = (static_cast<uint16_t>(OpCode::Trigger) >> 0) & 0x00FF;
    packet[OP_CODE_H] = (static_cast<uint16_t>(OpCode::Trigger) >> 8) & 0x00FF;
    packet[PROTOCOL_VER_H] = (PROTOCOL_VER >> 8) & 0x00FF;
    packet[PROTOCOL_VER_L] = (PROTOCOL_VER >> 0) & 0x00FF;
    packet[FILTER_1] = 0;
    packet[FILTER_2] = 0;
    packet[OEM_H] = (oem >> 8) & 0x00FF;
    packet[OEM_L] = (oem >> 0) & 0x00FF;
    packet[KEY] = key;
    packet[SUB_KEY] = subkey;
    if (payload) {
        memcpy(packet + PAYLOAD, payload, size);
    } else {
        memset(packet + PAYLOAD, 0, 512);
    }
}

} // namespace art_trigger
} // namespace art_net

using ArtTriggerCallback = art_net::art_trigger::CallbackType;
using ArtTriggerMetadata = art_net::art_trigger::Metadata;

#endif // ARTNET_ART_TRIGGER_H
