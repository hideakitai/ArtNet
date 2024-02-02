#include <ArtnetETH.h>

// Ethernet stuff
const IPAddress ip(192, 168, 0, 201);
const IPAddress gateway(192, 168, 0, 1);
const IPAddress subnet(255, 255, 255, 0);

ArtnetSender artnet;
const String target_ip = "192.168.0.200";
uint16_t universe = 1;

const uint16_t size = 512;
uint8_t data[size];
uint8_t value = 0;

void setup() {
    Serial.begin(115200);
    delay(2000);

    ETH.begin();
    ETH.config(ip, gateway, subnet);
    artnet.begin();

    Serial.println("start");
#ifdef UDP_TX_PACKET_MAX_SIZE
    Serial.println(UDP_TX_PACKET_MAX_SIZE);
#endif
}

void loop() {
    value = (millis() / 4) % 256;
    memset(data, value, size);

    artnet.setArtDmxData(data, size);
    artnet.streamArtDmxTo(target_ip, universe);  // automatically send set data in 40fps
    // artnet.streamArtDmxTo(target_ip, net, subnet, univ);  // or you can set net, subnet, and universe
}
