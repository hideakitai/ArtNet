#include <ArtnetETH.h>
#include <ArtnetWiFi.h>

// WiFi stuff
const char* ssid = "your-ssid";
const char* pwd = "your-password";
const IPAddress ip_wifi(192, 168, 1, 201);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);
// Ethernet stuff
const IPAddress ip_ether(192, 168, 0, 201);
uint8_t mac[] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB};

ArtnetETH artnet_ether;
ArtnetWiFi artnet_wifi;
const String target_ip = "192.168.1.200";
uint8_t universe = 1;  // 0 - 15

const uint16_t size = 512;
uint8_t data[size];
uint8_t value = 0;

// if Artnet packet comes to this universe, this function is called
void onArtDmxUniverse(const uint8_t *data, uint16_t size, const ArtDmxMetadata& metadata, const ArtNetRemoteInfo& remote)
{
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
};

// if Artnet packet comes, this function is called to every universe
void onArtDmx(const uint8_t *data, uint16_t size, const ArtDmxMetadata& metadata, const ArtNetRemoteInfo& remote)
{
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
};

void setup() {
    Serial.begin(115200);

    // WiFi stuff
    WiFi.begin(ssid, pwd);
    WiFi.config(ip_wifi, gateway, subnet);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.print("WiFi connected, IP = ");
    Serial.println(WiFi.localIP());
    // ArtnetWiFi
    artnet_wifi.begin();
    artnet_wifi.subscribeArtDmxUniverse(universe, onArtDmxUniverse);
    artnet_wifi.subscribeArtDmx(onArtDmx);

    // Ethernet stuff
    ETH.begin();
    ETH.config(ip_ether, gateway, subnet);
    // ArtnetETH
    artnet_ether.begin();
    artnet_ether.subscribeArtDmxUniverse(universe, onArtDmxUniverse);
    artnet_ether.subscribeArtDmx(onArtDmx);
}

void loop() {
    // check if artnet packet has come and execute callback
    artnet_wifi.parse();
    artnet_ether.parse();

    value = (millis() / 4) % 256;
    memset(data, value, size);

    artnet_wifi.setArtDmxData(data, size);
    artnet_ether.setArtDmxData(data, size);

    // automatically send set data in 40fps
    artnet_wifi.streamArtDmxTo(target_ip, universe);
    artnet_ether.streamArtDmxTo(target_ip, universe);
}
