#pragma once
#ifndef ARDUINO_ARTNET_H
#define ARDUINO_ARTNET_H

// Spec (Art-Net 4) : http://artisticlicence.com/WebSiteMaster/User%20Guides/art-net.pdf
// Packet Summary : https://art-net.org.uk/structure/packet-summary-2/
// Packet Definition : https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/

#include "ArtnetWiFi.h"
#include "ArtnetEther.h"

#if !defined(ARTNET_ENABLE_WIFI) && !defined(ARTNET_ENABLE_ETHER)
#error THIS PLATFORM HAS NO WIFI OR ETHERNET OR NOT SUPPORTED ARCHITECTURE. PLEASE LET ME KNOW!
#endif

#if defined(ARTNET_ENABLE_WIFI) && defined(ARTNET_ENABLE_ETHER)
#error BOTH WIFI AND ETHERNET ARE ENABLED. PLEASE INCLUDE ONLY ArtnetWiFi.h OR ArtnetEther.h
#endif

#endif  // ARDUINO_ARTNET_H
