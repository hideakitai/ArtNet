#pragma once
#ifndef ARTNET_ETHER_H
#define ARTNET_ETHER_H

#define ARTNET_ENABLE_ETHER

#include <Arduino.h>
#include <ArxTypeTraits.h>
#include <ArxContainer.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "Artnet/util/TeensyDirtySTLErrorSolution/TeensyDirtySTLErrorSolution.h"
#include "Artnet/Manager.h"

using ArtnetEther = art_net::Manager<EthernetUDP>;
using ArtnetEtherSender = art_net::Sender<EthernetUDP>;
using ArtnetEtherReceiver = art_net::Receiver<EthernetUDP>;

#endif  // ARTNET_ETHER_H
