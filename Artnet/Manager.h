#pragma once
#ifndef ARTNET_MANAGER_H

#include "Common.h"
#include "Receiver.h"
#include "Sender.h"

namespace art_net {

template <typename S>
class Manager : public Sender_<S>, public Receiver_<S> {
    S stream;

public:
    void begin(
        const uint8_t subscribe_net = 0,
        const uint8_t subscribe_subnet = 0,
        const uint16_t recv_port = DEFAULT_PORT) {
        stream.begin(recv_port);
        this->Sender_<S>::attach(stream);
        this->Receiver_<S>::attach(stream, subscribe_net, subscribe_subnet);
    }

    void parse() {
        this->Receiver_<S>::parse();
    }
};


} // namespace art_net

#endif // ARTNET_MANAGER_H
