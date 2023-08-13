#include <ArtnetETH.h>

// Ethernet stuff
const IPAddress ip(192, 168, 0, 201);
const IPAddress gateway(192, 168, 0, 1);
const IPAddress subnet(255, 255, 255, 0);

Artnet artnet;
const String target_ip = "192.168.0.200";
uint8_t universe = 1;  // 0 - 15

const uint16_t size = 512;
uint8_t data[size];
uint8_t value = 0;

void setup() {
    Serial.begin(115200);
    delay(2000);

    ETH.begin();
    ETH.config(ip, gateway, subnet);
    artnet.begin();
    // artnet.begin(net, subnet); // optionally you can change

    Serial.println("set subscriber");

    // if Artnet packet comes to this universe, this function is called
    artnet.subscribe(universe, [](const uint8_t* data, const uint16_t size) {
        Serial.print("artnet data (universe : ");
        Serial.print(universe);
        Serial.print(", size = ");
        Serial.print(size);
        Serial.print(") :");
        for (size_t i = 0; i < size; ++i) {
            Serial.print(data[i]);
            Serial.print(",");
        }
        Serial.println();
    });

    // if Artnet packet comes, this function is called to every universe
    artnet.subscribe([&](const uint32_t univ, const uint8_t* data, const uint16_t size) {
        Serial.print("ArtNet data has come to universe: ");
        Serial.println(univ);
    });

    Serial.println("start");
}

void loop() {
    artnet.parse();  // check if artnet packet has come and execute callback

    value = (millis() / 4) % 256;
    memset(data, value, size);

    artnet.streaming_data(data, size);
    artnet.streaming(target_ip, universe);  // automatically send set data in 40fps
    // artnet.streaming(target_ip, net, subnet, univ);  // or you can set net, subnet, and universe
}
