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

ArtnetWiFiReceiver artnet;
uint8_t universe1 = 1;  // 0 - 15
uint8_t universe2 = 2;  // 0 - 15

void callback(const uint8_t* data, const uint16_t size) {
    // you can also use pre-defined callbacks
}

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
    // artnet.subscribe_net(0);     // optionally you can change
    // artnet.subscribe_subnet(0);  // optionally you can change

    // if Artnet packet comes to this universe, this function (lambda) is called
    artnet.subscribe(universe1, [&](const uint8_t* data, const uint16_t size) {
        Serial.print("lambda : artnet data (universe : ");
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
