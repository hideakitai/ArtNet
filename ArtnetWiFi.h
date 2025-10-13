#pragma once
#ifndef ARTNET_WIFI_H
#define ARTNET_WIFI_H

#if defined(ESP_PLATFORM) || defined(ESP8266) || defined(ARDUINO_AVR_UNO_WIFI_REV2)                             \
    || defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(ARDUINO_SAMD_MKR1000) \
    || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_RASPBERRY_PI_PICO_W) || defined(ARDUINO_UNOR4_WIFI)
#define ARTNET_ENABLE_WIFI
#endif

#ifdef ARTNET_ENABLE_WIFI
#include <Arduino.h>
#include <ArxTypeTraits.h>
#include <ArxContainer.h>
#if defined(ESP_PLATFORM) || defined(ARDUINO_ARCH_RP2040)
#include <WiFi.h>
#include <WiFiUdp.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_MKRVIDOR4000) \
    || defined(ARDUINO_SAMD_NANO_33_IOT)
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_UNOR4_WIFI)
#include <WiFiS3.h>
#endif

#include "Artnet/ReceiverTraits.h"

namespace art_net {

template <>
struct LocalIP<WiFiClass>
{
    static IPAddress get(WiFiClass& wifi)
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
struct SubnetMask<WiFiClass>
{
    static IPAddress get(WiFiClass& wifi)
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
struct MacAddress<WiFiClass>
{
    static void get(WiFiClass& wifi, uint8_t mac[6])
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
IPAddress getLocalIP<WiFiUDP>()
{
    return LocalIP<WiFiClass>::get(WiFi);
}

template <>
IPAddress getSubnetMask<WiFiUDP>()
{
    return SubnetMask<WiFiClass>::get(WiFi);
}

template <>
void getMacAddress<WiFiUDP>(uint8_t mac[6])
{
    MacAddress<WiFiClass>::get(WiFi, mac);
}

} // namespace art_net

#include "Artnet/Manager.h"

using ArtnetWiFi = art_net::Manager<WiFiUDP>;
using ArtnetWiFiSender = art_net::Sender<WiFiUDP>;
using ArtnetWiFiReceiver = art_net::Receiver<WiFiUDP>;
#endif  // ARTNET_ENABLE_WIFI

#endif  // ARTNET_WIFI_H
