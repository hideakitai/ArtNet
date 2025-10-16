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
#include "Artnet/Manager.h"

using ArtnetNativeEther = art_net::Manager<EthernetUDP>;
using ArtnetNativeEtherSender = art_net::Sender<EthernetUDP>;
using ArtnetNativeEtherReceiver = art_net::Receiver<EthernetUDP>;

#endif  // ARTNET_NATIVE_ETHER_H
