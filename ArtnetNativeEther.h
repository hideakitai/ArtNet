#pragma once
#ifndef ARTNET_NATIVE_ETHER_H
#define ARTNET_NATIVE_ETHER_H

#include <Arduino.h>
#include <ArxTypeTraits.h>
#include <ArxContainer.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include "Artnet/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"
#include "Artnet/ReceiverTraits.h"

namespace art_net {

template <>
struct LocalIP<EthernetClass>
{
    static IPAddress get(EthernetClass const& ethernet)
    {
        return ethernet.localIP();
    }
};

template <>
struct SubnetMask<EthernetClass>
{
    static IPAddress get(EthernetClass const& ethernet)
    {
        return ethernet.subnetMask();
    }
};

template <>
struct MacAddress<EthernetClass>
{
    static void get(EthernetClass const& ethernet, uint8_t mac[6])
    {
        ethernet.MACAddress(mac);
    }
};

template <>
IPAddress getLocalIP<EthernetUDP>()
{
    return LocalIP<EthernetClass>::get(Ethernet);
}

template <>
IPAddress getSubnetMask<EthernetUDP>()
{
    return SubnetMask<EthernetClass>::get(Ethernet);
}

template <>
void getMacAddress<EthernetUDP>(uint8_t mac[6])
{
    MacAddress<EthernetClass>::get(Ethernet, mac);
}

template <>
bool isNetworkReady<EthernetUDP>()
{
    return true;
}

} // namespace art_net

#include "Artnet/Manager.h"

using ArtnetNativeEther = art_net::Manager<EthernetUDP>;
using ArtnetNativeEtherSender = art_net::Sender<EthernetUDP>;
using ArtnetNativeEtherReceiver = art_net::Receiver<EthernetUDP>;

#endif  // ARTNET_NATIVE_ETHER_H
