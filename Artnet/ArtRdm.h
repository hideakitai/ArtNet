#pragma once
#ifndef ARTNET_ARTRDM_H
#define ARTNET_ARTRDM_H

#include "Common.h"
#include <stdint.h>
#include <stddef.h>

namespace art_net {
namespace art_rdm {

enum Index : uint16_t {
    ID = 0,
    OP_CODE_L = 8,
    OP_CODE_H = 9,
    PROTOCOL_VER_H = 10,
    PROTOCOL_VER_L = 11,
    RDM_VER = 12,
    FILLER_2 = 13,
    SPARE_0 = 14,
    SPARE_1 = 15,
    SPARE_2 = 16,
    SPARE_3 = 17,
    SPARE_4 = 18,
    SPARE_5 = 19,
    SPARE_6 = 20,
    NET = 21,
    COMMAND = 22,
    ADDRESS_H = 23,
    ADDRESS_L = 24,
    RDM_PACKET = 25
};

constexpr uint16_t SIZE = 25;
constexpr uint16_t RDM_PACKET_SIZE = 231;

struct Metadata {
    uint8_t rdm_ver;
    uint8_t net;
    uint8_t command;
    uint16_t address;
};

using CallbackType = std::function<void(const uint8_t*, const uint16_t, const Metadata&, const RemoteInfo&)>;

inline Metadata generateMetadataFromOpRdm(const uint8_t* packet) {
    Metadata metadata;
    metadata.rdm_ver = packet[RDM_VER];
    metadata.net = packet[NET];
    metadata.command = packet[COMMAND];
    metadata.address = (packet[ADDRESS_H] << 8) | packet[ADDRESS_L];
    return metadata;
}

inline void setMetadataToOpRdm(uint8_t* packet, const Metadata& metadata) {
    for (size_t i = 0; i < ID_LENGTH; ++i) packet[i] = static_cast<uint8_t>(ARTNET_ID[i]);
    packet[OP_CODE_L] = (static_cast<uint16_t>(OpCode::Rdm) >> 0) & 0x00FF;
    packet[OP_CODE_H] = (static_cast<uint16_t>(OpCode::Rdm) >> 8) & 0x00FF;
    packet[PROTOCOL_VER_H] = (PROTOCOL_VER >> 8) & 0x00FF;
    packet[PROTOCOL_VER_L] = (PROTOCOL_VER >> 0) & 0x00FF;
    packet[RDM_VER] = metadata.rdm_ver;
    packet[FILLER_2] = 0;
    for (size_t i = SPARE_0; i <= SPARE_6; ++i) packet[i] = 0;
    packet[NET] = metadata.net;
    packet[COMMAND] = metadata.command;
    packet[ADDRESS_H] = (metadata.address >> 8) & 0x00FF;
    packet[ADDRESS_L] = (metadata.address >> 0) & 0x00FF;
}

inline void setRdmDataToOpRdm(uint8_t* packet, const uint8_t* data, const uint16_t size) {
    if (size > RDM_PACKET_SIZE) return;
    memcpy(packet + RDM_PACKET, data, size);
}

}  // namespace art_rdm

namespace art_rdm_sub {

enum Index : uint16_t {
    ID = 0,
    OP_CODE_L = 8,
    OP_CODE_H = 9,
    PROTOCOL_VER_H = 10,
    PROTOCOL_VER_L = 11,
    RDM_VER = 12,
    FILLER_2 = 13,
    UID_0 = 14,
    UID_1 = 15,
    UID_2 = 16,
    UID_3 = 17,
    UID_4 = 18,
    UID_5 = 19,
    SPARE_0 = 20,
    COMMAND_CLASS = 21,
    PARAMETER_ID_H = 22,
    PARAMETER_ID_L = 23,
    SUB_DEVICE_H = 24,
    SUB_DEVICE_L = 25,
    SUB_COUNT_H = 26,
    SUB_COUNT_L = 27,
    BLOCK_0 = 28,
    BLOCK_1 = 29,
    BLOCK_2 = 30,
    BLOCK_3 = 31,
    RDM_SUB_DATA = 32
};

constexpr uint16_t SIZE = 32;
constexpr uint16_t RDM_SUB_DATA_SIZE = 480;

struct Metadata {
    uint8_t rdm_ver;
    uint8_t uid[6];
    uint8_t command_class;
    uint16_t parameter_id;
    uint16_t sub_device;
    uint16_t sub_count;
    uint8_t block[4];
};

using CallbackType = std::function<void(const uint8_t*, const uint16_t, const Metadata&, const RemoteInfo&)>;

inline Metadata generateMetadataFromOpRdmSub(const uint8_t* packet) {
    Metadata metadata;
    metadata.rdm_ver = packet[RDM_VER];
    for (size_t i = 0; i < 6; ++i) {
        metadata.uid[i] = packet[UID_0 + i];
    }
    metadata.command_class = packet[COMMAND_CLASS];
    metadata.parameter_id = (packet[PARAMETER_ID_H] << 8) | packet[PARAMETER_ID_L];
    metadata.sub_device = (packet[SUB_DEVICE_H] << 8) | packet[SUB_DEVICE_L];
    metadata.sub_count = (packet[SUB_COUNT_H] << 8) | packet[SUB_COUNT_L];
    for (size_t i = 0; i < 4; ++i) {
        metadata.block[i] = packet[BLOCK_0 + i];
    }
    return metadata;
}

inline void setMetadataToOpRdmSub(uint8_t* packet, const Metadata& metadata) {
    for (size_t i = 0; i < ID_LENGTH; ++i) packet[i] = static_cast<uint8_t>(ARTNET_ID[i]);
    packet[OP_CODE_L] = (static_cast<uint16_t>(OpCode::RdmSub) >> 0) & 0x00FF;
    packet[OP_CODE_H] = (static_cast<uint16_t>(OpCode::RdmSub) >> 8) & 0x00FF;
    packet[PROTOCOL_VER_H] = (PROTOCOL_VER >> 8) & 0x00FF;
    packet[PROTOCOL_VER_L] = (PROTOCOL_VER >> 0) & 0x00FF;
    packet[RDM_VER] = metadata.rdm_ver;
    packet[FILLER_2] = 0;
    for (size_t i = 0; i < 6; ++i) {
        packet[UID_0 + i] = metadata.uid[i];
    }
    packet[SPARE_0] = 0;
    packet[COMMAND_CLASS] = metadata.command_class;
    packet[PARAMETER_ID_H] = (metadata.parameter_id >> 8) & 0x00FF;
    packet[PARAMETER_ID_L] = (metadata.parameter_id >> 0) & 0x00FF;
    packet[SUB_DEVICE_H] = (metadata.sub_device >> 8) & 0x00FF;
    packet[SUB_DEVICE_L] = (metadata.sub_device >> 0) & 0x00FF;
    packet[SUB_COUNT_H] = (metadata.sub_count >> 8) & 0x00FF;
    packet[SUB_COUNT_L] = (metadata.sub_count >> 0) & 0x00FF;
    for (size_t i = 0; i < 4; ++i) {
        packet[BLOCK_0 + i] = metadata.block[i];
    }
}

inline void setRdmSubDataToOpRdmSub(uint8_t* packet, const uint8_t* data, const uint16_t size) {
    if (size > RDM_SUB_DATA_SIZE) return;
    memcpy(packet + RDM_SUB_DATA, data, size);
}

}  // namespace art_rdm_sub

namespace art_tod_request {

enum Index : uint16_t {
    ID = 0,
    OP_CODE_L = 8,
    OP_CODE_H = 9,
    PROTOCOL_VER_H = 10,
    PROTOCOL_VER_L = 11,
    FILLER_1 = 12,
    FILLER_2 = 13,
    SPARE_0 = 14,
    SPARE_1 = 15,
    SPARE_2 = 16,
    SPARE_3 = 17,
    SPARE_4 = 18,
    SPARE_5 = 19,
    SPARE_6 = 20,
    NET = 21,
    COMMAND = 22,
    ADD_COUNT = 23,
    ADDRESS = 24
};

constexpr uint16_t SIZE = 24;
constexpr uint16_t MAX_ADDRESSES = 32;

struct Metadata {
    uint8_t net;
    uint8_t command;
    uint8_t add_count;
};

using CallbackType = std::function<void(const uint16_t*, const uint8_t, const Metadata&, const RemoteInfo&)>;

inline Metadata generateMetadataFromOpTodRequest(const uint8_t* packet) {
    Metadata metadata;
    metadata.net = packet[NET];
    metadata.command = packet[COMMAND];
    metadata.add_count = packet[ADD_COUNT];
    return metadata;
}

inline void setMetadataToOpTodRequest(uint8_t* packet, const Metadata& metadata) {
    for (size_t i = 0; i < ID_LENGTH; ++i) packet[i] = static_cast<uint8_t>(ARTNET_ID[i]);
    packet[OP_CODE_L] = (static_cast<uint16_t>(OpCode::TodRequest) >> 0) & 0x00FF;
    packet[OP_CODE_H] = (static_cast<uint16_t>(OpCode::TodRequest) >> 8) & 0x00FF;
    packet[PROTOCOL_VER_H] = (PROTOCOL_VER >> 8) & 0x00FF;
    packet[PROTOCOL_VER_L] = (PROTOCOL_VER >> 0) & 0x00FF;
    packet[FILLER_1] = 0;
    packet[FILLER_2] = 0;
    for (size_t i = SPARE_0; i <= SPARE_6; ++i) packet[i] = 0;
    packet[NET] = metadata.net;
    packet[COMMAND] = metadata.command;
    packet[ADD_COUNT] = metadata.add_count;
}

inline void setAddressesToOpTodRequest(uint8_t* packet, const uint16_t* addresses, const uint8_t count) {
    if (count > MAX_ADDRESSES) return;
    for (uint8_t i = 0; i < count; ++i) {
        packet[ADDRESS + i * 2] = (addresses[i] >> 8) & 0x00FF;
        packet[ADDRESS + i * 2 + 1] = (addresses[i] >> 0) & 0x00FF;
    }
}

}  // namespace art_tod_request

namespace art_tod_data {

enum Index : uint16_t {
    ID = 0,
    OP_CODE_L = 8,
    OP_CODE_H = 9,
    PROTOCOL_VER_H = 10,
    PROTOCOL_VER_L = 11,
    RDM_VER = 12,
    PORT = 13,
    SPARE_0 = 14,
    SPARE_1 = 15,
    SPARE_2 = 16,
    SPARE_3 = 17,
    SPARE_4 = 18,
    SPARE_5 = 19,
    SPARE_6 = 20,
    NET = 21,
    COMMAND_RESPONSE = 22,
    ADDRESS_H = 23,
    ADDRESS_L = 24,
    UID_TOTAL_H = 25,
    UID_TOTAL_L = 26,
    BLOCK_COUNT = 27,
    UID_COUNT = 28,
    TOD = 29
};

constexpr uint16_t SIZE = 29;
constexpr uint16_t MAX_UIDS = 200;

struct Metadata {
    uint8_t rdm_ver;
    uint8_t port;
    uint8_t net;
    uint8_t command_response;
    uint16_t address;
    uint16_t uid_total;
    uint8_t block_count;
    uint8_t uid_count;
};

using CallbackType = std::function<void(const uint8_t*, const uint8_t, const Metadata&, const RemoteInfo&)>;

inline Metadata generateMetadataFromOpTodData(const uint8_t* packet) {
    Metadata metadata;
    metadata.rdm_ver = packet[RDM_VER];
    metadata.port = packet[PORT];
    metadata.net = packet[NET];
    metadata.command_response = packet[COMMAND_RESPONSE];
    metadata.address = (packet[ADDRESS_H] << 8) | packet[ADDRESS_L];
    metadata.uid_total = (packet[UID_TOTAL_H] << 8) | packet[UID_TOTAL_L];
    metadata.block_count = packet[BLOCK_COUNT];
    metadata.uid_count = packet[UID_COUNT];
    return metadata;
}

inline void setMetadataToOpTodData(uint8_t* packet, const Metadata& metadata) {
    for (size_t i = 0; i < ID_LENGTH; ++i) packet[i] = static_cast<uint8_t>(ARTNET_ID[i]);
    packet[OP_CODE_L] = (static_cast<uint16_t>(OpCode::TodData) >> 0) & 0x00FF;
    packet[OP_CODE_H] = (static_cast<uint16_t>(OpCode::TodData) >> 8) & 0x00FF;
    packet[PROTOCOL_VER_H] = (PROTOCOL_VER >> 8) & 0x00FF;
    packet[PROTOCOL_VER_L] = (PROTOCOL_VER >> 0) & 0x00FF;
    packet[RDM_VER] = metadata.rdm_ver;
    packet[PORT] = metadata.port;
    for (size_t i = SPARE_0; i <= SPARE_6; ++i) packet[i] = 0;
    packet[NET] = metadata.net;
    packet[COMMAND_RESPONSE] = metadata.command_response;
    packet[ADDRESS_H] = (metadata.address >> 8) & 0x00FF;
    packet[ADDRESS_L] = (metadata.address >> 0) & 0x00FF;
    packet[UID_TOTAL_H] = (metadata.uid_total >> 8) & 0x00FF;
    packet[UID_TOTAL_L] = (metadata.uid_total >> 0) & 0x00FF;
    packet[BLOCK_COUNT] = metadata.block_count;
    packet[UID_COUNT] = metadata.uid_count;
}

inline void setUidsToOpTodData(uint8_t* packet, const uint8_t* uids, const uint8_t uid_count) {
    if (uid_count > MAX_UIDS) return;
    memcpy(packet + TOD, uids, uid_count * 6);
}

}  // namespace art_tod_data

namespace art_tod_control {

enum Index : uint16_t {
    ID = 0,
    OP_CODE_L = 8,
    OP_CODE_H = 9,
    PROTOCOL_VER_H = 10,
    PROTOCOL_VER_L = 11,
    FILLER_1 = 12,
    FILLER_2 = 13,
    SPARE_0 = 14,
    SPARE_1 = 15,
    SPARE_2 = 16,
    SPARE_3 = 17,
    SPARE_4 = 18,
    SPARE_5 = 19,
    SPARE_6 = 20,
    NET = 21,
    COMMAND = 22,
    ADDRESS_H = 23,
    ADDRESS_L = 24
};

constexpr uint16_t SIZE = 25;

struct Metadata {
    uint8_t net;
    uint8_t command;
    uint16_t address;
};

using CallbackType = std::function<void(const Metadata&, const RemoteInfo&)>;

inline Metadata generateMetadataFromOpTodControl(const uint8_t* packet) {
    Metadata metadata;
    metadata.net = packet[NET];
    metadata.command = packet[COMMAND];
    metadata.address = (packet[ADDRESS_H] << 8) | packet[ADDRESS_L];
    return metadata;
}

inline void setMetadataToOpTodControl(uint8_t* packet, const Metadata& metadata) {
    for (size_t i = 0; i < ID_LENGTH; ++i) packet[i] = static_cast<uint8_t>(ARTNET_ID[i]);
    packet[OP_CODE_L] = (static_cast<uint16_t>(OpCode::TodControl) >> 0) & 0x00FF;
    packet[OP_CODE_H] = (static_cast<uint16_t>(OpCode::TodControl) >> 8) & 0x00FF;
    packet[PROTOCOL_VER_H] = (PROTOCOL_VER >> 8) & 0x00FF;
    packet[PROTOCOL_VER_L] = (PROTOCOL_VER >> 0) & 0x00FF;
    packet[FILLER_1] = 0;
    packet[FILLER_2] = 0;
    for (size_t i = SPARE_0; i <= SPARE_6; ++i) packet[i] = 0;
    packet[NET] = metadata.net;
    packet[COMMAND] = metadata.command;
    packet[ADDRESS_H] = (metadata.address >> 8) & 0x00FF;
    packet[ADDRESS_L] = (metadata.address >> 0) & 0x00FF;
}

}  // namespace art_tod_control

}  // namespace art_net

#endif  // ARTNET_ARTRDM_H
