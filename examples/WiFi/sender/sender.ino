// Please include ArtnetWiFi.h to use Artnet on the platform
// which can use both WiFi and Ethernet
#include <ArtnetWiFi.h>
// this is also valid for other platforms which can use only WiFi
// #include <Artnet.h>

// WiFi stuff
const char* ssid = "your-ssid";
const char* pwd = "your-password";
const IPAddress ip(192, 168, 1, 201);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

ArtnetWiFiSender artnet;
const String target_ip = "192.168.1.200";
uint16_t universe = 1;

const uint16_t size = 512;
uint8_t data[size];
uint8_t value = 0;

void setup() {
    Serial.begin(115200);

    // WiFi stuff
    WiFi.begin(ssid, pwd);
    WiFi.config(ip, gateway, subnet);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.print("WiFi connected, IP = ");
    Serial.println(WiFi.localIP());

    artnet.begin();
}

void loop() {
    value = (millis() / 4) % 256;
    memset(data, value, size);

    artnet.setArtDmxData(data, size);
    artnet.streamArtDmxTo(target_ip, universe);  // automatically send set data in 40fps
    // artnet.streamArtDmxTo(target_ip, net, subnet, univ);  // or you can set net, subnet, and universe
}
