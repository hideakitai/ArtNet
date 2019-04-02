#include <Artnet.h>

// Ethernet stuff
const IPAddress ip(192, 168, 1, 201);
uint8_t mac[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB};

ArtnetSender artnet;
uint32_t universe = 1;

const uint16_t size = 512;
uint8_t data[size];
uint8_t value = 0;

void setup()
{
    Ethernet.begin(mac, ip);
    artnet.begin("127.0.0.1");
}

void loop()
{
    value = millis() % 256;
    memset(data, value, size);

    artnet.set(universe, data, size);
    artnet.streaming(); // automatically send set data in 40fps
}
