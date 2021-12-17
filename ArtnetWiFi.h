#pragma once
#ifndef ARTNET_WIFI_H
#define ARTNET_WIFI_H

#if defined(ESP_PLATFORM) || defined(ESP8266) || defined(ARDUINO_AVR_UNO_WIFI_REV2)                             \
    || defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(ARDUINO_SAMD_MKR1000) \
    || defined(ARDUINO_SAMD_NANO_33_IOT)
#define ARTNET_ENABLE_WIFI
#endif

#ifdef ARTNET_ENABLE_WIFI
#include <Arduino.h>
#include "Artnet/util/ArxTypeTraits/ArxTypeTraits.h"
#include "Artnet/util/ArxContainer/ArxContainer.h"
#ifdef ESP_PLATFORM
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
#endif
#include "Artnet/ArtnetCommon.h"
using ArtnetWiFi = arx::artnet::Manager<WiFiUDP>;
using ArtnetWiFiSender = arx::artnet::Sender<WiFiUDP>;
using ArtnetWiFiReceiver = arx::artnet::Receiver<WiFiUDP>;
#endif  // ARTNET_ENABLE_WIFI

#endif  // ARTNET_WIFI_H
