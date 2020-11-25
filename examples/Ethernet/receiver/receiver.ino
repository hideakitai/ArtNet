#include <Artnet.h>

// Ethernet stuff
const IPAddress ip(192, 168, 0, 201);
uint8_t mac[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB};

ArtnetReceiver artnet;
uint32_t universe1 = 1;
uint32_t universe2 = 2;

void callback(const uint8_t* data, const uint16_t size) {
    // you can also use pre-defined callbacks
}

void setup() {
    Serial.begin(115200);

    Ethernet.begin(mac, ip);
    artnet.begin();

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
