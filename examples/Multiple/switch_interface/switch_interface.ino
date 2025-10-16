#include <ArtnetEther.h>
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

// To switch between ArtnetWiFi and ArtnetEther at runtime, use pointer to the interface
// std::unique_ptr<ArtnetSenderInterface> artnet; // if you only need sender functions
// std::unique_ptr<ArtnetReceiverInterface> artnet; // if you only need receiver functions
std::unique_ptr<ArtnetInterface> artnet; // if you need both sender and receiver functions
const String target_ip = "192.168.1.200";
uint8_t universe = 1;  // 0 - 15

const uint16_t size = 512;
uint8_t data[size];
uint8_t value = 0;

// Emulate runtime switching
bool use_wifi = true;

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

    if (use_wifi) {
        Serial.println("Use WiFi for ArtNet");
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
        artnet = std::make_unique<ArtnetWiFi>();
        artnet->begin();
        artnet->subscribeArtDmxUniverse(universe, onArtDmxUniverse);
        artnet->subscribeArtDmx(onArtDmx);
    } else {
        Serial.println("Use Ethernet for ArtNet");
        // Ethernet stuff
        Ethernet.begin(mac, ip_ether);
        // ArtnetEther
        artnet = std::make_unique<ArtnetEther>();
        artnet->begin();
        artnet->subscribeArtDmxUniverse(universe, onArtDmxUniverse);
        artnet->subscribeArtDmx(onArtDmx);
    }
}

void loop() {
    // check if artnet packet has come and execute callback
    artnet->parse();

    value = (millis() / 4) % 256;
    memset(data, value, size);

    artnet->setArtDmxData(data, size);

    // automatically send set data in 40fps
    artnet->streamArtDmxTo(target_ip, universe);
}
