#pragma once
#ifndef ARTNET_MANAGER_H

#include "Common.h"
#include "Receiver.h"
#include "Sender.h"

namespace art_net {

template <typename S>
class Manager : public Sender_<S>, public Receiver_<S>
{
    S stream;

public:
    void begin(uint16_t recv_port = DEFAULT_PORT)
    {
        this->stream.begin(recv_port);
        this->Sender_<S>::attach(this->stream);
        this->Receiver_<S>::attach(this->stream);
    }

    void parse()
    {
        this->Receiver_<S>::parse();
    }
};


} // namespace art_net

#endif // ARTNET_MANAGER_H
