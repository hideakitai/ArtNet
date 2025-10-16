#pragma once
#ifndef ARTNET_RECEIVER_TRAITS_H
#define ARTNET_RECEIVER_TRAITS_H

#include "Common.h"
#include "ArtDmx.h"
#include "ArtNzs.h"
#include "ArtPollReply.h"
#include "ArtTrigger.h"
#include "ArtSync.h"

namespace art_net {

template <typename T> struct LocalIP;
template <typename T> struct SubnetMask;
template <typename T> struct MacAddress;
template <typename T> struct IsNetworkReady;

template <typename T>
IPAddress localIP(T&& x) {
    return LocalIP<std::decay_t<T>>::get(std::forward<T>(x));
}
template <typename T>
IPAddress subnetMask(T&& x) {
    return SubnetMask<std::decay_t<T>>::get(std::forward<T>(x));
}
template <typename T>
void macAddress(T&& x, uint8_t mac[6]) {
    MacAddress<std::decay_t<T>>::get(std::forward<T>(x), mac);
}
template <typename T>
bool isNetworkReady(T&& x) {
    return IsNetworkReady<std::decay_t<T>>::get(std::forward<T>(x));
}

template <typename T>
IPAddress getLocalIP();
template <typename T>
IPAddress getSubnetMask();
template <typename T>
void getMacAddress(uint8_t mac[6]);
template <typename T>
bool isNetworkReady();

struct IReceiver_
{
    virtual ~IReceiver_() = default;

    virtual OpCode parse() = 0;
    // subscribe artdmx packet for specified net, subnet, and universe
    virtual void subscribeArtDmxUniverse(uint8_t net, uint8_t subnet, uint8_t universe, const ArtDmxCallback& func) = 0;
    // subscribe artdmx packet for specified universe (15 bit)
    virtual void subscribeArtDmxUniverse(uint16_t universe, const ArtDmxCallback& func) = 0;
    // subscribe artnzs packet for specified universe (15 bit)
    virtual void subscribeArtNzsUniverse(uint16_t universe, const ArtNzsCallback& func) = 0;
    // subscribe artdmx packet for all universes
    virtual void subscribeArtDmx(const ArtDmxCallback& func) = 0;
    // subscribe other packets
    virtual void subscribeArtSync(const ArtSyncCallback& func) = 0;
    // subscribe art_trigger packet
    virtual void subscribeArtTrigger(const ArtTriggerCallback& func) = 0;

    virtual void unsubscribeArtDmxUniverse(uint8_t net, uint8_t subnet, uint8_t universe) = 0;
    virtual void unsubscribeArtDmxUniverse(uint16_t universe) = 0;
    virtual void unsubscribeArtDmxUniverses() = 0;
    virtual void unsubscribeArtDmx() = 0;
    virtual void unsubscribeArtNzsUniverse(uint16_t universe) = 0;
    virtual void unsubscribeArtSync() = 0;
    virtual void unsubscribeArtTrigger() = 0;

#ifdef FASTLED_VERSION
    virtual void forwardArtDmxDataToFastLED(uint8_t net, uint8_t subnet, uint8_t universe, CRGB* leds, uint16_t num) = 0;
    virtual void forwardArtDmxDataToFastLED(uint16_t universe, CRGB* leds, uint16_t num) = 0;
#endif

    // https://art-net.org.uk/how-it-works/discovery-packets/artpollreply/
    virtual void setArtPollReplyConfigOem(uint16_t oem) = 0;
    virtual void setArtPollReplyConfigEstaMan(uint16_t esta_man) = 0;
    virtual void setArtPollReplyConfigStatus1(uint8_t status1) = 0;
    virtual void setArtPollReplyConfigStatus2(uint8_t status2) = 0;
    virtual void setArtPollReplyConfigShortName(const String &short_name) = 0;
    virtual void setArtPollReplyConfigLongName(const String &long_name) = 0;
    virtual void setArtPollReplyConfigNodeReport(const String &node_report) = 0;
    virtual void setArtPollReplyConfigSwIn(size_t index, uint8_t sw_in) = 0;
    virtual void setArtPollReplyConfigSwIn(uint8_t sw_in[4]) = 0;
    virtual void setArtPollReplyConfigSwIn(uint8_t sw_in_0, uint8_t sw_in_1, uint8_t sw_in_2, uint8_t sw_in_3) = 0;
    virtual void setArtPollReplyConfig(
        uint16_t oem,
        uint16_t esta_man,
        uint8_t status1,
        uint8_t status2,
        const String &short_name,
        const String &long_name,
        const String &node_report,
        uint8_t sw_in[4]
    ) = 0;
    virtual void setArtPollReplyConfig(const ArtPollReplyConfig &cfg) = 0;
    virtual void setLogger(Print* logger) = 0;
};

struct IReceiver : virtual IReceiver_
{
    virtual ~IReceiver() = default;
    virtual void begin(uint16_t recv_port = DEFAULT_PORT) = 0;
};

} // namespace art_net

using ArtnetReceiverInterface = art_net::IReceiver;

#endif // ARTNET_RECEIVER_TRAITS_H
