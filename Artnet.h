#pragma once
#ifndef ARDUINO_ARTNET_H
#define ARDUINO_ARTNET_H

// Spec (Art-Net 4) : http://artisticlicence.com/WebSiteMaster/User%20Guides/art-net.pdf
// Packet Summary : https://art-net.org.uk/structure/packet-summary-2/
// Packet Definition : https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/

#include <Arduino.h>
#include "util/ArxTypeTraits/ArxTypeTraits.h"
#include "util/ArxContainer/ArxContainer.h"

#if defined(ESP_PLATFORM) || defined(ESP8266) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_NANO_33_IOT)
#define ARTNET_ENABLE_WIFI
#endif

#if defined(ESP8266) || !defined(ARTNET_ENABLE_WIFI)
#define ARTNET_ENABLE_ETHER
#endif

#if !defined(ARTNET_ENABLE_WIFI) && !defined(ARTNET_ENABLE_ETHER)
#error THIS PLATFORM HAS NO WIFI OR ETHERNET OR NOT SUPPORTED ARCHITECTURE. PLEASE LET ME KNOW!
#endif

#ifdef ARTNET_ENABLE_WIFI
#ifdef ESP_PLATFORM
#include <WiFi.h>
#include <WiFiUdp.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(ARDUINO_SAMD_NANO_33_IOT)
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#endif
#endif  // ARTNET_ENABLE_WIFI

#ifdef ARTNET_ENABLE_ETHER
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"
#endif  // ARTNET_ENABLE_ETHER

namespace arx {
    namespace artnet {
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
            // N/A
            NA = 0x0000,
        };

        constexpr uint16_t OPC(OpCode op) { return static_cast<uint16_t>(op); }

        enum class Index : uint16_t {
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

        constexpr uint16_t IDX(Index i) { return static_cast<uint16_t>(i); }

        constexpr uint16_t DEFAULT_PORT{6454};
        constexpr uint16_t HEADER_SIZE{18};
        constexpr uint16_t PACKET_SIZE{530};
        constexpr uint16_t PROTOCOL_VER{0x0014};
        constexpr uint8_t ID_LENGTH{8};
        constexpr char ID[ID_LENGTH]{"Art-Net"};
        constexpr float DEFAULT_FPS{40.};
        constexpr uint32_t DEFAULT_INTERVAL_MS{(uint32_t)(1000. / DEFAULT_FPS)};

        static constexpr uint8_t NUM_PIXELS_PER_UNIV{170};

        using CallbackAllType = std::function<void(const uint32_t universe, const uint8_t* data, const uint16_t size)>;
        using CallbackType = std::function<void(const uint8_t* data, const uint16_t size)>;

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
        template <uint16_t SIZE>
        using Array = std::array<uint8_t, SIZE>;
        using CallbackMap = std::map<uint32_t, CallbackType>;
        using namespace std;
#else
        template <uint16_t SIZE>
        using Array = arx::vector<uint8_t, SIZE>;
        using CallbackMap = arx::map<uint32_t, CallbackType, 8>;
        using namespace arx;
#endif

        union ArtPollReply {
            struct {
                uint8_t id[8];            // 8
                uint16_t op_code;         // 10
                uint8_t ip[4];            // 14
                uint16_t port;            // 16
                uint8_t ver_h;            // 17
                uint8_t ver_l;            // 18
                uint8_t net_sw;           // 19
                uint8_t sub_sw;           // 20
                uint8_t oem_h;            // 21
                uint8_t oem_l;            // 22
                uint8_t ubea_ver;         // 23
                uint8_t status_1;         // 24
                uint8_t esta_man_l;       // 25
                uint8_t esta_man_h;       // 26
                uint8_t short_name[18];   // 44
                uint8_t long_name[64];    // 108
                uint8_t node_report[64];  // 172
                uint8_t num_ports_h;      // 173
                uint8_t num_ports_l;      // 174
                uint8_t port_types[4];    // 178
                uint8_t good_input[4];    // 182
                uint8_t good_output[4];   // 186
                uint8_t sw_in[4];         // 190
                uint8_t sw_out[4];        // 194
                uint8_t sw_video;         // 195
                uint8_t sw_macro;         // 196
                uint8_t sw_remote;        // 197
                uint8_t spare[3];         // 200
                uint8_t style;            // 201
                uint8_t mac_h;            // 202
                uint8_t mac[4];           // 206
                uint8_t mac_l;            // 207
                uint8_t bind_ip[4];       // 211
                uint8_t bind_index;       // 212
                uint8_t status_2;         // 213
                uint8_t filler[8][26];    // 421
            };
            uint8_t b[421];
        };
        template <typename S>
        class Sender_ {
            Array<PACKET_SIZE> packet;
            String ip;
            uint16_t port{DEFAULT_PORT};
            uint8_t target_net{0};
            uint8_t target_subnet{0};
            uint8_t target_universe{0};
            uint8_t seq{0};
            uint8_t phy{0};

            uint32_t prev_send_ms{0};
            S* stream;

        public:
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
#else
            Sender_() { packet.resize(PACKET_SIZE); }
#endif
            virtual ~Sender_() {}

            void net(const uint8_t n) { target_net = n & 0x7F; }
            void subnet(const uint8_t s) { target_subnet = s & 0x0F; }
            void universe(const uint8_t u) { target_universe = u & 0x0F; }
            void universe15bit(const uint8_t u) {
                net((u >> 8) & 0xFF);
                subnet((u >> 4) & 0x0F);
                universe((u >> 0) & 0x0F);
            }

            void set(const uint8_t* const data, const uint16_t size = 512) {
                packet[IDX(Index::PHYSICAL)] = phy;
                packet[IDX(Index::NET)] = target_net;
                packet[IDX(Index::SUBUNI)] = (target_subnet << 4) | target_universe;
                packet[IDX(Index::LENGTH_H)] = (size >> 8) & 0xFF;
                packet[IDX(Index::LENGTH_L)] = (size >> 0) & 0xFF;
                memcpy((&packet[IDX(Index::DATA)]), data, size);
            }

            void set(const uint32_t universe_, const uint8_t* const data, const uint16_t size = 512) {
                universe15bit(universe_);
                set(data, size);
            }

            void set(const uint8_t net_, const uint8_t subnet_, const uint8_t universe_, const uint8_t* const data, const uint16_t size = 512) {
                net(net_);
                subnet(subnet_);
                universe(universe_);
                set(data, size);
            }

            void set(const uint16_t ch, const uint8_t data) {
                packet[IDX(Index::DATA) + ch] = data;
            }

            void send() {
                packet[IDX(Index::SEQUENCE)] = seq++;
                stream->beginPacket(ip.c_str(), port);
                stream->write(packet.data(), packet.size());
                stream->endPacket();
            }
            void send(const uint8_t* const data, const uint16_t size = 512) {
                set(data, size);
                send();
            }
            void send(const uint32_t universe_, const uint8_t* const data, const uint16_t size = 512) {
                set(universe_, data, size);
                send();
            }
            void send(const uint8_t net_, const uint8_t subnet_, const uint8_t universe_, const uint8_t* const data, const uint16_t size = 512) {
                set(net_, subnet_, universe_, data, size);
                send();
            }

            void streaming() {
                if ((millis() - prev_send_ms) > DEFAULT_INTERVAL_MS) {
                    send();
                    prev_send_ms = millis();
                }
            }

            void physical(const uint8_t i) const { phy = constrain(i, 0, 3); }

            // void poll_reply(const IPAddress& remote_ip, const uint16_t remote_port) {
            void poll_reply() {
                ArtPollReply r;
                for (size_t i = 0; i < ID_LENGTH; i++) r.id[i] = static_cast<uint8_t>(ID[i]);
                r.op_code = (uint16_t)OpCode::PollReply;
#ifdef ARTNET_ENABLE_WIFI
                IPAddress my_ip = WiFi.localIP();
#endif
#ifdef ARTNET_ENABLE_ETHER
                IPAddress my_ip = Ethernet.localIP();
#endif
                for (size_t i = 0; i < 4; ++i) r.ip[i] = my_ip[i];
                r.port = DEFAULT_PORT;
                r.ver_h = (PROTOCOL_VER >> 8) & 0x00FF;
                r.ver_l = (PROTOCOL_VER >> 0) & 0x00FF;
                r.net_sw = 0;       // TODO:
                r.sub_sw = 0;       // TODO:
                r.oem_h = 0;        // https://github.com/tobiasebsen/ArtNode/blob/master/src/Art-NetOemCodes.h
                r.oem_l = 0xFF;     // OemUnknown
                r.ubea_ver = 0;     // UBEA not programmed
                r.status_1 = 0x00;  // Unknown / Normal
                r.esta_man_l = 0;   // No EATA manufacture code
                r.esta_man_h = 0;   // No ESTA manufacture code
                char short_name[18] = "Arduino ArtNet";
                memcpy(r.short_name, short_name, sizeof(short_name));
                char long_name[64] = "Arduino ArtNet Protocol by hideakitai/ArtNet";
                memcpy(r.long_name, long_name, sizeof(long_name));
                static size_t counts = 0;
                char node_report[64];
                sprintf(node_report, "#0001 [%d] Arduino ArtNet enabled", counts++);
                memcpy(r.node_report, node_report, sizeof(node_report));
                r.num_ports_h = 0;  // Reserved
                r.num_ports_l = 1;  // This library implements only 1 port
                for (size_t i = 0; i < 4; ++i) {
                    r.port_types[i] = 0xC0;   // I/O available by DMX512
                    r.good_input[i] = 0x80;   // Data received without error
                    r.good_output[i] = 0x80;  // Data transmittedj without error
                    r.sw_in[i] = 0;           // TODO:
                    r.sw_out[i] = 0;          // TODO:
                }
                r.sw_video = 0;   // Video display shows local data
                r.sw_macro = 0;   // No support for macro key inputs
                r.sw_remote = 0;  // No support for remote trigger inputs
                memset(r.spare, 0x00, 3);
                r.style = 0x00;  // A DMX to / from Art-Net device
                r.mac_h = 0;     // Do not supply mac address
                memset(r.mac, 0x00, 4);
                r.mac_l = 0;
                for (size_t i = 0; i < 4; ++i) r.bind_ip[i] = my_ip[i];
                r.bind_index = 0;
                r.status_2 = 0x08;  // sACN capable (maybe)
                memset(r.filler, 0x00, 208);

                static const IPAddress DIRECTED_BROADCAST_ADDR(2, 255, 255, 255);
                stream->beginPacket(DIRECTED_BROADCAST_ADDR, DEFAULT_PORT);
                stream->write(r.b, 421);
                stream->endPacket();
            }

            uint8_t sequence() const { return seq; }

        protected:
            void attach(S& s, const String& user_ip, const uint16_t user_port = DEFAULT_PORT) {
                stream = &s;
                ip = user_ip;
                port = user_port;
                for (size_t i = 0; i < ID_LENGTH; i++) packet[IDX(Index::ID) + i] = static_cast<uint8_t>(ID[i]);
                packet[IDX(Index::OP_CODE_H)] = (OPC(OpCode::Dmx) >> 8) & 0x00FF;
                packet[IDX(Index::OP_CODE_L)] = (OPC(OpCode::Dmx) >> 0) & 0x00FF;
                packet[IDX(Index::PROTOCOL_VER_H)] = (PROTOCOL_VER >> 8) & 0x00FF;
                packet[IDX(Index::PROTOCOL_VER_L)] = (PROTOCOL_VER >> 0) & 0x00FF;
            }
        };

        template <typename S>
        class Receiver_ {
            Array<PACKET_SIZE> packet;
            IPAddress remote_ip;
            uint16_t remote_port;
            CallbackMap callbacks;
            CallbackAllType callback_all;
            S* stream;

        public:
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
#else
            Receiver_() { packet.resize(PACKET_SIZE); }
#endif

            virtual ~Receiver_() {}

            OpCode parse() {
                const size_t size = stream->parsePacket();
                if (size == 0) return OpCode::NA;

                uint8_t d[size];
                stream->read(d, size);
                if (size <= HEADER_SIZE) return OpCode::NA;  // discard packet if incomplete

                if (checkID(d)) {
                    switch (opcode(d)) {
                        case OPC(OpCode::Dmx): {
                            memcpy(packet.data(), d, size);
                            remote_ip = stream->S::remoteIP();
                            remote_port = (uint16_t)stream->S::remotePort();
                            if (callback_all) callback_all(universe15bit(), data(), size - HEADER_SIZE);
                            for (auto& c : callbacks)
                                if (universe15bit() == c.first) c.second(data(), size - HEADER_SIZE);
                            return OpCode::Dmx;
                        }
                        case OPC(OpCode::Poll): {
                            remote_ip = stream->S::remoteIP();
                            remote_port = (uint16_t)stream->S::remotePort();
                            return OpCode::Poll;
                        }
                        default: {
                            return OpCode::NA;
                        }
                    }
                }
                return OpCode::NA;
            }

            inline const IPAddress&
            ip() const {
                return remote_ip;
            }
            inline uint16_t port() const { return remote_port; }

            inline String id() const {
                String str;
                for (uint8_t i = 0; i < ID_LENGTH; ++i) str += packet[IDX(Index::ID) + i];
                return str;
            }
            inline uint16_t opcode() const {
                return (packet[IDX(Index::OP_CODE_H)] << 8) | packet[IDX(Index::OP_CODE_L)];
            }
            inline uint16_t opcode(const uint8_t* p) const {
                return (p[IDX(Index::OP_CODE_H)] << 8) | p[IDX(Index::OP_CODE_L)];
            }
            inline uint16_t version() const {
                return (packet[IDX(Index::PROTOCOL_VER_H)] << 8) | packet[IDX(Index::PROTOCOL_VER_L)];
            }
            inline uint8_t sequence() const {
                return packet[IDX(Index::SEQUENCE)];
            }
            inline uint8_t physical() const {
                return packet[IDX(Index::PHYSICAL)];
            }
            inline uint8_t net() const {
                return packet[IDX(Index::NET)] & 0x7F;
            }
            inline uint8_t subnet() const {
                return (packet[IDX(Index::SUBUNI)] >> 4) & 0x0F;
            }
            inline uint8_t universe() const {
                return packet[IDX(Index::SUBUNI)] & 0x0F;
            }
            inline uint16_t universe15bit() const {
                return (packet[IDX(Index::NET)] << 8) | packet[IDX(Index::SUBUNI)];
            }
            inline uint16_t length() const {
                return (packet[IDX(Index::LENGTH_H)] << 8) | packet[IDX(Index::LENGTH_L)];
            }
            inline uint16_t size() const {
                return length();
            }
            inline uint8_t* data() {
                return &(packet[HEADER_SIZE]);
            }
            inline uint8_t data(const uint16_t i) const {
                return packet[HEADER_SIZE + i];
            }

            template <typename F>
            inline auto subscribe(const uint32_t universe, F&& func)
                -> std::enable_if_t<arx::is_callable<F>::value> {
                callbacks.insert(make_pair(universe, arx::function_traits<F>::cast(func)));
            }
            template <typename F>
            inline auto subscribe(const uint32_t universe, F* func)
                -> std::enable_if_t<arx::is_callable<F>::value> {
                callbacks.insert(make_pair(universe, arx::function_traits<F>::cast(func)));
            }
            template <typename F>
            inline auto subscribe(F&& func)
                -> std::enable_if_t<arx::is_callable<F>::value> {
                callback_all = arx::function_traits<F>::cast(func);
            }
            template <typename F>
            inline auto subscribe(F* func)
                -> std::enable_if_t<arx::is_callable<F>::value> {
                callback_all = arx::function_traits<F>::cast(func);
            }
            template <typename F>
            inline auto subscribe(const uint8_t net, const uint8_t subnet, const uint8_t universe, F&& func)
                -> std::enable_if_t<arx::is_callable<F>::value> {
                uint32_t u = ((uint32_t)net << 8) | ((uint32_t)subnet << 4) | (uint32_t)universe;
                subscribe(u, func);
            }
            template <typename F>
            inline auto subscribe(const uint8_t net, const uint8_t subnet, const uint8_t universe, F* func)
                -> std::enable_if_t<arx::is_callable<F>::value> {
                uint32_t u = ((uint32_t)net << 8) | ((uint32_t)subnet << 4) | (uint32_t)universe;
                subscribe(u, func);
            }

            inline void unsubscribe(const uint32_t universe) {
                auto it = callbacks.find(universe);
                if (it != callbacks.end())
                    callbacks.erase(it);
            }
            inline void unsubscribe() {
                callback_all = nullptr;
            }
            inline void unsubscribe(const uint8_t net, const uint8_t subnet, const uint8_t universe) {
                uint32_t u = ((uint32_t)net << 8) | ((uint32_t)subnet << 4) | (uint32_t)universe;
                unsubscribe(u);
            }

            inline void clear_subscribers() {
                unsubscribe();
                callbacks.clear();
            }

#ifdef FASTLED_VERSION
            inline void forward(const uint32_t universe, CRGB* leds, const uint16_t num) {
                subscribe(universe, [&](const uint8_t* data, const uint16_t size) {
                    if (size < num * 3) Serial.println(F("ERROR: Too many LEDs to forward"));
                    for (size_t pixel = 0; pixel < num; ++pixel) {
                        size_t idx = pixel * 3;
                        leds[pixel].r = data[idx + 0];
                        leds[pixel].g = data[idx + 1];
                        leds[pixel].b = data[idx + 2];
                    }
                });
            }
            inline void forward(const uint8_t net, const uint8_t subnet, const uint8_t universe, CRGB* leds, const uint16_t num) {
                uint32_t u = ((uint32_t)net << 8) | ((uint32_t)subnet << 4) | (uint32_t)universe;
                forward(u, leds, num);
            }
#endif

        protected:
            void attach(S& s) {
                stream = &s;
            }

        private:
            inline bool checkID() const {
                const char* idptr = reinterpret_cast<const char*>(packet.data());
                return !strcmp(ID, idptr);
            }
            inline bool checkID(const uint8_t* p) const {
                const char* idptr = reinterpret_cast<const char*>(p);
                return !strcmp(ID, idptr);
            }
        };  // namespace artnet

        template <typename S>
        class Manager : public Sender_<S>, public Receiver_<S> {
            S stream;

        public:
            void begin(const String& send_ip, const uint16_t send_port = DEFAULT_PORT, const uint16_t recv_port = DEFAULT_PORT) {
                stream.begin(recv_port);
                this->Sender_<S>::attach(stream, send_ip, send_port);
                this->Receiver_<S>::attach(stream);
            }

            void parse() {
                OpCode op_code = this->Receiver_<S>::parse();
                switch (op_code) {
                    case OpCode::Poll: {
                        // this->Sender_<S>::poll_reply(
                        //     this->Receiver_<S>::ip(),
                        //     this->Receiver_<S>::port());
                        this->Sender_<S>::poll_reply();
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        };

        template <typename S>
        class Sender : public Sender_<S> {
            S stream;

        public:
            void begin(const String& ip, const uint16_t port = DEFAULT_PORT) {
                stream.begin(DEFAULT_PORT);
                this->Sender_<S>::attach(stream, ip, port);
            }
        };

        template <typename S>
        class Receiver : public Receiver_<S> {
            S stream;

        public:
            void begin(const uint16_t port = DEFAULT_PORT) {
                stream.begin(port);
                this->Receiver_<S>::attach(stream);
            }
        };
    }  // namespace artnet
}  // namespace arx

#ifdef ARTNET_ENABLE_WIFI
using ArtnetWiFi = arx::artnet::Manager<WiFiUDP>;
using ArtnetWiFiSender = arx::artnet::Sender<WiFiUDP>;
using ArtnetWiFiReceiver = arx::artnet::Receiver<WiFiUDP>;
#endif
#ifdef ARTNET_ENABLE_ETHER
using Artnet = arx::artnet::Manager<EthernetUDP>;
using ArtnetSender = arx::artnet::Sender<EthernetUDP>;
using ArtnetReceiver = arx::artnet::Receiver<EthernetUDP>;
#endif

#endif  // ARDUINO_ARTNET_H
