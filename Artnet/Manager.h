#pragma once
#ifndef ARTNET_MANAGER_H
#define ARTNET_MANAGER_H

#include "Common.h"
#include "Receiver.h"
#include "Sender.h"
#include "ManagerTraits.h"

namespace art_net {

template <typename S>
class Manager : public IManager, public Sender_<S>, public Receiver_<S>
{
    S stream;

public:
    void begin(uint16_t port = DEFAULT_PORT) override
    {
        this->stream.begin(port);
        this->Sender_<S>::attach(this->stream);
        this->Receiver_<S>::attach(this->stream);
    }
};

} // namespace art_net

#endif // ARTNET_MANAGER_H
