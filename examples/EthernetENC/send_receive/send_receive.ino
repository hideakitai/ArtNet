#ifdef __AVR__
#warning THIS EXAMPLE MAY USE TOO MUCH MEMORY FOR AVR. WE RECOMMEND TO USE SENDER OR RECEIVER ONLY.
#endif

#include <ArtnetEtherENC.h>


// Ethernet stuff
const IPAddress ip(192, 168, 0, 201);
uint8_t mac[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB};

Artnet artnet;
const String target_ip = "192.168.0.200";
uint8_t universe = 1;  // 0 - 15

const uint16_t size = 512;
uint8_t data[size];
uint8_t value = 0;

void setup() {
    Serial.begin(115200);
    delay(2000);

    Ethernet.begin(mac, ip);
    artnet.begin();

    // if Artnet packet comes to this universe, this function is called
    artnet.subscribeArtDmxUniverse(universe, [&](const uint8_t *data, uint16_t size, const ArtDmxMetadata& metadata, const ArtNetRemoteInfo& remote) {
        Serial.print("lambda : artnet data from ");
        Serial.print(remote.ip);
        Serial.print(":");
        Serial.print(remote.port);
        Serial.print(", universe = ");
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
    artnet.subscribeArtDmx([&](const uint8_t *data, uint16_t size, const ArtDmxMetadata& metadata, const ArtNetRemoteInfo& remote) {
        Serial.print("received ArtNet data from ");
        Serial.print(remote.ip);
        Serial.print(":");
        Serial.print(remote.port);
        Serial.print(", net = ");
        Serial.print(metadata.net);
        Serial.print(", subnet = ");
        Serial.print(metadata.subnet);
        Serial.print(", universe = ");
        Serial.print(metadata.universe);
        Serial.print(", sequence = ");
        Serial.print(metadata.sequence);
        Serial.print(", size = ");
        Serial.print(size);
        Serial.println(")");
    });
}

void loop() {
    artnet.parse();  // check if artnet packet has come and execute callback

    value = (millis() / 4) % 256;
    memset(data, value, size);

    artnet.setArtDmxData(data, size);
    artnet.streamArtDmxTo(target_ip, universe);  // automatically send set data in 40fps
    // artnet.streamArtDmxTo(target_ip, net, subnet, univ);  // or you can set net, subnet, and universe
}
