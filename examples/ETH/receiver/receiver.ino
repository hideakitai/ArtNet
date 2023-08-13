#include <ArtnetETH.h>

// Ethernet stuff
const IPAddress ip(192, 168, 0, 201);
const IPAddress gateway(192, 168, 0, 1);
const IPAddress subnet(255, 255, 255, 0);

ArtnetReceiver artnet;
uint32_t universe1 = 1;  // 0 - 15
uint32_t universe2 = 2;  // 0 - 15

void callback(const uint8_t* data, const uint16_t size) {
    // you can also use pre-defined callbacks
}

void setup() {
    Serial.begin(115200);

    ETH.begin();
    ETH.config(ip, gateway, subnet);
    artnet.begin();
    // artnet.subscribe_net(0);     // optionally you can change
    // artnet.subscribe_subnet(0);  // optionally you can change

    // if Artnet packet comes to this universe, this function is called
    artnet.subscribe(universe1, [&](const uint8_t* data, const uint16_t size) {
        Serial.print("artnet data (universe : ");
        Serial.print(universe1);
        Serial.print(", size = ");
        Serial.print(size);
        Serial.print(") :");
        for (size_t i = 0; i < size; ++i) {
            Serial.print(data[i]);
            Serial.print(",");
        }
        Serial.println();
    });

    // you can also use pre-defined callbacks
    artnet.subscribe(universe2, callback);
}

void loop() {
    artnet.parse();  // check if artnet packet has come and execute callback
}
