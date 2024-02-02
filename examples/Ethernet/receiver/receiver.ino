// Please include ArtnetEther.h to use Artnet on the platform
// which can use both WiFi and Ethernet
#include <ArtnetEther.h>
// #include <ArtnetNativeEther.h>  // only for Teensy 4.1
// this is also valid for other platforms which can use only Ethernet
// #include <Artnet.h>

// Ethernet stuff
const IPAddress ip(192, 168, 0, 201);
uint8_t mac[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB};

ArtnetReceiver artnet;
uint16_t universe1 = 1; // 0 - 32767
uint8_t universe2 = 2;  // 0 - 15

void callback(const uint8_t *data, uint16_t size, const ArtDmxMetadata &metadata, const ArtNetRemoteInfo &remote) {
    // you can also use pre-defined callbacks
}

void setup() {
    Serial.begin(115200);

    Ethernet.begin(mac, ip);
    artnet.begin();

    // if Artnet packet comes to this universe, this function (lambda) is called
    artnet.subscribeArtDmxUniverse(universe1, [&](const uint8_t *data, uint16_t size, const ArtDmxMetadata &metadata, const ArtNetRemoteInfo &remote) {
        Serial.print("lambda : artnet data from ");
        Serial.print(remote.ip);
        Serial.print(":");
        Serial.print(remote.port);
        Serial.print(", universe = ");
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
    artnet.subscribeArtDmxUniverse(universe2, callback);
}

void loop() {
    artnet.parse();  // check if artnet packet has come and execute callback
}
