#pragma once
#ifndef ARTNET_WIFI_H
#define ARTNET_WIFI_H

#include <Arduino.h>
#include <ArxTypeTraits.h>
#include <ArxContainer.h>
#if defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_RP2040)
#include <WiFi.h>
#include <WiFiUdp.h>
using ArtnetWiFiClass = WiFiClass;
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
using ArtnetWiFiClass = ESP8266WiFiClass;
#elif defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(ARDUINO_SAMD_NANO_33_IOT)
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
using ArtnetWiFiClass = WiFiClass;
#elif defined(ARDUINO_SAMD_MKR1000)
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
using ArtnetWiFiClass = WiFiClass;
#elif defined(ARDUINO_UNOR4_WIFI)
#include <WiFiS3.h>
using ArtnetWiFiClass = CWifi;
#endif
#include "Artnet/ReceiverTraits.h"

namespace art_net {

template <>
struct LocalIP<ArtnetWiFiClass>
{
    static IPAddress get(ArtnetWiFiClass& wifi)
    {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040)
        if( wifi.getMode() == WIFI_AP ) {
            return wifi.softAPIP();
        } else {
            return wifi.localIP();
        }
#else
        return wifi.localIP();
#endif
    }
};

template <>
struct SubnetMask<ArtnetWiFiClass>
{
    static IPAddress get(ArtnetWiFiClass& wifi)
    {
#if defined(ARDUINO_ARCH_ESP32)
        if( wifi.getMode() == WIFI_AP ) {
            return wifi.softAPSubnetMask();
        } else {
            return wifi.subnetMask();
        }
#else
        return wifi.subnetMask();
#endif
    }
};

template <>
struct MacAddress<ArtnetWiFiClass>
{
    static void get(ArtnetWiFiClass& wifi, uint8_t mac[6])
    {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_RP2040)
        if( wifi.getMode() == WIFI_AP ) {
            wifi.softAPmacAddress(mac);
        } else {
            wifi.macAddress(mac);
        }
#else
        wifi.macAddress(mac);
#endif
    }
};

template <>
struct IsNetworkReady<ArtnetWiFiClass>
{
    static bool get(ArtnetWiFiClass& wifi)
    {
        auto status = wifi.status();
        auto is_connected = status == WL_CONNECTED;
#if defined(ARDUINO_ARCH_ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_RP2040)
        bool is_ap_active = wifi.getMode() == WIFI_AP;
#else
        bool is_ap_active = status == WL_AP_CONNECTED;
#endif
        return is_connected || is_ap_active;
    }
};

template <>
IPAddress getLocalIP<WiFiUDP>()
{
    return LocalIP<ArtnetWiFiClass>::get(WiFi);
}

template <>
IPAddress getSubnetMask<WiFiUDP>()
{
    return SubnetMask<ArtnetWiFiClass>::get(WiFi);
}

template <>
void getMacAddress<WiFiUDP>(uint8_t mac[6])
{
    MacAddress<ArtnetWiFiClass>::get(WiFi, mac);
}

template <>
bool isNetworkReady<WiFiUDP>()
{
    return IsNetworkReady<ArtnetWiFiClass>::get(WiFi);
}

} // namespace art_net

#include "Artnet/Manager.h"

using ArtnetWiFi = art_net::Manager<WiFiUDP>;
using ArtnetWiFiSender = art_net::Sender<WiFiUDP>;
using ArtnetWiFiReceiver = art_net::Receiver<WiFiUDP>;

#endif  // ARTNET_WIFI_H
