#pragma once
#ifndef ARTNET_ARTDMX_H
#define ARTNET_ARTDMX_H

#include "Common.h"
#include <stdint.h>
#include <stddef.h>

namespace art_net {

namespace art_dmx {

using CallbackTypeForUniverse = std::function<void(const uint8_t* data, const uint16_t size)>;
using CallbackTypeForAllPacket = std::function<void(const uint32_t universe, const uint8_t* data, const uint16_t size)>;
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
using CallbackMapForUniverse = std::map<uint32_t, CallbackTypeForUniverse>;
#else
template <uint16_t SIZE>
using CallbackMapForUniverse = arx::map<uint32_t, CallbackTypeForUniverse, NUM_POLLREPLY_PUBLIC_PORT_LIMIT>;
#endif


enum Index : uint16_t {
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

class ArtDmx {
    uint8_t target_net {0};
    uint8_t target_subnet {0};
    uint8_t target_universe {0};
    uint8_t seq {0};
    uint8_t phy {0};

public:
    void set_header(uint8_t *packet)
    {
        for (size_t i = 0; i < ID_LENGTH; i++) packet[i] = static_cast<uint8_t>(ARTNET_ID[i]);
        packet[OP_CODE_L] = (static_cast<uint16_t>(OpCode::Dmx) >> 0) & 0x00FF;
        packet[OP_CODE_H] = (static_cast<uint16_t>(OpCode::Dmx) >> 8) & 0x00FF;
        packet[PROTOCOL_VER_H] = (PROTOCOL_VER >> 8) & 0x00FF;
        packet[PROTOCOL_VER_L] = (PROTOCOL_VER >> 0) & 0x00FF;
        packet[SEQUENCE] = seq++;
        packet[PHYSICAL] = phy;
        packet[NET] = target_net;
        packet[SUBUNI] = (target_subnet << 4) | target_universe;
        packet[LENGTH_H] = (512 >> 8) & 0xFF;
        packet[LENGTH_L] = (512 >> 0) & 0xFF;
    }

    void set_universe(const uint32_t universe_) {
        target_net = (universe_ >> 8) & 0x7F;
        target_subnet = (universe_ >> 4) & 0x0F;
        target_universe = universe_ & 0x0F;
    }
    void set_universe(const uint8_t net_, const uint8_t subnet_, const uint8_t universe_) {
        target_net = net_ & 0x7F;
        target_subnet = subnet_ & 0x0F;
        target_universe = universe_ & 0x0F;
    }
    void set_physical(const uint8_t i) {
        phy = constrain(i, 0, 3);
    }

    uint8_t sequence() const {
        return seq;
    }

    void set_data(uint8_t *packet, const uint8_t* const data, const uint16_t size) {
        memcpy((&packet[art_dmx::DATA]), data, size);
    }
    void set_data(uint8_t *packet, const uint16_t ch, const uint8_t data) {
        packet[art_dmx::DATA + ch] = data;
    }
};

} // namespace art_dmx

} // namespace art_net

#endif // ARTNET_ARTDMX_H
