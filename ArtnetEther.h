#pragma once
#ifndef ARTNET_ETHER_H
#define ARTNET_ETHER_H

#define ARTNET_ENABLE_ETHER

#include <Arduino.h>
#include "Artnet/util/ArxTypeTraits/ArxTypeTraits.h"
#include "Artnet/util/ArxContainer/ArxContainer.h"
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "Artnet/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"
#include "Artnet/ArtnetCommon.h"

using Artnet = arx::artnet::Manager<EthernetUDP>;
using ArtnetSender = arx::artnet::Sender<EthernetUDP>;
using ArtnetReceiver = arx::artnet::Receiver<EthernetUDP>;

#endif  // ARTNET_ETHER_H
