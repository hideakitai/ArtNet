#pragma once
#ifndef ARTNET_RECEIVER_H
#define ARTNET_RECEIVER_H

#include "Common.h"
#include "ArtDmx.h"
#include "ArtPollReply.h"
#include "ArtTrigger.h"
#include "ArtSync.h"

namespace art_net {

template <typename S>
class Receiver_ {
    Array<PACKET_SIZE> packet;
    IPAddress remote_ip;
    uint16_t remote_port;
    uint8_t net_switch;  // net of universe
    uint8_t sub_switch;  // subnet of universe
    art_dmx::CallbackMapForUniverse callbacks;
    art_dmx::CallbackTypeForAllPacket callback_all;
    art_sync::CallbackType callback_artsync;
    art_trigger::CallbackType callback_arttrigger;
    S* stream;
    bool b_verbose {false};
    art_poll_reply::ArtPollReply art_poll_reply_ctx;

public:
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
#else
    Receiver_() {
        packet.resize(PACKET_SIZE);
    }
#endif

    virtual ~Receiver_() {}

    OpCode parse() {
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
        const size_t size = stream->parsePacket();
        if (size == 0) return OpCode::NA;

        uint8_t* d = new uint8_t[size];
        stream->read(d, size);

        OpCode op_code = OpCode::NA;
        if (checkID(d)) {
            remote_ip = stream->S::remoteIP();
            remote_port = (uint16_t)stream->S::remotePort();
            OpCode received_op_code = static_cast<OpCode>(opcode(d));
            switch (received_op_code) {
                case OpCode::Dmx: {
                    memcpy(packet.data(), d, size);
                    if (callback_all) callback_all(universe15bit(), data(), size - HEADER_SIZE);
                    for (auto& c : callbacks)
                        if (universe15bit() == c.first) c.second(data(), size - HEADER_SIZE);
                    op_code = OpCode::Dmx;
                    break;
                }
                case OpCode::Poll: {
                    poll_reply();
                    op_code = OpCode::Poll;
                    break;
                }
                case OpCode::Trigger: {
                    if (callback_arttrigger) {
                        uint16_t oem = (d[art_trigger::OEM_H] << 8) | d[art_trigger::OEM_L];
                        uint8_t key = d[art_trigger::KEY];
                        uint8_t sub_key = d[art_trigger::SUB_KEY];
                        callback_arttrigger(oem, key, sub_key, d + art_trigger::PAYLOAD, size - art_trigger::PAYLOAD);
                    }
                    op_code = OpCode::Trigger;
                    break;
                }
                case OpCode::Sync: {
                    if (callback_artsync) callback_artsync();
                    op_code = OpCode::Sync;
                    break;
                }
                default: {
                    if (b_verbose) {
                        Serial.print(F("Unsupported OpCode: "));
                        Serial.println(opcode(d), HEX);
                    }
                    op_code = OpCode::NA;
                    break;
                }
            }
        }
        delete[] d;
        return op_code;
    }

    inline const IPAddress& ip() const {
        return remote_ip;
    }
    inline uint16_t port() const {
        return remote_port;
    }

    inline String id() const {
        String str;
        for (uint8_t i = 0; i < ID_LENGTH; ++i) str += packet[art_dmx::ID + i];
        return str;
    }
    inline uint16_t opcode() const {
        return (packet[art_dmx::OP_CODE_H] << 8) | packet[art_dmx::OP_CODE_L];
    }
    inline uint16_t opcode(const uint8_t* p) const {
        return (p[art_dmx::OP_CODE_H] << 8) | p[art_dmx::OP_CODE_L];
    }
    inline uint16_t version() const {
        return (packet[art_dmx::PROTOCOL_VER_H] << 8) | packet[art_dmx::PROTOCOL_VER_L];
    }
    inline uint8_t sequence() const {
        return packet[art_dmx::SEQUENCE];
    }
    inline uint8_t physical() const {
        return packet[art_dmx::PHYSICAL];
    }
    uint8_t net() const {
        return packet[art_dmx::NET] & 0x7F;
    }
    uint8_t subnet() const {
        return (packet[art_dmx::SUBUNI] >> 4) & 0x0F;
    }
    inline uint8_t universe() const {
        return packet[art_dmx::SUBUNI] & 0x0F;
    }
    inline uint16_t universe15bit() const {
        return (packet[art_dmx::NET] << 8) | packet[art_dmx::SUBUNI];
    }
    inline uint16_t length() const {
        return (packet[art_dmx::LENGTH_H] << 8) | packet[art_dmx::LENGTH_L];
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

    template <typename Fn>
    inline auto subscribe(const uint8_t universe, Fn&& func) -> std::enable_if_t<arx::is_callable<Fn>::value> {
        if (universe > 0xF) {
            if (b_verbose) {
                Serial.println(F("universe out of bounds"));
            }
            return;
        } else {
            uint32_t u = ((uint32_t)net_switch << 8) | ((uint32_t)sub_switch << 4) | (uint32_t)universe;
            callbacks.insert(make_pair(u, arx::function_traits<Fn>::cast(func)));
        }
    }
    template <typename Fn>
    inline auto subscribe(const uint8_t universe, Fn* func) -> std::enable_if_t<arx::is_callable<Fn>::value> {
        if (universe > 0xF) {
            if (b_verbose) {
                Serial.println(F("universe out of bounds"));
            }
        } else {
            uint32_t u = ((uint32_t)net_switch << 8) | ((uint32_t)sub_switch << 4) | (uint32_t)universe;
            callbacks.insert(make_pair(u, arx::function_traits<Fn>::cast(func)));
        }
    }
    template <typename Fn>
    inline auto subscribe15bit(const uint16_t universe, Fn&& func)
        -> std::enable_if_t<arx::is_callable<Fn>::value> {
        callbacks.insert(make_pair(universe, arx::function_traits<Fn>::cast(func)));
    }
    template <typename Fn>
    inline auto subscribe15bit(const uint16_t universe, Fn* func)
        -> std::enable_if_t<arx::is_callable<Fn>::value> {
        callbacks.insert(make_pair(universe, arx::function_traits<Fn>::cast(func)));
    }
    template <typename F>
    inline auto subscribe(F&& func) -> std::enable_if_t<arx::is_callable<F>::value> {
        callback_all = arx::function_traits<F>::cast(func);
    }
    template <typename F>
    inline auto subscribe(F* func) -> std::enable_if_t<arx::is_callable<F>::value> {
        callback_all = arx::function_traits<F>::cast(func);
    }
    template <typename F>
    inline auto subscribeArtSync(F&& func) -> std::enable_if_t<arx::is_callable<F>::value> {
        callback_artsync = arx::function_traits<F>::cast(func);
    }
    template <typename F>
    inline auto subscribeArtSync(F* func) -> std::enable_if_t<arx::is_callable<F>::value> {
        callback_artsync = arx::function_traits<F>::cast(func);
    }
    template <typename F>
    inline auto subscribeArtTrigger(F&& func) -> std::enable_if_t<arx::is_callable<F>::value> {
        callback_arttrigger = arx::function_traits<F>::cast(func);
    }
    template <typename F>
    inline auto subscribeArtTrigger(F* func) -> std::enable_if_t<arx::is_callable<F>::value> {
        callback_arttrigger = arx::function_traits<F>::cast(func);
    }

    inline void unsubscribe(const uint8_t universe) {
        auto it = callbacks.find(universe);
        if (it != callbacks.end()) callbacks.erase(it);
    }
    inline void unsubscribe() {
        callback_all = nullptr;
    }
    inline void unsubscribeArtSync() {
        callback_artsync = nullptr;
    }
    inline void unsubscribeArtTrigger() {
        callback_arttrigger = nullptr;
    }

    inline void clear_subscribers() {
        unsubscribe();
        callbacks.clear();
    }

#ifdef FASTLED_VERSION
    inline void forward(const uint8_t universe, CRGB* leds, const uint16_t num) {
        subscribe(universe, [&](const uint8_t* data, const uint16_t size) {
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

    void verbose(const bool b) {
        b_verbose = b;
    }

    // for ArtPollReply
    void shortname(const String& sn) {
        art_poll_reply_ctx.shortname(sn);
    }
    void longname(const String& ln) {
        art_poll_reply_ctx.longname(ln);
    }
    void nodereport(const String& nr) {
        art_poll_reply_ctx.nodereport(nr);
    }

protected:
    void attach(S& s, const uint8_t subscribe_net = 0, const uint8_t subscribe_subnet = 0) {
        stream = &s;
        if (subscribe_net > 128) {
            if (b_verbose) {
                Serial.println(F("Net must be < 128"));
            }
        } else {
            net_switch = subscribe_net;
        }
        if (subscribe_subnet > 16) {
            if (b_verbose) {
                Serial.println(F("Subnet must be < 16"));
            }
        } else {
            sub_switch = subscribe_subnet;
        }
    }

private:
    inline bool checkID(const uint8_t* p) const {
        const char* idptr = reinterpret_cast<const char*>(p);
        return !strcmp(ARTNET_ID, idptr);
    }

    void poll_reply() {
        const IPAddress my_ip = localIP();
        const IPAddress my_subnet = subnetMask();
        uint8_t my_mac[6];
        macAddress(my_mac);
        for (const auto &cb_pair : callbacks) {
            art_poll_reply::Packet r = art_poll_reply_ctx.generate_reply(my_ip, my_mac, cb_pair.first, net_switch, sub_switch);
            static const IPAddress local_broadcast_addr = IPAddress((uint32_t)my_ip | ~(uint32_t)my_subnet);
            stream->beginPacket(local_broadcast_addr, DEFAULT_PORT);
            stream->write(r.b, sizeof(art_poll_reply::Packet));
            stream->endPacket();
        }
    }

#ifdef ARTNET_ENABLE_WIFI
    template <typename T = S>
    auto localIP() -> std::enable_if_t<std::is_same<T, WiFiUDP>::value, IPAddress> {
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
    auto subnetMask() -> std::enable_if_t<std::is_same<T, WiFiUDP>::value, IPAddress> {
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
    auto macAddress(uint8_t* mac) -> std::enable_if_t<std::is_same<T, WiFiUDP>::value> {
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
    auto localIP() -> std::enable_if_t<std::is_same<T, EthernetUDP>::value, IPAddress> {
        return Ethernet.localIP();
    }
    template <typename T = S>
    auto subnetMask() -> std::enable_if_t<std::is_same<T, EthernetUDP>::value, IPAddress> {
        return Ethernet.subnetMask();
    }
    template <typename T = S>
    auto macAddress(uint8_t* mac) -> std::enable_if_t<std::is_same<T, EthernetUDP>::value> {
        Ethernet.MACAddress(mac);
    }
#endif  // ARTNET_ENABLE_ETHER

#ifdef ARTNET_ENABLE_ETH
    template <typename T = S>
    auto localIP() -> std::enable_if_t<std::is_same<T, WiFiUDP>::value, IPAddress> {
        return ETH.localIP();
    }
    template <typename T = S>
    auto subnetMask() -> std::enable_if_t<std::is_same<T, WiFiUDP>::value, IPAddress> {
        return ETH.subnetMask();
    }
    template <typename T = S>
    auto macAddress(uint8_t* mac) -> std::enable_if_t<std::is_same<T, WiFiUDP>::value> {
        ETH.macAddress(mac);
    }
#endif  // ARTNET_ENABLE_ETH
};

template <typename S>
class Receiver : public Receiver_<S> {
    S stream;

public:
    void begin(
        const uint8_t subscribe_net = 0,
        const uint8_t subscribe_subnet = 0,
        const uint16_t recv_port = DEFAULT_PORT) {
        stream.begin(recv_port);
        this->Receiver_<S>::attach(stream, subscribe_net, subscribe_subnet);
    }
};

} // namespace art_net

#endif // ARTNET_RECEIVER_H
