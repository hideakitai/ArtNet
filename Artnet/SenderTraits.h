#pragma once
#ifndef ARTNET_SENDER_TRAITS_H
#define ARTNET_SENDER_TRAITS_H

namespace art_net {

struct ISender_
{
    virtual ~ISender_() = default;

    // streaming artdmx packet
    virtual void setArtDmxData(const uint8_t* const data, uint16_t size) = 0;
    virtual void setArtDmxData(uint16_t ch, uint8_t data) = 0;

    virtual void streamArtDmxTo(const String& ip, uint16_t universe15bit) = 0;
    virtual void streamArtDmxTo(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe) = 0;
    virtual void streamArtDmxTo(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, uint8_t physical) = 0;

    // streaming artnzs packet
    virtual void setArtNzsData(const uint8_t* const data, uint16_t size) = 0;
    virtual void setArtNzsData(uint16_t ch, uint8_t data) = 0;

    virtual void streamArtNzsTo(const String& ip, uint16_t universe15bit) = 0;
    virtual void streamArtNzsTo(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe) = 0;
    virtual void streamArtNzsTo(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, uint8_t start_code) = 0;

    // one-line artdmx sender
    virtual void sendArtDmx(const String& ip, uint16_t universe15bit, const uint8_t* const data, uint16_t size) = 0;
    virtual void sendArtDmx(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, const uint8_t* const data, uint16_t size) = 0;
    virtual void sendArtDmx(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, uint8_t physical, const uint8_t *data, uint16_t size) = 0;

    // one-line artnzs sender
    virtual void sendArtNzs(const String& ip, uint16_t universe15bit, const uint8_t* const data, uint16_t size) = 0;
    virtual void sendArtNzs(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, const uint8_t* const data, uint16_t size) = 0;
    virtual void sendArtNzs(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, uint8_t start_code, const uint8_t *data, uint16_t size) = 0;

    virtual void sendArtTrigger(const String& ip, uint16_t oem = 0, uint8_t key = 0, uint8_t subkey = 0, const uint8_t *payload = nullptr, uint16_t size = 512) = 0;

    virtual void sendArtSync(const String& ip) = 0;
};

struct ISender : virtual ISender_
{
    virtual ~ISender() = default;
    virtual void begin(uint16_t send_port = DEFAULT_PORT) = 0;
};

} // namespace art_net

using ArtnetSenderInterface = art_net::ISender;

#endif // ARTNET_SENDER_TRAITS_H
