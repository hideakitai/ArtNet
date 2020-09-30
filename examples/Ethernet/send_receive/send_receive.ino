#include <Artnet.h>

// Ethernet stuff
const IPAddress ip(192, 168, 1, 201);
uint8_t mac[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB};

Artnet artnet;

const uint16_t size = 512;
uint8_t data[size];
uint8_t value = 0;
uint32_t universe = 1;

void setup()
{
    Serial.begin(115200);

    Ethernet.begin(mac, ip);
    artnet.begin("127.0.0.1"); // send to localhost

    Serial.println("set subscriber");

    // if Artnet packet comes to this universe, this function is called
    artnet.subscribe(universe, [](uint8_t* data, uint16_t size)
    {
        Serial.print("artnet data (universe : ");
        Serial.print(universe);
        Serial.println(") = ");
        for (size_t i = 0; i < size; ++i)
        {
            Serial.print(data[i]); Serial.print(",");
        }
        Serial.println();
    });


    // if Artnet packet comes, this function is called to every universe
    artnet.subscribe([&](uint32_t univ, uint8_t* data, uint16_t size)
    {
        Serial.print("ArtNet data has come to universe: ");
        Serial.println(univ);
    });

    Serial.println("start");
}

void loop()
{
    artnet.parse(); // check if artnet packet has come and execute callback

    value = millis() % 256;
    memset(data, value, size);

    artnet.set(universe, data, size);
    artnet.streaming(); // automatically send set data in 40fps

    // Serial.println("loop");
}
