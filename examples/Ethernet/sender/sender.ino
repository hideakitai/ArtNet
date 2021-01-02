#include <Artnet.h>

// Ethernet stuff
const IPAddress ip(192, 168, 0, 201);
uint8_t mac[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB};

ArtnetSender artnet;
const String target_ip = "192.168.0.200";
uint32_t universe = 1;

const uint16_t size = 512;
uint8_t data[size];
uint8_t value = 0;

void setup() {
    Serial.begin(115200);
    delay(2000);

    Ethernet.begin(mac, ip);
    artnet.begin();

    Serial.println("start");
    Serial.println(UDP_TX_PACKET_MAX_SIZE);
}

void loop() {
    value = (millis() / 4) % 256;
    memset(data, value, size);

    artnet.set(universe, data, size);
    artnet.streaming(target_ip);  // automatically send set data in 40fps
}
