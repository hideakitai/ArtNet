#pragma once
#ifndef ARTNET_ETH_H
#define ARTNET_ETH_H

#include <Arduino.h>
#include <ArxTypeTraits.h>
#include <ArxContainer.h>
#include <ETH.h>
#include <WiFiUdp.h>
#include "Artnet/ReceiverTraits.h"

namespace art_net {

template <>
struct LocalIP<ETHClass>
{
    static IPAddress get(ETHClass& eth)
    {
        return eth.localIP();
    }
};
template <>
struct SubnetMask<ETHClass>
{
    static IPAddress get(ETHClass& eth)
    {
        return eth.subnetMask();
    }
};
template <>
struct MacAddress<ETHClass>
{
    static void get(ETHClass& eth, uint8_t mac[6])
    {
        eth.macAddress(mac);
    }
};

template <>
IPAddress getLocalIP<WiFiUDP>()
{
    return LocalIP<ETHClass>::get(ETH);
}

template <>
IPAddress getSubnetMask<WiFiUDP>()
{
    return SubnetMask<ETHClass>::get(ETH);
}

template <>
void getMacAddress<WiFiUDP>(uint8_t mac[6])
{
    MacAddress<ETHClass>::get(ETH, mac);
}

template <>
bool isNetworkReady<WiFiUDP>()
{
    return true;
}

} // namespace art_net

#include "Artnet/Manager.h"

// ETH.h is a library for Ethernet PHY, but we should use WiFi library's apis for sever/client
using ArtnetETH = art_net::Manager<WiFiUDP>;
using ArtnetETHSender = art_net::Sender<WiFiUDP>;
using ArtnetETHReceiver = art_net::Receiver<WiFiUDP>;

#endif // ARTNET_ETH_H
