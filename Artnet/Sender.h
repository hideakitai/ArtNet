#pragma once
#ifndef ARTNET_SENDER_H
#define ARTNET_SENDER_H

#include "Common.h"
#include "ArtDmx.h"
#include "ArtTrigger.h"
#include "ArtSync.h"

namespace art_net {

template <typename S>
class Sender_ {
    Array<PACKET_SIZE> packet;
    art_dmx::ArtDmx artdmx_ctx;
    art_trigger::ArtTrigger art_trigger_ctx;

    IntervalMap intervals;
    S* stream;

public:
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
#else
    Sender_() {
        packet.resize(PACKET_SIZE);
    }
#endif
    virtual ~Sender_() {}

    // streaming packet
    void streaming_data(const uint8_t* const data, const uint16_t size) {
        artdmx_ctx.set_data(packet.data(), data, size);
    }
    void streaming_data(const uint16_t ch, const uint8_t data) {
        artdmx_ctx.set_data(packet.data(), ch, data);
    }

    void streaming(const String& ip, const uint32_t universe_) {
        uint32_t now = millis();
        if (intervals.find(universe_) == intervals.end()) {
            intervals.insert(make_pair(universe_, now));
        }
        if (now >= intervals[universe_] + DEFAULT_INTERVAL_MS) {
            set_universe(universe_);
            send_packet(ip);
            intervals[universe_] = now;
        }
    }
    void streaming(const String& ip, const uint8_t net_, const uint8_t subnet_, const uint8_t universe_) {
        uint32_t u = ((uint32_t)net_ << 8) | ((uint32_t)subnet_ << 4) | (uint32_t)universe_;
        streaming(ip, u);
    }

    // one-line sender
    void send(const String& ip, const uint32_t universe_, const uint8_t* const data, const uint16_t size) {
        set_universe(universe_);
        streaming_data(data, size);
        send_packet(ip);
    }
    void send(
        const String& ip,
        const uint8_t net_,
        const uint8_t subnet_,
        const uint8_t universe_,
        const uint8_t* const data,
        const uint16_t size) {
        set_universe(net_, subnet_, universe_);
        streaming_data(data, size);
        send_packet(ip);
    }

    void send_raw(const String& ip, uint16_t port, const uint8_t* const data, size_t size) {
        stream->beginPacket(ip.c_str(), port);
        stream->write(data, size);
        stream->endPacket();
    }

    void physical(const uint8_t i) {
        artdmx_ctx.set_physical(i);
    }

    uint8_t sequence() const {
        return artdmx_ctx.sequence();
    }

    // ArtTrigger
    void set_oem(uint16_t oem) {
        art_trigger_ctx.set_oem(oem);
    }
    void set_key(uint8_t key) {
        art_trigger_ctx.set_key(key);
    }
    void set_subkey(uint8_t subkey) {
        art_trigger_ctx.set_subkey(subkey);
    }
    void set_payload(const uint8_t* const payload, uint16_t size) {
        art_trigger_ctx.set_payload(packet.data(), payload, size);
    }
    void trigger(const String& ip) {
        art_trigger_ctx.set_header(packet.data());
        send_raw(ip, DEFAULT_PORT, packet.data(), packet.size());
    }

    // ArtSync
    void sync(const String& ip) {
        art_sync::set_header(packet.data());
        send_raw(ip, DEFAULT_PORT, packet.data(), packet.size());
    }

protected:
    void attach(S& s) {
        stream = &s;
    }

    void send_packet(const String& ip) {
#ifdef ARTNET_ENABLE_WIFI
        auto status = WiFi.status();
        auto is_connected = status == WL_CONNECTED;
#if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_RP2040)
        bool is_ap_active = WiFi.getMode() == WIFI_AP;
#else
        bool is_ap_active = status == WL_AP_CONNECTED;
#endif
        if (!is_connected && !is_ap_active) {
            return;
        }
#endif
        artdmx_ctx.set_header(packet.data());
        send_raw(ip, DEFAULT_PORT, packet.data(), packet.size());
    }

    void set_universe(const uint32_t universe_) {
        artdmx_ctx.set_universe(universe_);
    }
    void set_universe(const uint8_t net_, const uint8_t subnet_, const uint8_t universe_) {
        artdmx_ctx.set_universe(net_, subnet_, universe_);
    }
};

template <typename S>
class Sender : public Sender_<S> {
    S stream;

public:
    void begin() {
        stream.begin(DEFAULT_PORT);
        this->Sender_<S>::attach(stream);
    }
};

} // namespace art_net

#endif // ARTNET_SENDER_H
