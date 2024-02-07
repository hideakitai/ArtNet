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
#include "Artnet/Manager.h"

using ArtnetWiFi = art_net::Manager<WiFiUDP>;
using ArtnetWiFiSender = art_net::Sender<WiFiUDP>;
using ArtnetWiFiReceiver = art_net::Receiver<WiFiUDP>;
#endif  // ARTNET_ENABLE_WIFI

#endif  // ARTNET_WIFI_H
