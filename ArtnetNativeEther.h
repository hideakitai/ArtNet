#pragma once
#ifndef ARTNET_NATIVE_ETHER_H
#define ARTNET_NATIVE_ETHER_H

#define ARTNET_ENABLE_ETHER

#include <Arduino.h>
#include <ArxTypeTraits.h>
#include <ArxContainer.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include "Artnet/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"
#include "Artnet/ArtnetCommon.h"

using Artnet = arx::artnet::Manager<EthernetUDP>;
using ArtnetSender = arx::artnet::Sender<EthernetUDP>;
using ArtnetReceiver = arx::artnet::Receiver<EthernetUDP>;

#endif  // ARTNET_NATIVE_ETHER_H

