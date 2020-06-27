#include <Artnet.h>

// WiFi stuff
const char* ssid = "your-ssid";
const char* pwd = "your-password";
const IPAddress ip(192, 168, 1, 201);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);

ArtnetWiFi artnet;

const uint16_t size = 512;
uint8_t data[size];
uint8_t value = 0;
uint32_t universe = 1;

void setup()
{
    Serial.begin(115200);

    // WiFi stuff
    WiFi.begin(ssid, pwd);
    WiFi.config(ip, gateway, subnet);
    while (WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(500); }
    Serial.print("WiFi connected, IP = "); Serial.println(WiFi.localIP());

    artnet.begin("127.0.0.1"); // send to localhost

    // if Artnet packet comes to this universe, this function is called
    artnet.subscribe(universe, [&](uint8_t* data, uint16_t size)
    {
        Serial.print("artnet data (universe : ");
        Serial.print(universe);
        Serial.println(") = ");
        for (size_t i = 0; i < size; ++i)
        {
            Serial.print(data[i]); Serial.print(",");
        }
        Serial.println();
    });
}

void loop()
{
    artnet.parse(); // check if artnet packet has come and execute callback

    value = millis() % 256;
    memset(data, value, size);

    artnet.set(universe, data, size);
    artnet.streaming(); // automatically send set data in 40fps
}
