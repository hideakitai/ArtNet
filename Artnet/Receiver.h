#pragma once
#ifndef ARTNET_RECEIVER_H
#define ARTNET_RECEIVER_H

#include "Common.h"
#include "ArtDmx.h"
#include "ArtNzs.h"
#include "ArtPollReply.h"
#include "ArtTrigger.h"
#include "ArtSync.h"

namespace art_net {

namespace {

struct NoPrint : public Print
{
    size_t write(uint8_t) override { return 0; }
};
static NoPrint no_log;

} // namespace

template <typename S>
class Receiver_
{
    S *stream;
    Array<PACKET_SIZE> packet;

    art_dmx::CallbackMap callback_art_dmx_universes;
    art_dmx::CallbackType callback_art_dmx;
    art_nzs::CallbackMap callback_art_nzs_universes;
    art_sync::CallbackType callback_art_sync;
    art_trigger::CallbackType callback_art_trigger;
    ArtPollReplyConfig art_poll_reply_config;

    Print *logger {&no_log};

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
        if (!isNetworkReady()) {
            return OpCode::NoPacket;
        }
#endif
        size_t size = this->stream->parsePacket();
        if (size == 0) {
            return OpCode::NoPacket;
        }

        if (size > PACKET_SIZE) {
            this->logger->print(F("Packet size is unexpectedly too large: "));
            this->logger->println(size);
            size = PACKET_SIZE;
        }
        this->stream->read(this->packet.data(), size);

        if (!checkID()) {
            this->logger->println(F("Packet ID is not Art-Net"));
            return OpCode::ParseFailed;
        }

        RemoteInfo remote_info;
        remote_info.ip = this->stream->S::remoteIP();
        remote_info.port = (uint16_t)this->stream->S::remotePort();

        OpCode op_code = OpCode::Unsupported;
        OpCode received_op_code = static_cast<OpCode>(this->getOpCode());
        switch (received_op_code) {
            case OpCode::Dmx: {
                art_dmx::Metadata metadata = art_dmx::generateMetadataFrom(this->packet.data());
                if (this->callback_art_dmx) {
                    this->callback_art_dmx(this->getArtDmxData(), size - HEADER_SIZE, metadata, remote_info);
                }
                for (auto& cb : this->callback_art_dmx_universes) {
                    if (this->getArtDmxUniverse15bit() == cb.first) {
                        cb.second(this->getArtDmxData(), size - HEADER_SIZE, metadata, remote_info);
                    }
                }
                op_code = OpCode::Dmx;
                break;
            }
            case OpCode::Nzs: {
                art_nzs::Metadata metadata = art_nzs::generateMetadataFrom(this->packet.data());
                for (auto& cb : this->callback_art_nzs_universes) {
                    if (this->getArtDmxUniverse15bit() == cb.first) {
                        cb.second(this->getArtDmxData(), size - HEADER_SIZE, metadata, remote_info);
                    }
                }
                op_code = OpCode::Nzs;
                break;
            }
            case OpCode::Poll: {
                this->sendArtPollReply(remote_info);
                op_code = OpCode::Poll;
                break;
            }
            case OpCode::Trigger: {
                if (this->callback_art_trigger) {
                    ArtTriggerMetadata metadata = {
                        .oem = this->getArtTriggerOEM(),
                        .key = this->getArtTriggerKey(),
                        .sub_key = this->getArtTriggerSubKey(),
                        .payload = this->getArtTriggerPayload(),
                        .size = static_cast<uint16_t>(size - art_trigger::PAYLOAD),
                    };
                    this->callback_art_trigger(metadata, remote_info);
                }
                op_code = OpCode::Trigger;
                break;
            }
            case OpCode::Sync: {
                if (this->callback_art_sync) {
                    this->callback_art_sync(remote_info);
                }
                op_code = OpCode::Sync;
                break;
            }
            default: {
                this->logger->print(F("Unsupported OpCode: "));
                this->logger->println(this->getOpCode(), HEX);
                op_code = OpCode::Unsupported;
                break;
            }
        }

        this->stream->flush();
        return op_code;
    }

    // subscribe artdmx packet for specified net, subnet, and universe
    template <typename Fn>
    auto subscribeArtDmxUniverse(uint8_t net, uint8_t subnet, uint8_t universe, const Fn &func)
    -> std::enable_if_t<arx::is_callable<Fn>::value>
    {
        if (net > 0x7F) {
            this->logger->println(F("net should be less than 0x7F"));
            return;
        }
        if (subnet > 0xF) {
            this->logger->println(F("subnet should be less than 0xF"));
            return;
        }
        if (universe > 0xF) {
            this->logger->println(F("universe should be less than 0xF"));
            return;
        }
        uint16_t u = ((uint16_t)net << 8) | ((uint16_t)subnet << 4) | (uint16_t)universe;
        this->subscribeArtDmxUniverse(u, func);
    }

    // subscribe artdmx packet for specified universe (15 bit)
    template <typename Fn>
    auto subscribeArtDmxUniverse(uint16_t universe, const Fn &func)
    -> std::enable_if_t<arx::is_callable<Fn>::value>
    {
        this->callback_art_dmx_universes.insert(std::make_pair(universe, arx::function_traits<Fn>::cast(func)));
    }

    // subscribe artnzs packet for specified universe (15 bit)
    template <typename Fn>
    auto subscribeArtNzsUniverse(uint16_t universe, const Fn &func)
    -> std::enable_if_t<arx::is_callable<Fn>::value>
    {
        this->callback_art_nzs_universes.insert(std::make_pair(universe, arx::function_traits<Fn>::cast(func)));
    }

    // subscribe artdmx packet for all universes
    template <typename Fn>
    auto subscribeArtDmx(const Fn &func)
    -> std::enable_if_t<arx::is_callable<Fn>::value>
    {
        this->callback_art_dmx = arx::function_traits<Fn>::cast(func);
    }

    // subscribe other packets
    template <typename Fn>
    auto subscribeArtSync(const Fn &func)
    -> std::enable_if_t<arx::is_callable<Fn>::value>
    {
        this->callback_art_sync = arx::function_traits<Fn>::cast(func);
    }

    template <typename Fn>
    auto subscribeArtTrigger(const Fn &func)
    -> std::enable_if_t<arx::is_callable<Fn>::value>
    {
        this->callback_art_trigger = arx::function_traits<Fn>::cast(func);
    }

    void unsubscribeArtDmxUniverse(uint8_t net, uint8_t subnet, uint8_t universe)
    {
        uint16_t u = ((uint16_t)net << 8) | ((uint16_t)subnet << 4) | (uint16_t)universe;
        this->unsubscribe(u);
    }
    void unsubscribeArtDmxUniverse(uint16_t universe)
    {
        auto it = this->callback_art_dmx_universes.find(universe);
        if (it != this->callback_art_dmx_universes.end()) {
            this->callback_art_dmx_universes.erase(it);
        }
    }
    void unsubscribeArtDmxUniverses()
    {
        this->callback_art_dmx_universes.clear();
    }
    void unsubscribeArtDmx()
    {
        this->callback_art_dmx = nullptr;
    }

    void unsubscribeArtNzsUniverse(uint16_t universe)
    {
        auto it = this->callback_art_nzs_universes.find(universe);
        if (it != this->callback_art_nzs_universes.end()) {
            this->callback_art_nzs_universes.erase(it);
        }
    }

    void unsubscribeArtSync()
    {
        this->callback_art_sync = nullptr;
    }

    void unsubscribeArtTrigger()
    {
        this->callback_art_trigger = nullptr;
    }

#ifdef FASTLED_VERSION
    void forwardArtDmxDataToFastLED(uint8_t net, uint8_t subnet, uint8_t universe, CRGB* leds, uint16_t num)
    {
        uint16_t u = ((uint16_t)net << 8) | ((uint16_t)subnet << 4) | (uint16_t)universe;
        this->forwardArtDmxDataToFastLED(u, leds, num);
    }
    void forwardArtDmxDataToFastLED(uint16_t universe, CRGB* leds, uint16_t num)
    {
        this->subscribeArtDmxUniverse(universe, [this, leds, num](const uint8_t* data, const uint16_t size, const ArtDmxMetadata &, const RemoteInfo &) {
            size_t n;
            if (num <= size / 3) {
                // OK: requested number of LEDs is less than received data size
                n = num;
            } else {
                n = size / 3;
                this->logger->println(F("WARN: ArtNet packet size is less than requested LED numbers to forward"));
                this->logger->print(F("      requested: "));
                this->logger->print(num * 3);
                this->logger->print(F("      received : "));
                this->logger->println(size);
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

    // https://art-net.org.uk/how-it-works/discovery-packets/artpollreply/
    void setArtPollReplyConfigOem(uint16_t oem)
    {
        this->art_poll_reply_config.oem = oem;
    }
    void setArtPollReplyConfigEstaMan(uint16_t esta_man)
    {
        this->art_poll_reply_config.esta_man = esta_man;
    }
    void setArtPollReplyConfigStatus1(uint8_t status1)
    {
        this->art_poll_reply_config.status1 = status1;
    }
    void setArtPollReplyConfigStatus2(uint8_t status2)
    {
        this->art_poll_reply_config.status2 = status2;
    }
    void setArtPollReplyConfigShortName(const String &short_name)
    {
        this->art_poll_reply_config.short_name = short_name;
    }
    void setArtPollReplyConfigLongName(const String &long_name)
    {
        this->art_poll_reply_config.long_name = long_name;
    }
    void setArtPollReplyConfigNodeReport(const String &node_report)
    {
        this->art_poll_reply_config.node_report = node_report;
    }
    void setArtPollReplyConfigSwIn(size_t index, uint8_t sw_in)
    {
        if (index < 4) {
            this->art_poll_reply_config.sw_in[index] = sw_in;
        }
    }
    void setArtPollReplyConfigSwIn(uint8_t sw_in[4])
    {
        for (size_t i = 0; i < 4; ++i) {
            this->art_poll_reply_config.sw_in[i] = sw_in[i];
        }
    }
    void setArtPollReplyConfigSwIn(uint8_t sw_in_0, uint8_t sw_in_1, uint8_t sw_in_2, uint8_t sw_in_3)
    {
        this->setArtPollReplyConfigSwIn(0, sw_in_0);
        this->setArtPollReplyConfigSwIn(1, sw_in_1);
        this->setArtPollReplyConfigSwIn(2, sw_in_2);
        this->setArtPollReplyConfigSwIn(3, sw_in_3);
    }
    void setArtPollReplyConfig(
        uint16_t oem,
        uint16_t esta_man,
        uint8_t status1,
        uint8_t status2,
        const String &short_name,
        const String &long_name,
        const String &node_report,
        uint8_t sw_in[4]
    ) {
        this->setArtPollReplyConfigOem(oem);
        this->setArtPollReplyConfigEstaMan(esta_man);
        this->setArtPollReplyConfigStatus1(status1);
        this->setArtPollReplyConfigStatus2(status2);
        this->setArtPollReplyConfigShortName(short_name);
        this->setArtPollReplyConfigLongName(long_name);
        this->setArtPollReplyConfigNodeReport(node_report);
        this->setArtPollReplyConfigSwIn(sw_in);
    }
    void setArtPollReplyConfig(const ArtPollReplyConfig &cfg)
    {
        this->art_poll_reply_config = cfg;
    }

    void setLogger(Print* logger)
    {
        this->logger = logger;
    }

protected:
    void attach(S& s)
    {
        this->stream = &s;
    }

private:

    bool checkID() const
    {
        const char* idptr = reinterpret_cast<const char*>(this->packet.data());
        return !strcmp(ARTNET_ID, idptr);
    }

    uint16_t getOpCode() const
    {
        return (this->packet[art_dmx::OP_CODE_H] << 8) | this->packet[art_dmx::OP_CODE_L];
    }

    uint16_t getArtDmxUniverse15bit() const
    {
        return (this->packet[art_dmx::NET] << 8) | this->packet[art_dmx::SUBUNI];
    }

    const uint8_t *getArtDmxData() const
    {
        return &(this->packet[art_dmx::DATA]);
    }

    void sendArtPollReply(const RemoteInfo &remote)
    {
        const IPAddress my_ip = this->localIP();
        uint8_t my_mac[6];
        this->macAddress(my_mac);

        arx::stdx::map<uint16_t, bool> universes;
        for (const auto &cb_pair : this->callback_art_dmx_universes) {
            universes[cb_pair.first] = true;
        }
        for (const auto &cb_pair : this->callback_art_nzs_universes) {
            universes[cb_pair.first] = true;
        }
        // if no universe is subscribed, send reply for universe 0
        if (universes.empty()) {
            universes[0] = true;
        }

        for (const auto &u_pair : universes) {
            art_poll_reply::Packet reply = art_poll_reply::generatePacketFrom(my_ip, my_mac, u_pair.first, this->art_poll_reply_config);
            this->stream->beginPacket(remote.ip, DEFAULT_PORT);
            this->stream->write(reply.b, sizeof(art_poll_reply::Packet));
            this->stream->endPacket();
        }
    }

    uint16_t getArtTriggerOEM() const
    {
        return (this->packet[art_trigger::OEM_H] << 8) | this->packet[art_trigger::OEM_L];
    }

    uint8_t getArtTriggerKey() const
    {
        return this->packet[art_trigger::KEY];
    }

    uint8_t getArtTriggerSubKey() const
    {
        return this->packet[art_trigger::SUB_KEY];
    }

    const uint8_t *getArtTriggerPayload() const
    {
        return this->packet.data() + art_trigger::PAYLOAD;
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
    inline auto macAddress(uint8_t* mac) -> std::enable_if_t<std::is_same<T, EthernetUDP>::value>
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
    void begin(uint16_t recv_port = DEFAULT_PORT)
    {
        this->stream.begin(recv_port);
        this->Receiver_<S>::attach(this->stream);
    }
};

} // namespace art_net

#endif // ARTNET_RECEIVER_H
