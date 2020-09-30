#pragma once
#ifndef ARDUINO_ARTNET_H
#define ARDUINO_ARTNET_H

// Spec (Art-Net 4) : http://artisticlicence.com/WebSiteMaster/User%20Guides/art-net.pdf
// Packet Summary : https://art-net.org.uk/structure/packet-summary-2/
// Packet Definition : https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/

#include <Arduino.h>
#include "util/ArxTypeTraits/ArxTypeTraits.h"
#include "util/ArxContainer/ArxContainer.h"

#if defined(ESP_PLATFORM)\
 || defined(ESP8266)\
 || defined(ARDUINO_AVR_UNO_WIFI_REV2)\
 || defined(ARDUINO_SAMD_MKRWIFI1010)\
 || defined(ARDUINO_SAMD_MKRVIDOR4000)\
 || defined(ARDUINO_SAMD_MKR1000)\
 || defined(ARDUINO_SAMD_NANO_33_IOT)
    #define ARTNET_ENABLE_WIFI
#endif

#if defined(ESP8266)\
 || !defined(ARTNET_ENABLE_WIFI)
    #define ARTNET_ENABLE_ETHER
#endif

#if !defined (ARTNET_ENABLE_WIFI)\
 && !defined (ARTNET_ENABLE_ETHER)
    #error THIS PLATFORM HAS NO WIFI OR ETHERNET OR NOT SUPPORTED ARCHITECTURE. PLEASE LET ME KNOW!
#endif

#ifdef ARTNET_ENABLE_WIFI
    #ifdef ESP_PLATFORM
        #include <WiFi.h>
        #include <WiFiUdp.h>
    #elif defined (ESP8266)
        #include <ESP8266WiFi.h>
        #include <WiFiUdp.h>
    #elif defined (ARDUINO_AVR_UNO_WIFI_REV2)\
        || defined(ARDUINO_SAMD_MKRWIFI1010)\
        || defined(ARDUINO_SAMD_MKRVIDOR4000)\
        || defined(ARDUINO_SAMD_NANO_33_IOT)
        #include <SPI.h>
        #include <WiFiNINA.h>
        #include <WiFiUdp.h>
    #elif defined (ARDUINO_SAMD_MKR1000)
        #include <SPI.h>
        #include <WiFi101.h>
        #include <WiFiUdp.h>
    #endif
#endif // ARTNET_ENABLE_WIFI

#ifdef ARTNET_ENABLE_ETHER
    #include <Ethernet.h>
    #include <EthernetUdp.h>
    #include "util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"
#endif // ARTNET_ENABLE_ETHER


namespace arduino
{
    namespace artnet
    {
        // Packet Summary : https://art-net.org.uk/structure/packet-summary-2/
        // Packet Definition : https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/

        enum class OpCode : uint16_t
        {
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
            FileFnReply = 0xF600
        };

        constexpr uint16_t OPC(OpCode op) { return static_cast<uint16_t>(op); }

        enum class Index : uint16_t
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

        constexpr uint16_t IDX(Index i) { return static_cast<uint16_t>(i); }

        constexpr uint16_t DEFAULT_PORT { 6454 };
        constexpr uint16_t HEADER_SIZE { 18 };
        constexpr uint16_t PACKET_SIZE { 530 };
        constexpr uint16_t PROTOCOL_VER { 0x0014 };
        constexpr uint8_t ID_LENGTH { 8 };
        constexpr char ID[ID_LENGTH] { "Art-Net" };
        constexpr float DEFAULT_FPS { 40. };
        constexpr uint32_t DEFAULT_INTERVAL_MS { (uint32_t)(1000. / DEFAULT_FPS) };

        static constexpr uint8_t NUM_PIXELS_PER_UNIV { 170 };

        using CallbackType = std::function<void(uint8_t* data, uint16_t size)>;
        struct Map { uint32_t universe; CallbackType func; };

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L // Have libstdc++11
        template <uint16_t SIZE>
        using Array = std::array<uint8_t, SIZE>;
        using CallbackMap = std::vector<Map>;
#else
        template <uint16_t SIZE>
        using Array = arx::vector<uint8_t, SIZE>;
        using CallbackMap = arx::vector<Map, 8>;
#endif

        template <typename S>
        class Sender_
        {
            Array<PACKET_SIZE> packet;
            const char* ip;
            uint16_t port {DEFAULT_PORT};
            uint8_t target_net {0};
            uint8_t target_subnet {0};
            uint8_t target_universe {0};
            uint8_t seq {0};
            uint8_t phy {0};

            uint32_t prev_send_ms {0};
            S* stream;

        public:

            virtual ~Sender_() {}

            void net(const uint8_t n) { target_net = n & 0x7F; }
            void subnet(const uint8_t s) { target_subnet = s & 0x0F; }
            void universe(const uint8_t u) { target_universe = u & 0x0F; }
            void universe15bit(const uint8_t u)
            {
                net((u >> 8) & 0xFF);
                subnet((u >> 4) & 0x0F);
                universe((u >> 0) & 0x0F);
            }

            void set(const uint8_t* const data, const uint16_t size = 512)
            {
                packet[IDX(Index::PHYSICAL)] = phy;
                packet[IDX(Index::NET)] = target_net;
                packet[IDX(Index::SUBUNI)] = (target_subnet << 4) | target_universe;
                packet[IDX(Index::LENGTH_H)] = (size >> 8) & 0xFF;
                packet[IDX(Index::LENGTH_L)] = (size >> 0) & 0xFF;
                memcpy((&packet[IDX(Index::DATA)]), data, size);
            }

            void set(const uint32_t universe_, const uint8_t* const data, const uint16_t size = 512)
            {
                universe15bit(universe_);
                set(data, size);
            }

            void set(const uint8_t net_, const uint8_t subnet_, const uint8_t universe_, const uint8_t* const data, const uint16_t size = 512)
            {
                net(net_);
                subnet(subnet_);
                universe(universe_);
                set(data, size);
            }

            void set(const uint16_t ch, const uint8_t data)
            {
                packet[IDX(Index::DATA) + ch] = data;
            }

            void send()
            {
                packet[IDX(Index::SEQUENCE)] = seq++;
                stream->beginPacket(ip, port);
                stream->write(packet.data(), packet.size());
                stream->endPacket();
            }
            void send(const uint8_t* const data, const uint16_t size = 512)
            {
                set(data, size);
                send();
            }
            void send(const uint32_t universe_, const uint8_t* const data, const uint16_t size = 512)
            {
                set(universe_, data, size);
                send();
            }
            void send(const uint8_t net_, const uint8_t subnet_, const uint8_t universe_, const uint8_t* const data, const uint16_t size = 512)
            {
                set(net_, subnet_, universe_, data, size);
                send();
            }

            void streaming()
            {
                if ((millis() - prev_send_ms) > DEFAULT_INTERVAL_MS)
                {
                    send();
                    prev_send_ms = millis();
                }
            }

            void physical(const uint8_t i) const { phy = constrain(i, 0, 3); }
            uint8_t sequence() const { return seq; }

        protected:

            void attach(S& s, const char* user_ip, const uint16_t user_port = DEFAULT_PORT)
            {
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
        class Receiver_
        {
            Array<PACKET_SIZE> packet;
            IPAddress remote_ip;
            uint16_t remote_port;
            CallbackMap callbacks;
            S* stream;

        public:

            virtual ~Receiver_() {}

            bool parse()
            {
                const size_t size = stream->parsePacket();
                if (size == 0) return false;

                uint8_t d[size];
                stream->read(d, size);

                if (checkID(d))
                {
                    if (opcode(d) == OPC(OpCode::Dmx))
                    {
                        memmove(packet.data(), d, size);
                        memcpy(packet.data(), d, size);
                        remote_ip = stream->S::remoteIP();
                        remote_port = (uint16_t)stream->S::remotePort();
                        for (auto& c : callbacks)
                        {
                            if (universe15bit() == c.universe) c.func(data(), size);
                        }
                        return true;
                    }
                }
                return false;
            }

            inline const IPAddress& ip() const { return remote_ip; }
            inline uint16_t port() const { return remote_port; }

            inline String id() const
            {
                String str;
                for (uint8_t i = 0; i < ID_LENGTH; ++i) str += packet[IDX(Index::ID) + i];
                return str;
            }
            inline uint16_t opcode() const
            {
                return (packet[IDX(Index::OP_CODE_H)] << 8) | packet[IDX(Index::OP_CODE_L)];
            }
            inline uint16_t opcode(const uint8_t* p) const
            {
                return (p[IDX(Index::OP_CODE_H)] << 8) | p[IDX(Index::OP_CODE_L)];
            }
            inline uint16_t version() const
            {
                return (packet[IDX(Index::PROTOCOL_VER_H)] << 8) | packet[IDX(Index::PROTOCOL_VER_L)];
            }
            inline uint8_t sequence() const
            {
                return packet[IDX(Index::SEQUENCE)];
            }
            inline uint8_t physical() const
            {
                return packet[IDX(Index::PHYSICAL)];
            }
            inline uint8_t net() const
            {
                return packet[IDX(Index::NET)] & 0x7F;
            }
            inline uint8_t subnet() const
            {
                return (packet[IDX(Index::SUBUNI)] >> 4) & 0x0F;
            }
            inline uint8_t universe() const
            {
                return packet[IDX(Index::SUBUNI)] & 0x0F;
            }
            inline uint16_t universe15bit() const
            {
                return (packet[IDX(Index::NET)] << 8) | packet[IDX(Index::SUBUNI)];
            }
            inline uint16_t length() const
            {
                return (packet[IDX(Index::LENGTH_H)] << 8) | packet[IDX(Index::LENGTH_L)];
            }
            inline uint16_t size() const
            {
                return length();
            }
            inline uint8_t* data()
            {
                return &(packet[HEADER_SIZE]);
            }
            inline uint8_t data(const uint16_t i) const
            {
                return packet[HEADER_SIZE + i];
            }

            inline void subscribe(const uint32_t universe, const CallbackType& func)
            {
                callbacks.push_back(Map{universe, func});
            }
            inline void subscribe(const CallbackType& func)
            {
                subscribe(0, func);
            }
            inline void subscribe(const uint8_t net, const uint8_t subnet, const uint8_t universe, const CallbackType& func)
            {
                uint32_t u = ((uint32_t)net << 8) | ((uint32_t)subnet << 4) | (uint32_t)universe;
                subscribe(u, func);
            }

            inline void unsubscribe(const uint32_t universe)
            {
                size_t idx = callbacks.size();
                for (size_t i = 0; i < callbacks.size(); ++i)
                    if (callbacks[i].universe == universe) {
                        idx = i;
                        break;
                    }

                if (idx != callbacks.size())
                    callbacks.erase(callbacks.begin() + idx);
            }
            inline void unsubscribe()
            {
                unsubscribe(0);
            }
            inline void unsubscribe(const uint8_t net, const uint8_t subnet, const uint8_t universe)
            {
                uint32_t u = ((uint32_t)net << 8) | ((uint32_t)subnet << 4) | (uint32_t)universe;
                unsubscribe(u);
            }

        protected:

            void attach(S& s) { stream = &s; }

        private:

            inline bool checkID() const
            {
                const char* idptr = reinterpret_cast<const char*>(packet.data());
                return !strcmp(ID, idptr);
            }
            inline bool checkID(const uint8_t* p) const
            {
                const char* idptr = reinterpret_cast<const char*>(p);
                return !strcmp(ID, idptr);
            }
        };

        template <typename S>
        class Manager : public Sender_<S>, public Receiver_<S>
        {
            S stream;
        public:
            void begin(const char* send_ip, const uint16_t send_port = DEFAULT_PORT, const uint16_t recv_port = DEFAULT_PORT)
            {
                stream.begin(recv_port);
                this->Sender_<S>::attach(stream, send_ip, send_port);
                this->Receiver_<S>::attach(stream);
            }
        };

        template <typename S>
        class Sender : public Sender_<S>
        {
            S stream;
        public:
            void begin(const char* ip, const uint16_t port = DEFAULT_PORT)
            {
                this->Sender_<S>::attach(stream, ip, port);
            }
        };

        template <typename S>
        class Receiver : public Receiver_<S>
        {
            S stream;
        public:
            void begin(const uint16_t port = DEFAULT_PORT)
            {
                stream.begin(port);
                this->Receiver_<S>::attach(stream);
            }
        };
    }
}

#ifdef ARTNET_ENABLE_WIFI
    using ArtnetWiFi = arduino::artnet::Manager<WiFiUDP>;
    using ArtnetWiFiSender = arduino::artnet::Sender<WiFiUDP>;
    using ArtnetWiFiReceiver = arduino::artnet::Receiver<WiFiUDP>;
#endif
#ifdef ARTNET_ENABLE_ETHER
    using Artnet = arduino::artnet::Manager<EthernetUDP>;
    using ArtnetSender = arduino::artnet::Sender<EthernetUDP>;
    using ArtnetReceiver = arduino::artnet::Receiver<EthernetUDP>;
#endif

#endif // ARDUINO_ARTNET_H
