#pragma once
#ifndef ARTNET_ETH_H
#define ARTNET_ETH_H

#include <Arduino.h>
#include <ArxTypeTraits.h>
#include <ArxContainer.h>
#include <ETH.h>
#include <WiFiUdp.h>
#include "Artnet/ReceiverTraits.h"

// ETH.h is a library for Ethernet PHY, but we should use WiFi library's apis for sever/client
// So we define new type to avoid type confliction with WiFiUDP
struct ETHUdp : public WiFiUDP {};

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
IPAddress getLocalIP<ETHUdp>()
{
    return LocalIP<ETHClass>::get(ETH);
}

template <>
IPAddress getSubnetMask<ETHUdp>()
{
    return SubnetMask<ETHClass>::get(ETH);
}

template <>
void getMacAddress<ETHUdp>(uint8_t mac[6])
{
    MacAddress<ETHClass>::get(ETH, mac);
}

template <>
bool isNetworkReady<ETHUdp>()
{
    return true;
}

} // namespace art_net

#include "Artnet/Manager.h"

using ArtnetETH = art_net::Manager<ETHUdp>;
using ArtnetETHSender = art_net::Sender<ETHUdp>;
using ArtnetETHReceiver = art_net::Receiver<ETHUdp>;

#endif // ARTNET_ETH_H
