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

// I²C-address of Ethernet PHY (0 or 1 for LAN8720, 31 for TLK110)
#ifndef ETH_PHY_ADDR
  #define ETH_PHY_ADDR        1
#endif

// Type of the Ethernet PHY (LAN8720 or TLK110)
//typedef enum { ETH_PHY_LAN8720, ETH_PHY_TLK110, ETH_PHY_RTL8201, ETH_PHY_DP83848, ETH_PHY_DM9051, ETH_PHY_KSZ8081, ETH_PHY_MAX } eth_phy_type_t;

#ifndef ETH_PHY_TYPE
  #define ETH_PHY_TYPE    ETH_PHY_LAN8720
#endif

// Pin# of the enable signal for the external crystal oscillator (-1 to disable for internal APLL source)
#ifndef ETH_PHY_POWER
  #define ETH_PHY_POWER  16
#endif

// Pin# of the I²C clock signal for the Ethernet PHY
#ifndef ETH_PHY_MDC
  #define ETH_PHY_MDC     23
#endif

// Pin# of the I²C IO signal for the Ethernet PHY
#ifndef ETH_PHY_MDIO
  #define ETH_PHY_MDIO    18
#endif

#ifndef ETH_CLK_MODE
  #define ETH_CLK_MODE    ETH_CLOCK_GPIO0_IN  //  ETH_CLOCK_GPIO17_OUT
#endif


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


