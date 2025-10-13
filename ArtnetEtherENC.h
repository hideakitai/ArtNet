#pragma once
#ifndef ARTNET_ETHER_H
#define ARTNET_ETHER_H

#include <Arduino.h>
#include <ArxTypeTraits.h>
#include <ArxContainer.h>
#include <EthernetENC.h>
#include <EthernetUdp.h>
#include "Artnet/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"
#include "Artnet/ReceiverTraits.h"

namespace art_net {

template <>
struct LocalIP<UIPEthernetClass>
{
    static IPAddress get(UIPEthernetClass& ethernet)
    {
        return ethernet.localIP();
    }
};

template <>
struct SubnetMask<UIPEthernetClass>
{
    static IPAddress get(UIPEthernetClass& ethernet)
    {
        return ethernet.subnetMask();
    }
};

template <>
struct MacAddress<UIPEthernetClass>
{
    static void get(UIPEthernetClass& ethernet, uint8_t mac[6])
    {
        ethernet.macAddress(mac);
    }
};

template <>
IPAddress getLocalIP<EthernetUDP>()
{
    return LocalIP<UIPEthernetClass>::get(Ethernet);
}

template <>
IPAddress getSubnetMask<EthernetUDP>()
{
    return SubnetMask<UIPEthernetClass>::get(Ethernet);
}

template <>
void getMacAddress<EthernetUDP>(uint8_t mac[6])
{
    MacAddress<UIPEthernetClass>::get(Ethernet, mac);
}

template <>
bool isNetworkReady<EthernetUDP>()
{
    return true;
}

} // namespace art_net

#include "Artnet/Manager.h"

using ArtnetEtherENC = art_net::Manager<EthernetUDP>;
using ArtnetEtherENCSender = art_net::Sender<EthernetUDP>;
using ArtnetEtherENCReceiver = art_net::Receiver<EthernetUDP>;

#endif  // ARTNET_ETHER_H
