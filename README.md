# ArtNet

Art-Net Sender/Receiver for Arduino (Ethernet, WiFi)

**NOTE : BREAKING API CHANGES (v0.2.0 or later)**

## Feature

- Art-Net with both Ethernet and WiFi
- Support a lot of boards which can use Ethernet or WiFi
- Multiple receiver callbacks depending on universe
- Mutilple destination streaming with sender
- One-line send to desired destination
- Flexible net/subnet/universe setting
- Easy data forwarding to [FastLED](https://github.com/FastLED/FastLED)

## Supported Platforms

#### WiFi

- ESP32
- ESP8266
- Arduino Uno WiFi Rev2
- Arduino MKR VIDOR 4000
- Arduino MKR WiFi 1010
- Arduino MKR WiFi 1000
- Arduino Nano 33 IoT

#### Ethernet

- ESP32
- ESP8266
- Almost all platforms without WiFi

## Usage

This library has following Art-Net controller options.
Please use them depending on the situation.

- ArtnetReveiver
- ArtnetSender
- Artnet (Integrated Sender/Receiver)

### Include Artnet

Please include `#include "Artnet.h` first.

If you use the board which has both `WiFi` and `Ethernet`, you can't use `#include <Artnet.h>`. Please replace it with `#include <ArtnetWiFi.h>` or `#include <ArtnetEther.h>` depending on the interface you want to use.

```C++
// For the boards which can use ether WiFi or Ethernet
#include <Artnet.h>
// OR use WiFi on the board which can use both WiFi and Ethernet
#include <ArtnetWiFi.h>
// OR use Ethenet on the board which can use both WiFi and Ethernet
#include <ArtnetEther.h>
```

### ArtnetReceiver

```C++
#include <Artnet.h>
ArtnetReceiver artnet;

void callback(const uint8_t* data, const uint16_t size) {
    // you can also use pre-defined callbacks
}

void setup() {
    // setup Ethernet/WiFi...

    artnet.begin(); // waiting for Art-Net in default port
    // artnet.begin(net, subnet); // optionally you can set net and subnet here

    artnet.subscribe(universe1, [](const uint8_t* data, const uint16_t size) {
        // if Artnet packet comes to this universe(0-15), this function is called
    });
    artnet.subscribe(universe2, callback);  // you can also use pre-defined callbacks
}

void loop() {
    artnet.parse(); // check if artnet packet has come and execute callback
}
```

### ArtnetSender

```C++
#include <Artnet.h>
ArtnetSender artnet;

void setup() {
    // setup Ethernet/WiFi...

    artnet.begin();
}

void loop() {
    // change send data as you want

    artnet.send("127.0.0.1", universe15bit, data_ptr, size); // one-line send
    // artnet.send("127.0.0.1", net, subnet, univ, data_ptr, size); // or you can set net and subnet

    artnet.streaming_data(data_ptr, size);
    artnet.streaming("127.0.0.1", universe15bit); // automatically send set data in 40fps (15bit universe)
    // artnet.streaming("127.0.0.1", net, subnet, univ); // or you can set net and subnet here
}
```

### Artnet (Integrated Sender/Receiver)

```C++
#include <Artnet.h>
Artnet artnet;

void setup()
{
    // setup Ethernet/WiFi...

    artnet.begin(); // send to localhost and listen to default port
    // artnet.begin(net, subnet); // optionally you can set net and subnet here

    artnet.subscribe(universe, [&](const uint8_t* data, const uint16_t size) {
        // if Artnet packet comes to this universe, this function is called
    });

    artnet.subscribe([&](const uint32_t univ, const uint8_t* data, const uint16_t size) {
        // if Artnet packet comes, this function is called to every universe
    });
}

void loop() {
    artnet.parse(); // check if artnet packet has come and execute callback

    // change send data as you want

    artnet.send("127.0.0.1", universe15bit, data_ptr, size); // one-line send
    // artnet.send("127.0.0.1", net, subnet, univ, data_ptr, size); // or you can set net and subnet

    artnet.streaming_data(data_ptr, size);
    artnet.streaming("127.0.0.1", universe15bit); // automatically send set data in 40fps (15bit universe)
    // artnet.streaming("127.0.0.1", net, subnet, univ); // or you can set net and subnet here
}
```

### Artnet Receiver + FastLED

```C++
#include <Artnet.h>
ArtnetReceiver artnet;

// FastLED
#define NUM_LEDS 1
CRGB leds[NUM_LEDS];
const uint8_t PIN_LED_DATA = 3;

void setup() {
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

void loop() {
    artnet.parse(); // check if artnet packet has come and execute callback
    FastLED.show();
}
```

## Other Settings

### Subscribing Callbacks with Net, Sub-Net and Universe as you like

- You can set Net (0-127) and Sub-Net (0-15) like `artnet.begin(net, subnet)`
- Universe (0-15) can be set in `artnet.subscribe(universe, callback)`,
- Callbacks are limited to 4 universes (depending on the spec of Art-Net)
- These universes (targets of the callbacks) are reflected to `net_sw` `sub_sw` `sw_in` in `ArtPollreply` automatically

```C++
artnet.begin(net, subnet); // net and subnet can be set only once
artnet.subscribe(univ1, callback1); // 4 callbacks can be set
artnet.subscribe(univ2, callback2); // these universes are reported to
artnet.subscribe(univ3, callback3); // Art-Net controller if it polls
artnet.subscribe(univ4, callback4); // Art-Net devices
```

Or you can register callbacks based on 15 bit universe. But these universes are not reflected to `ArtPollReply` automatically.

```C++
artnet.subscribe15bit(univ15bit1, callback1); // 4 callbacks can be set
artnet.subscribe15bit(univ15bit2, callback2); // these universes are NOT reported to
artnet.subscribe15bit(univ15bit3, callback3); // Art-Net controller if it polls
artnet.subscribe15bit(univ15bit4, callback4); // Art-Net devices
```

### Sending Art-Net to Net, Sub-Net and Universe as you like

#### One-line sender

```C++
artnet.send(ip, univ15bit, data, size);         // use 15bit universer or
artnet.send(ip, net, subnet, univ, data, size); // net, subnet, and universe
```

#### Streaming

```C++
artnet.streaming_data(data, size);       // set data first
artnet.streaming(ip, univ15bit);         // stream to 15bit universe or
artnet.streaming(ip, net, subnet, univ); // net, subnet, and universe
```

### ArtPollReply Setting

- This library supports `ArtPoll` and `ArtPollReply`
- `ArtPoll` is automatically parsed and sends `ArtPollReply`
- `net_sw` `sub_sw` `sw_in` etc. are set automatically based on registerd callbacks
- You can configure the information of `ArtPollReply` as follows
  - `void shortname(const String& sn)`
  - `void longname(const String& ln)`
  - `void nodereport(const String& nr)`

## APIs

### ArtnetSender

```C++
// streaming packet
void streaming_data(const uint8_t* const data, const uint16_t size);
void streaming_data(const uint16_t ch, const uint8_t data);
void streaming(const String& ip, const uint32_t universe_);
void streaming(const String& ip, const uint8_t net_, const uint8_t subnet_, const uint8_t universe_);
// one-line sender
void send(const String& ip, const uint32_t universe_, const uint8_t* const data, const uint16_t size);
void send(const String& ip, const uint8_t net_, const uint8_t subnet_, const uint8_t universe_, const uint8_t* const data, const uint16_t size);
// others
void physical(const uint8_t i);
uint8_t sequence() const;
```

### ArtnetReceiver

```C++
OpCode parse();
// subscribers
template <typename F> inline auto subscribe(const uint8_t universe, F&& func);
template <typename F> inline auto subscribe(const uint8_t universe, F* func);
template <typename F> inline auto subscribe(F&& func);
template <typename F> inline auto subscribe(F* func);
// for FastLED
inline void forward(const uint8_t universe, CRGB* leds, const uint16_t num);
// unsubscribe
inline void unsubscribe(const uint8_t universe);
inline void unsubscribe();
inline void clear_subscribers();
// ArtPollReply information
void shortname(const String& sn);
void longname(const String& ln);
void nodereport(const String& nr);
// option
void verbose(const bool b);
// others
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
```

### Note

Some boards without enough memory (e.g. Uno, Nano, etc.) may not be able to use integrated sender/receiver because of the lack of enough memory. Please consider to use more powerful board or to use only sender OR receiver.

## Reference

- [Spec (Art-Net 4)](http://artisticlicence.com/WebSiteMaster/User%20Guides/art-net.pdf)
- [Packet Summary](https://art-net.org.uk/structure/packet-summary-2/)
- [Packet Definition](https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/)

## Embedded Libraries

- [ArxContainer v0.4.0](https://github.com/hideakitai/ArxContainer)
- [ArxTypeTraits v0.2.3](https://github.com/hideakitai/ArxTypeTraits)
- [TeensyDirtySTLErrorSolution v0.1.0](https://github.com/hideakitai/TeensyDirtySTLErrorSolution)

## License

MIT
