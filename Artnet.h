#pragma once
#ifndef ARDUINO_ARTNET_H
#define ARDUINO_ARTNET_H

// Spec (Art-Net 4) : http://artisticlicence.com/WebSiteMaster/User%20Guides/art-net.pdf
// Packet Summary : https://art-net.org.uk/structure/packet-summary-2/
// Packet Definition : https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/

#include <Arduino.h>
#include "Artnet/util/ArxTypeTraits/ArxTypeTraits.h"
#include "Artnet/util/ArxContainer/ArxContainer.h"

#if defined(ESP_PLATFORM) || defined(ESP8266) || defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_NANO_33_IOT)
#define ARTNET_ENABLE_WIFI
#endif

#if defined(ESP_PLATFORM) || defined(ESP8266) || !defined(ARTNET_ENABLE_WIFI)
#define ARTNET_ENABLE_ETHER
#endif

#if !defined(ARTNET_ENABLE_WIFI) && !defined(ARTNET_ENABLE_ETHER)
#error THIS PLATFORM HAS NO WIFI OR ETHERNET OR NOT SUPPORTED ARCHITECTURE. PLEASE LET ME KNOW!
#endif

#ifdef ARTNET_ENABLE_WIFI
#ifdef ESP_PLATFORM
#include <WiFi.h>
#include <WiFiUdp.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_AVR_UNO_WIFI_REV2) || defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(ARDUINO_SAMD_NANO_33_IOT)
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#endif
#endif  // ARTNET_ENABLE_WIFI

#ifdef ARTNET_ENABLE_ETHER
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "Artnet/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"
#endif  // ARTNET_ENABLE_ETHER

#include "Artnet/ArtnetCommon.h"

#ifdef ARTNET_ENABLE_WIFI
using ArtnetWiFi = arx::artnet::Manager<WiFiUDP>;
using ArtnetWiFiSender = arx::artnet::Sender<WiFiUDP>;
using ArtnetWiFiReceiver = arx::artnet::Receiver<WiFiUDP>;
#endif
#ifdef ARTNET_ENABLE_ETHER
using Artnet = arx::artnet::Manager<EthernetUDP>;
using ArtnetSender = arx::artnet::Sender<EthernetUDP>;
using ArtnetReceiver = arx::artnet::Receiver<EthernetUDP>;
#endif

#endif  // ARDUINO_ARTNET_H
