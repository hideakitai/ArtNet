#pragma once
#ifndef ARTNET_MANAGER_TRAITS_H
#define ARTNET_MANAGER_TRAITS_H

#include "ReceiverTraits.h"
#include "SenderTraits.h"

namespace art_net {

struct IManager : virtual ISender_, virtual IReceiver_
{
    virtual ~IManager() = default;
    virtual void begin(uint16_t port = DEFAULT_PORT) = 0;
};

} // namespace art_net

using ArtnetInterface = art_net::IManager;

#endif // ARTNET_MANAGER_TRAITS_H
