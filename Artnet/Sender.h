#pragma once
#ifndef ARTNET_SENDER_H
#define ARTNET_SENDER_H

#include "Common.h"
#include "ArtDmx.h"
#include "ArtTrigger.h"
#include "ArtSync.h"

namespace art_net {

template <typename S>
class Sender_
{
    S* stream;
    Array<PACKET_SIZE> packet;
    art_dmx::LastSendTimeMsMap last_send_times;
    art_dmx::SequenceMap sequences;

public:
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
#else
    Sender_()
    {
        this->packet.resize(PACKET_SIZE);
    }
#endif

    // streaming artdmx packet
    void setArtDmxData(const uint8_t* const data, uint16_t size)
    {
        art_dmx::setDataTo(this->packet.data(), data, size);
    }
    void setArtDmxData(uint16_t ch, uint8_t data)
    {
        art_dmx::setDataTo(this->packet.data(), ch, data);
    }

    void streamArtDmxTo(const String& ip, uint16_t universe15bit)
    {
        uint8_t net = (universe15bit >> 8) & 0x7F;
        uint8_t subnet = (universe15bit >> 4) & 0x0F;
        uint8_t universe = (universe15bit >> 0) & 0x0F;
        this->streamArtDmxTo(ip, net, subnet, universe, 0);
    }
    void streamArtDmxTo(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe)
    {
        this->streamArtDmxTo(ip, net, subnet, universe, 0);
    }
    void streamArtDmxTo(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, uint8_t physical)
    {
        art_dmx::Destination dest {ip, net, subnet, universe};
        uint32_t now = millis();
        if (this->last_send_times.find(dest) == this->last_send_times.end()) {
            this->last_send_times.insert(make_pair(dest, uint32_t(0)));
        }
        if (now >= this->last_send_times[dest] + DEFAULT_INTERVAL_MS) {
            this->sendArxDmxInternal(dest, physical);
            this->last_send_times[dest] = now;
        }
    }

    // one-line artdmx sender
    void sendArtDmx(const String& ip, uint16_t universe15bit, const uint8_t* const data, uint16_t size)
    {
        uint8_t net = (universe15bit >> 8) & 0x7F;
        uint8_t subnet = (universe15bit >> 4) & 0x0F;
        uint8_t universe = (universe15bit >> 0) & 0x0F;
        this->sendArtDmx(ip, net, subnet, universe, 0, data, size);
    }
    void sendArtDmx(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, const uint8_t* const data, uint16_t size)
    {
        this->sendArtDmx(ip, net, subnet, universe, 0, data, size);
    }
    void sendArtDmx(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, uint8_t physical, const uint8_t *data, uint16_t size)
    {
        art_dmx::Destination dest {ip, net, subnet, universe};
        this->setArtDmxData(data, size);
        this->sendArxDmxInternal(dest, physical);
    }

    void sendArtTrigger(const String& ip, uint16_t oem = 0, uint8_t key = 0, uint8_t subkey = 0, const uint8_t *payload = nullptr, uint16_t size = 512)
    {
        art_trigger::setDataTo(packet.data(), oem, key, subkey, payload, size);
        this->sendRawData(ip, DEFAULT_PORT, packet.data(), packet.size());
    }

    void sendArtSync(const String& ip)
    {
        art_sync::setMetadataTo(packet.data());
        this->sendRawData(ip, DEFAULT_PORT, packet.data(), art_sync::PACKET_SIZE);
    }

protected:
    void attach(S& s)
    {
        this->stream = &s;
    }

    void sendArxDmxInternal(const art_dmx::Destination &dest, uint8_t physical)
    {
#ifdef ARTNET_ENABLE_WIFI
        if (!isNetworkReady()) {
            return;
        }
#endif
        if (this->sequences.find(dest) == this->sequences.end()) {
            this->sequences.insert(make_pair(dest, uint8_t(0)));
        }
        art_dmx::setMetadataTo(this->packet.data(), this->sequences[dest], physical, dest.net, dest.subnet, dest.universe);
        this->sendRawData(dest.ip, DEFAULT_PORT, this->packet.data(), this->packet.size());
        this->sequences[dest] = (this->sequences[dest] + 1) % 256;
    }

    void sendRawData(const String& ip, uint16_t port, const uint8_t* const data, size_t size)
    {
        this->stream->beginPacket(ip.c_str(), port);
        this->stream->write(data, size);
        this->stream->endPacket();
    }
};

template <typename S>
class Sender : public Sender_<S>
{
    S stream;

public:
    void begin()
    {
        this->stream.begin(DEFAULT_PORT);
        this->Sender_<S>::attach(this->stream);
    }
};

} // namespace art_net

#endif // ARTNET_SENDER_H
