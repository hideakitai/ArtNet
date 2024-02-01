#pragma once
#ifndef ARTNET_RECEIVER_H
#define ARTNET_RECEIVER_H

#include "Common.h"
#include "ArtDmx.h"
#include "ArtPollReply.h"
#include "ArtTrigger.h"
#include "ArtSync.h"

namespace art_net {

struct RemoteInfo
{
    IPAddress ip;
    uint16_t port;
};

template <typename S>
class Receiver_
{
    S *stream;
    Array<PACKET_SIZE> packet;
    IPAddress remote_ip;
    uint16_t remote_port;
    uint8_t net_switch;  // net of universe
    uint8_t sub_switch;  // subnet of universe

    art_dmx::CallbackMapForUniverse callbacks;
    art_dmx::CallbackTypeForAllPacket callback_all;
    art_sync::CallbackType callback_artsync;
    art_trigger::CallbackType callback_arttrigger;

    bool b_verbose {false};
    art_poll_reply::ArtPollReply art_poll_reply_ctx;

public:
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
#else
    Receiver_()
    {
        this->packet.resize(PACKET_SIZE);
    }
#endif

    OpCode parse()
    {
#ifdef ARTNET_ENABLE_WIFI
        auto status = WiFi.status();
        auto is_connected = status == WL_CONNECTED;
#if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_RP2040)
        bool is_ap_active = WiFi.getMode() == WIFI_AP;
#else
        bool is_ap_active = status == WL_AP_CONNECTED;
#endif
        if (!is_connected && !is_ap_active) {
            return OpCode::NA;
        }
#endif
        const size_t size = this->stream->parsePacket();
        if (size == 0) {
            return OpCode::NA;
        }

        uint8_t* d = new uint8_t[size];
        this->stream->read(d, size);

        OpCode op_code = OpCode::NA;
        if (checkID(d)) {
            this->remote_ip = this->stream->S::remoteIP();
            this->remote_port = (uint16_t)this->stream->S::remotePort();
            OpCode received_op_code = static_cast<OpCode>(this->opcode(d));
            switch (received_op_code) {
                case OpCode::Dmx: {
                    memcpy(this->packet.data(), d, size);
                    if (this->callback_all) {
                        this->callback_all(this->universe15bit(), this->data(), size - HEADER_SIZE);
                    }
                    for (auto& c : this->callbacks) {
                        if (this->universe15bit() == c.first) c.second(this->data(), size - HEADER_SIZE);
                    }
                    op_code = OpCode::Dmx;
                    break;
                }
                case OpCode::Poll: {
                    this->poll_reply();
                    op_code = OpCode::Poll;
                    break;
                }
                case OpCode::Trigger: {
                    if (this->callback_arttrigger) {
                        uint16_t oem = (d[art_trigger::OEM_H] << 8) | d[art_trigger::OEM_L];
                        uint8_t key = d[art_trigger::KEY];
                        uint8_t sub_key = d[art_trigger::SUB_KEY];
                        this->callback_arttrigger(oem, key, sub_key, d + art_trigger::PAYLOAD, size - art_trigger::PAYLOAD);
                    }
                    op_code = OpCode::Trigger;
                    break;
                }
                case OpCode::Sync: {
                    if (this->callback_artsync) {
                        this->callback_artsync();
                    }
                    op_code = OpCode::Sync;
                    break;
                }
                default: {
                    if (this->b_verbose) {
                        Serial.print(F("Unsupported OpCode: "));
                        Serial.println(this->opcode(d), HEX);
                    }
                    op_code = OpCode::NA;
                    break;
                }
            }
        }
        delete[] d;
        return op_code;
    }

    const IPAddress& ip() const
    {
        return this->remote_ip;
    }
    uint16_t port() const
    {
        return this->remote_port;
    }

    template <typename Fn>
    auto subscribe(uint8_t universe, const Fn &func)
    -> std::enable_if_t<arx::is_callable<Fn>::value>
    {
        if (universe > 0xF) {
            if (this->b_verbose) {
                Serial.println(F("universe out of bounds"));
            }
            return;
        } else {
            uint32_t u = ((uint32_t)this->net_switch << 8) | ((uint32_t)this->sub_switch << 4) | (uint32_t)universe;
            this->callbacks.insert(make_pair(u, arx::function_traits<Fn>::cast(func)));
        }
    }

    template <typename Fn>
    auto subscribe15bit(uint16_t universe, const Fn &func)
    -> std::enable_if_t<arx::is_callable<Fn>::value>
    {
        this->callbacks.insert(make_pair(universe, arx::function_traits<Fn>::cast(func)));
    }

    template <typename Fn>
    auto subscribe(const Fn &func)
    -> std::enable_if_t<arx::is_callable<Fn>::value>
    {
        this->callback_all = arx::function_traits<Fn>::cast(func);
    }

    template <typename Fn>
    auto subscribeArtSync(const Fn &func)
    -> std::enable_if_t<arx::is_callable<Fn>::value>
    {
        this->callback_artsync = arx::function_traits<Fn>::cast(func);
    }

    template <typename Fn>
    auto subscribeArtTrigger(const Fn &func)
    -> std::enable_if_t<arx::is_callable<Fn>::value>
    {
        this->callback_arttrigger = arx::function_traits<Fn>::cast(func);
    }

    void unsubscribe(uint8_t universe)
    {
        auto it = this->callbacks.find(universe);
        if (it != this->callbacks.end()) {
            this->callbacks.erase(it);
        }
    }

    void unsubscribe()
    {
        this->callback_all = nullptr;
    }

    void unsubscribeArtSync()
    {
        this->callback_artsync = nullptr;
    }

    void unsubscribeArtTrigger()
    {
        this->callback_arttrigger = nullptr;
    }

    void clear_subscribers()
    {
        this->unsubscribe();
        this->callbacks.clear();
    }

#ifdef FASTLED_VERSION
    void forward(const uint8_t universe, CRGB* leds, const uint16_t num)
    {
        this->subscribe(universe, [&](const uint8_t* data, const uint16_t size) {
            size_t n;
            if (num <= size / 3) {
                // OK: requested number of LEDs is less than received data size
                n = num;
            } else {
                n = size / 3;
                Serial.println(F("WARN: ArtNet packet size is less than requested LED numbers to forward"));
                Serial.print(F("      requested: "));
                Serial.print(num * 3);
                Serial.print(F("      received : "));
                Serial.println(size);
            }
            for (size_t pixel = 0; pixel < n; ++pixel) {
                size_t idx = pixel * 3;
                leds[pixel].r = data[idx + 0];
                leds[pixel].g = data[idx + 1];
                leds[pixel].b = data[idx + 2];
            }
        });
    }
#endif

    void verbose(bool b)
    {
        this->b_verbose = b;
    }

    // for ArtPollReply
    void shortname(const String& sn)
    {
        this->art_poll_reply_ctx.shortname(sn);
    }
    void longname(const String& ln)
    {
        this->art_poll_reply_ctx.longname(ln);
    }
    void nodereport(const String& nr)
    {
        this->art_poll_reply_ctx.nodereport(nr);
    }

protected:
    void attach(S& s, const uint8_t subscribe_net = 0, const uint8_t subscribe_subnet = 0)
    {
        this->stream = &s;
        if (subscribe_net > 128) {
            if (this->b_verbose) {
                Serial.println(F("Net must be < 128"));
            }
        } else {
            this->net_switch = subscribe_net;
        }
        if (subscribe_subnet > 16) {
            if (this->b_verbose) {
                Serial.println(F("Subnet must be < 16"));
            }
        } else {
            this->sub_switch = subscribe_subnet;
        }
    }

private:
    uint16_t opcode(const uint8_t* p) const
    {
        return (p[art_dmx::OP_CODE_H] << 8) | p[art_dmx::OP_CODE_L];
    }

    uint16_t universe15bit() const
    {
        return (this->packet[art_dmx::NET] << 8) | this->packet[art_dmx::SUBUNI];
    }

    const uint8_t *data() const
    {
        return &(this->packet[HEADER_SIZE]);
    }

    bool checkID(const uint8_t* p) const
    {
        const char* idptr = reinterpret_cast<const char*>(p);
        return !strcmp(ARTNET_ID, idptr);
    }

    void poll_reply()
    {
        const IPAddress my_ip = this->localIP();
        const IPAddress my_subnet = this->subnetMask();
        uint8_t my_mac[6];
        this->macAddress(my_mac);
        for (const auto &cb_pair : this->callbacks) {
            art_poll_reply::Packet r = this->art_poll_reply_ctx.generate_reply(my_ip, my_mac, cb_pair.first, this->net_switch, this->sub_switch);
            static const IPAddress local_broadcast_addr = IPAddress((uint32_t)my_ip | ~(uint32_t)my_subnet);
            this->stream->beginPacket(local_broadcast_addr, DEFAULT_PORT);
            this->stream->write(r.b, sizeof(art_poll_reply::Packet));
            this->stream->endPacket();
        }
    }

#ifdef ARTNET_ENABLE_WIFI
    template <typename T = S>
    auto localIP() -> std::enable_if_t<std::is_same<T, WiFiUDP>::value, IPAddress>
    {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040)
        if( WiFi.getMode() == WIFI_AP ) {
            return WiFi.softAPIP();
        } else {
            return WiFi.localIP();
        }
#else
        return WiFi.localIP();
#endif
    }
    template <typename T = S>
    auto subnetMask() -> std::enable_if_t<std::is_same<T, WiFiUDP>::value, IPAddress>
    {
#if defined(ARDUINO_ARCH_ESP32)
        if( WiFi.getMode() == WIFI_AP ) {
            return WiFi.softAPSubnetMask();
        } else {
            return WiFi.subnetMask();
        }
#else
        return WiFi.subnetMask();
#endif
    }
    template <typename T = S>
    auto macAddress(uint8_t* mac) -> std::enable_if_t<std::is_same<T, WiFiUDP>::value>
    {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040)
        if( WiFi.getMode() == WIFI_AP ) {
            WiFi.softAPmacAddress(mac);
        } else {
            WiFi.macAddress(mac);
        }
#else
        WiFi.macAddress(mac);
#endif
    }
#endif  // ARTNET_ENABLE_WIFI

#ifdef ARTNET_ENABLE_ETHER
    template <typename T = S>
    auto localIP() -> std::enable_if_t<std::is_same<T, EthernetUDP>::value, IPAddress>
    {
        return Ethernet.localIP();
    }
    template <typename T = S>
    auto subnetMask() -> std::enable_if_t<std::is_same<T, EthernetUDP>::value, IPAddress>
    {
        return Ethernet.subnetMask();
    }
    template <typename T = S>
    auto macAddress(uint8_t* mac) -> std::enable_if_t<std::is_same<T, EthernetUDP>::value>
    {
        Ethernet.MACAddress(mac);
    }
#endif  // ARTNET_ENABLE_ETHER

#ifdef ARTNET_ENABLE_ETH
    template <typename T = S>
    auto localIP() -> std::enable_if_t<std::is_same<T, WiFiUDP>::value, IPAddress>
    {
        return ETH.localIP();
    }
    template <typename T = S>
    auto subnetMask() -> std::enable_if_t<std::is_same<T, WiFiUDP>::value, IPAddress>
    {
        return ETH.subnetMask();
    }
    template <typename T = S>
    auto macAddress(uint8_t* mac) -> std::enable_if_t<std::is_same<T, WiFiUDP>::value>
    {
        ETH.macAddress(mac);
    }
#endif  // ARTNET_ENABLE_ETH
};

template <typename S>
class Receiver : public Receiver_<S>
{
    S stream;

public:
    void begin(uint8_t subscribe_net = 0, uint8_t subscribe_subnet = 0, uint16_t recv_port = DEFAULT_PORT)
    {
        this->stream.begin(recv_port);
        this->Receiver_<S>::attach(this->stream, this->subscribe_net, this->subscribe_subnet);
    }
};

} // namespace art_net

#endif // ARTNET_RECEIVER_H
