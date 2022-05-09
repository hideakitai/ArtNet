#pragma once
#ifndef ARTNET_WIFI_H
#define ARTNET_WIFI_H
#endif

#ifndef ARTNET_ENABLE_ETH
#define ARTNET_ENABLE_ETH
#endif

#ifdef ARTNET_ENABLE_ETH
#include <Arduino.h>
#include "Artnet/util/ArxTypeTraits/ArxTypeTraits.h"
#include "Artnet/util/ArxContainer/ArxContainer.h"
#ifdef ESP_PLATFORM
#include <ETH.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#elif defined(ESP8266)
#include <ETH.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#endif
#include "Artnet/ArtnetCommon.h"
using ArtnetWiFi = arx::artnet::Manager<WiFiUDP>;
using ArtnetWiFiSender = arx::artnet::Sender<WiFiUDP>;
using ArtnetWiFiReceiver = arx::artnet::Receiver<WiFiUDP>;
#endif  // ARTNET_ENABLE_ETH


