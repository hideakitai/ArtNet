#pragma once
#ifndef ARTNET_ARTPOLLREPLY_H
#define ARTNET_ARTPOLLREPLY_H

#include "Common.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

namespace art_net {
namespace art_poll_reply {

union Packet {
    struct {
        uint8_t id[8];
        uint8_t op_code_l;
        uint8_t op_code_h;
        uint8_t ip[4];
        uint8_t port_l;
        uint8_t port_h;
        uint8_t ver_h;
        uint8_t ver_l;
        uint8_t net_sw;
        uint8_t sub_sw;
        uint8_t oem_h;
        uint8_t oem_l;
        uint8_t ubea_ver;
        uint8_t status_1;
        uint8_t esta_man_l;
        uint8_t esta_man_h;
        uint8_t short_name[18];
        uint8_t long_name[64];
        uint8_t node_report[64];
        uint8_t num_ports_h;
        uint8_t num_ports_l;
        uint8_t port_types[NUM_POLLREPLY_PUBLIC_PORT_LIMIT];
        uint8_t good_input[NUM_POLLREPLY_PUBLIC_PORT_LIMIT];
        uint8_t good_output[NUM_POLLREPLY_PUBLIC_PORT_LIMIT];
        uint8_t sw_in[NUM_POLLREPLY_PUBLIC_PORT_LIMIT];
        uint8_t sw_out[NUM_POLLREPLY_PUBLIC_PORT_LIMIT];
        uint8_t sw_video;
        uint8_t sw_macro;
        uint8_t sw_remote;
        uint8_t spare[3];
        uint8_t style;
        uint8_t mac[6];
        uint8_t bind_ip[4];
        uint8_t bind_index;
        uint8_t status_2;
        uint8_t filler[26];
    };
    uint8_t b[239];
};

class ArtPollReply
{
    String short_name {"Arduino ArtNet"};
    String long_name {"Ardino ArtNet Protocol by hideakitai/ArtNet"};
    String node_report {""};

public:
    Packet generate_reply(const IPAddress &my_ip, const uint8_t my_mac[6], uint32_t universe, uint8_t net_switch, uint8_t sub_switch)
    {
        Packet r;
        for (size_t i = 0; i < ID_LENGTH; i++) r.id[i] = static_cast<uint8_t>(ARTNET_ID[i]);
        r.op_code_l = ((uint16_t)OpCode::PollReply >> 0) & 0x00FF;
        r.op_code_h = ((uint16_t)OpCode::PollReply >> 8) & 0x00FF;
        memcpy(r.mac, my_mac, 6);
        for (size_t i = 0; i < 4; ++i) r.ip[i] = my_ip[i];
        r.port_l = (DEFAULT_PORT >> 0) & 0xFF;
        r.port_h = (DEFAULT_PORT >> 8) & 0xFF;
        r.ver_h = (PROTOCOL_VER >> 8) & 0x00FF;
        r.ver_l = (PROTOCOL_VER >> 0) & 0x00FF;
        r.oem_h = 0;        // https://github.com/tobiasebsen/ArtNode/blob/master/src/Art-NetOemCodes.h
        r.oem_l = 0xFF;     // OemUnknown
        r.ubea_ver = 0;     // UBEA not programmed
        r.status_1 = 0x00;  // Unknown / Normal
        r.esta_man_l = 0;   // No EATA manufacture code
        r.esta_man_h = 0;   // No ESTA manufacture code
        memset(r.short_name, 0, 18);
        memset(r.long_name, 0, 64);
        memset(r.node_report, 0, 64);
        memcpy(r.short_name, short_name.c_str(), short_name.length());
        memcpy(r.long_name, long_name.c_str(), long_name.length());
        memcpy(r.node_report, node_report.c_str(), node_report.length());
        r.num_ports_h = 0;                 // Reserved
        r.num_ports_l = 1;
        memset(r.sw_in, 0, 4);
        memset(r.sw_out, 0, 4);
        memset(r.port_types, 0, 4);
        memset(r.good_input, 0, 4);
        memset(r.good_output, 0, 4);
        r.net_sw = net_switch & 0x7F;
        r.sub_sw = sub_switch & 0x0F;
        // https://github.com/hideakitai/ArtNet/issues/81
        r.sw_in[0] = universe & 0x0F;
        r.sw_out[0] = 0;          // dummy: output port is flexible
        r.port_types[0] = 0xC0;   // I/O available by DMX512
        r.good_input[0] = 0x80;   // Data received without error
        r.good_output[0] = 0x80;  // Data transmitted without error
        r.sw_video = 0;   // Video display shows local data
        r.sw_macro = 0;   // No support for macro key inputs
        r.sw_remote = 0;  // No support for remote trigger inputs
        memset(r.spare, 0x00, 3);
        r.style = 0x00;  // A DMX to / from Art-Net device
        for (size_t i = 0; i < 4; ++i) r.bind_ip[i] = my_ip[i];
        r.bind_index = 0;
        r.status_2 = 0x08;  // sACN capable
        memset(r.filler, 0x00, 26);

        return r;
    }

    void shortname(const String& sn) {
        short_name = sn;
    }
    void longname(const String& ln) {
        long_name = ln;
    }
    void nodereport(const String& nr) {
        node_report = nr;
    }
};

} // namespace art_poll_reply
} // namespace art_net

#endif // ARTNET_ARTPOLLREPLY_H
