# ArtNet

Art-Net Sender/Receiver for Arduino (Ethernet, WiFi)

## Feature

- support Art-Net with both Ethernet and WiFi
- register multiple callbacks depending on universe
- flexible net/subnet/universe setting


## Usage

This library has following Art-Net controller options.
Please use them depending on the situation.

- ArtnetSender
- ArtnetReveiver
- Artnet (Integrated Sender/Receiver)


### Warning

**From v0.1.11, arguments of callbacks must be `const` for safety. Previous sketches can not be compiled as is, so please add `const` to the arguments of callbacks.**

### ArtnetSender

```C++
#include <Artnet.h>

// declarations for Ethernet/WiFi

ArtnetSender artnet;

void setup()
{
    // setup Ethernet/WiFi...

    artnet.begin("127.0.0.1"); // set destination ip
}

void loop()
{
    // change send data as you want

    artnet.set(universe, data_ptr, size);
    artnet.streaming(); // automatically send set data in 40fps
}
```

### ArtnetReceiver

```C++
#include <Artnet.h>

// declarations for Ethernet/WiFi

ArtnetReceiver artnet;

void callback(const uint8_t* data, const uint16_t size)
{
    // you can also use pre-defined callbacks
}

void setup()
{
    // setup Ethernet/WiFi...

    artnet.begin(); // waiting for Art-Net in default port

    // if Artnet packet comes to this universe, this function is called
    artnet.subscribe(universe1, [](const uint8_t* data, const uint16_t size)
    {
        // use received data[], and size
    });

    // you can also use pre-defined callbacks
    artnet.subscribe(universe2, callback);
}

void loop()
{
    artnet.parse(); // check if artnet packet has come and execute callback
}

```

### Artnet (Integrated Sender/Receiver)

```C++
#include <Artnet.h>

// declarations for Ethernet/WiFi

Artnet artnet;

void setup()
{
    // setup Ethernet/WiFi...

    artnet.begin("127.0.0.1"); // send to localhost and listen to default port

    // if Artnet packet comes to this universe, this function is called
    artnet.subscribe(universe, [&](const uint8_t* data, const uint16_t size)
    {
        // do something with data coming to universe
    });

    // if Artnet packet comes, this function is called to every universe
    artnet.subscribe([&](const uint32_t univ, const uint8_t* data, const uint16_t size)
    {
        // do something with data coming to all universe
    });
}

void loop()
{
    artnet.parse(); // check if artnet packet has come and execute callback

    // change send data as you want

    artnet.set(universe, data, size); // set send data
    artnet.streaming(); // automatically send set data in 40fps
}
```


### Artnet Receiver + FastLED


``` C++
#include <Artnet.h>

// declarations for Ethernet/WiFi

ArtnetReceiver artnet;

// FastLED
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];
const uint8_t PIN_LED_DATA = 3;

void setup()
{
    // setup Ethernet/WiFi...

    // setup FastLED
    FastLED.addLeds<NEOPIXEL, PIN_LED>(&leds, NUM_LEDS);

    artnet.begin();

    // if Artnet packet comes to this universe, forward them to fastled directly
    artnet.forward(universe, leds, NUM_LEDS);

    // this can be achieved manually as follows
    // artnet.subscribe(universe, [](uint8_t* data, uint16_t size)
    // {
    //     // artnet data size per packet is 512 max
    //     // so there is max 170 pixel per packet (per universe)
    //     for (size_t pixel = 0; pixel < NUM_LEDS; ++pixel)
    //     {
    //         size_t idx = pixel * 3;
    //         leds[pixel].r = data[idx + 0];
    //         leds[pixel].g = data[idx + 1];
    //         leds[pixel].b = data[idx + 2];
    //     }
    // });
}

void loop()
{
    artnet.parse(); // check if artnet packet has come and execute callback
    FastLED.show();
}
```


## Other Settings

### Net, Sub-Net, Universe, and Universe(15bit)

You can set Net, Sub-Net, Universe and Universe(15bit) flexibly.

#### Sender

```C++
// set separately
artent.net(n);
artent.subnet(s);
artent.universe(u);
artnet.set(data, size);

// set as 15bit universe
artent.universe15bit(u);
artnet.set(data, size);

// set with data and size
artnet.set(universe15bit, data, size);
artnet.set(net, subnet, universe, data, size);
```

#### Receiver

```C++
artnet.subscribe(universe15bit, function);
artnet.subscribe(net, subnet, universe, function);
```

### One Time Sending (Not Streaming)

In Sender class, you can also send Art-Net packet once.
This sends only 1 packet (NOT streaming).

```C++
artnet.send(data, size);
artnet.send(universe15bit, data, size);
artnet.send(net, subnet, universe, data, size);
```

### Set Non-Default Port

```C++
// ArtnetSender
artnet.begin(ip);       // default
artnet.begin(ip, port); // set your own
// ArtnetReceiver
artnet.begin();     // default
artnet.begin(port); // set your own
// Artnet (integrated)
artnet.begin(ip);                       // default send/receiver
artnet.begin(ip, send_port, recv_port); // set your own
```

## APIs

### ArtnetSender

```C++
void net(const uint8_t n);
void subnet(const uint8_t s);
void universe(const uint8_t u);
void universe15bit(const uint8_t u);
void set(const uint8_t* const data, const uint16_t size = 512);
void set(const uint32_t universe_, const uint8_t* const data, const uint16_t size = 512);
void set(const uint8_t net_, const uint8_t subnet_, const uint8_t universe_, const uint8_t* const data, const uint16_t size = 512);
void send();
void send(const uint8_t* const data, const uint16_t size = 512);
void send(const uint32_t universe_, const uint8_t* const data, const uint16_t size = 512);
void send(const uint8_t net_, const uint8_t subnet_, const uint8_t universe_, const uint8_t* const data, const uint16_t size = 512);
void streaming();
void physical(const uint8_t i) const;
uint8_t sequence() const;
```

### ArtnetReceiver

```C++
bool parse();
inline const IPAddress& ip() const;
uint16_t port() const;
String id() const;
uint16_t opcode() const;
uint16_t opcode(const uint8_t* p) const;
uint16_t version() const;
uint8_t sequence() const;
uint8_t physical() const;
uint8_t net() const;
uint8_t subnet() const;
uint8_t universe() const;
uint16_t universe15bit() const;
uint16_t length() const;
uint16_t size() const;
uint8_t* data();
uint8_t data(const uint16_t i) const;
void subscribe(const uint32_t universe, const CallbackType& func);
void subscribe(const uint8_t net, const uint8_t subnet, const uint8_t universe, const CallbackType& func);
void subscribe(const CallbackAllType& func); // for all packet of all universe
void unsubscribe(const uint32_t universe);
void unsubscribe(const uint8_t net, const uint8_t subnet, const uint8_t universe);
void unsubscribe(); // for all packet of all universe
void clear_subscribers(); // clear all callbacks
inline void forward(const uint32_t universe, CRGB* leds, const uint16_t num);
inline void forward(const uint8_t net, const uint8_t subnet, const uint8_t universe, CRGB* leds, const uint16_t num);
```

## Supported Platform

This library currently supports following platforms and interfaces.
Please feel free to send PR or request for more board support!

#### WiFi

- ESP32
- ESP8266
- Arduino Uno WiFi Rev2
- Arduino MKR VIDOR 4000
- Arduino MKR WiFi 1010
- Arduino MKR WiFi 1000
- Arduino Nano 33 IoT

#### Ethernet

- ESP8266
- Almost all platforms without WiFi


### Note

Some boards without enough memory (e.g. Uno, Nano, etc.) may not be able to use integrated sender/receiver because of the lack of enough memory. Please consider to use more powerful board or to use only sender OR receiver.


## Reference

- [Spec (Art-Net 4)](http://artisticlicence.com/WebSiteMaster/User%20Guides/art-net.pdf)
- [Packet Summary](https://art-net.org.uk/structure/packet-summary-2/)
- [Packet Definition](https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/)


## Embedded Libraries

- [ArxContainer v0.3.10](https://github.com/hideakitai/ArxContainer)
- [ArxTypeTraits v0.2.1](https://github.com/hideakitai/ArxTypeTraits)
- [TeensyDirtySTLErrorSolution v0.1.0](https://github.com/hideakitai/TeensyDirtySTLErrorSolution)


## License

MIT
