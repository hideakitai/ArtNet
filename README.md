# ArtNet

[Art-Net](https://en.wikipedia.org/wiki/Art-Net) Sender/Receiver for Arduino (Ethernet, WiFi)

> [!WARNING]
> Breaking API changes from v0.4.0 and above

> [!WARNING]
> Dependent libraries removed from v0.3.0

If you have already installed this library before v0.3.0, please follow:

- Cloned from GitHub (manually): Please install dependent libraries manually
- Installed from library manager: re-install this library from library manager
  - Dependent libraries will be installed automatically

## Feature

- Supports following protocols:
  - ArtDmx
  - ArtNzs
  - ArtPoll/ArtPollReply
  - ArtTrigger
  - ArtSync
- Supports multiple WiFi/Ethernet libraries
  - WiFi
  - WiFiNINA
  - WiFiS3 (Arduino Uno R4 WiFi)
  - Ethernet
  - EthernetENC
  - ETH (ESP32)
- Supports a lot of boards which can use Ethernet or WiFi
- Multiple receiver callbacks depending on universe
- Mutilple destination streaming with sender
- One-line send to desired destination
- Flexible net/subnet/universe setting
- Easy data forwarding to [FastLED](https://github.com/FastLED/FastLED)

## Supported Platforms

#### WiFi

- ESP32
- ESP8266
- Raspberry Pi Pico W
- Arduino Uno WiFi Rev2
- Arduino Uno R4 WiFi
- Arduino MKR VIDOR 4000
- Arduino MKR WiFi 1010
- Arduino MKR WiFi 1000
- Arduino Nano 33 IoT

#### Ethernet

- ESP32 (Ethernet and ETH)
- ESP8266
- Almost all platforms without WiFi
- Any platform supported by ENC28J60 (please read following section)

<details>
  <summary>Notes for ENC28J60 ethernet controller (click to expand)</summary>

When using the ENC28J60 controller

- make sure to **clone** the [EthernetENC](https://github.com/JAndrassy/EthernetENC) library (version =< 2.0.4 doesn't support MAC address)
- simply replace `#include <Artnet.h>` with `#include <ArtnetEtherENC.h>`
</details>

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

void callback(const uint8_t *data, uint16_t size, const ArtDmxMetadata &metadata, const ArtNetRemoteInfo &remote) {
    // you can also use pre-defined callbacks
}

void setup() {
    // setup Ethernet/WiFi...

    artnet.begin(); // waiting for Art-Net in default port

    artnet.subscribeArtDmxUniverse(universe15bit, [&](const uint8_t *data, uint16_t size, const ArtDmxMetadata &metadata, const ArtNetRemoteInfo &remote) {
        // if Artnet packet comes to this universe(0-15), this function is called
    });

    // you can also use pre-defined callbacks
    artnet.subscribeArtDmxUniverse(net, subnet, universe, callback);
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

    // one-line send
    artnet.sendArtDmx("127.0.0.1", universe15bit, data_ptr, size);
    // or you can set net and subnet
    // artnet.sendArtDmx("127.0.0.1", net, subnet, universe, data_ptr, size);

    // To use streamArtDmxTo(), you need to setArtDmxData() before streamArtDmxTo()
    artnet.setArtDmxData(data_ptr, size);
    // automatically send set data in 40fps
    artnet.streamArtDmxTo("127.0.0.1", universe15bit);
    // or you can set net and subnet here
    // artnet.streamArtDmxTo("127.0.0.1", net, subnet, universe);
}
```

### Artnet (Integrated Sender/Receiver)

`ArtNet` class has `ArtNetReceiver` and `ArtNetSender` features.

```C++
#include <Artnet.h>
Artnet artnet;

void setup()
{
    // setup Ethernet/WiFi...

    artnet.begin(); // send to localhost and listen to default port

    artnet.subscribeArtDmxUniverse(universe, [&](const uint8_t *data, uint16_t size, const ArtDmxMetadata &metadata, const ArtNetRemoteInfo &remote) {
        // if Artnet packet comes to this universe, this function is called
    });
    artnet.subscribeArtDmx([&](const uint8_t *data, uint16_t size, const ArtDmxMetadata& metadata, const ArtNetRemoteInfo& remote) {
        // if Artnet packet comes, this function is called for every universe
    });
}

void loop() {
    artnet.parse(); // check if artnet packet has come and execute callback

    // change send data as you want

    artnet.send("127.0.0.1", universe, data_ptr, size); // one-line send

    artnet.streaming_data(data_ptr, size);
    artnet.streaming("127.0.0.1", universe); // stream in 40 fps
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
    artnet.forwardArtDmxDataToFastLED(universe, leds, NUM_LEDS);

    // this can be achieved manually as follows
    // artnet.subscribeArtDmxUniverse(universe, [](const uint8_t *data, uint16_t size, const ArtDmxMetadata& metadata, const ArtNetRemoteInfo& remote)
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

## Other Configurations

### Subscribing Callbacks with Net, Sub-Net and Universe as you like

- The relationship of Net (0-127), Sub-Net (0-15), 4-bit Universe (0-15) and 15-bit Universe (0-32767) is `universe15bit = (net << 8) | (subnet << 4) | universe4bit`
- You can subscribe ArtDmx data for Net (0-127), Sub-Net (0-15), and 4-bit Universe (0-15) like `artnet.subscribeArtDmxUniverse(net, subnet, universe, callback)`
- Or you can use 15-bit Universe (0-32767) can be set lnke `artnet.subscribeArtDmxUniverse(universe, callback)`
- Subscribed universes (targets of the callbacks) are automatically reflected to `net_sw` `sub_sw` `sw_out` in `ArtPollreply`

### ArtPollReply Configuration

- This library supports `ArtPoll` and `ArtPollReply`
- `ArtPoll` is automatically parsed and sends `ArtPollReply`
- You can configure the following information of by `setArtPollReplyConfig()`
- Other settings are set automatically based on registerd callbacks
- Please refer [here](https://art-net.org.uk/downloads/art-net.pdf) for more information

```C++
struct ArtPollReplyMetadata
{
    uint16_t oem {0x00FF};      // OemUnknown https://github.com/tobiasebsen/ArtNode/blob/master/src/Art-NetOemCodes.h
    uint16_t esta_man {0x0000}; // ESTA manufacturer code
    uint8_t status1 {0x00};     // Unknown / Normal
    uint8_t status2 {0x08};     // sACN capable
    String short_name {"Arduino ArtNet"};
    String long_name {"Ardino ArtNet Protocol by hideakitai/ArtNet"};
    String node_report {""};
    // Four universes from Device to Controller
    // NOTE: Only low 4 bits of the universes
    // NOTE: Upper 11 bits of the universes will be
    //       shared with the subscribed universe (net, subnet)
    // e.g.) If you have subscribed universe 0x1234,
    //       you can set the device to controller universes
    //       from 0x1230 to 0x123F (sw_in will be from 0x0 to 0xF).
    //       So, I recommned subscribing only in the range of
    //       0x1230 to 0x123F if you want to set the sw_in.
    // REF: Please refer the Art-Net spec for the detail.
    //      https://art-net.org.uk/downloads/art-net.pdf
    uint8_t sw_in[4] {0};
};
```

```c++
// set information for artpollreply individually
// https://art-net.org.uk/downloads/art-net.pdf
void setArtPollReplyConfig(const ArtPollReplyConfig &cfg);
void setArtPollReplyConfig(uint16_t oem, uint16_t esta_man, uint8_t status1, uint8_t status2, const String &short_name, const String &long_name, const String &node_report, uint8_t sw_in[4]);
void setArtPollReplyConfigOem(uint16_t oem);
void setArtPollReplyConfigEstaMan(uint16_t esta_man);
void setArtPollReplyConfigStatus1(uint8_t status1);
void setArtPollReplyConfigStatus2(uint8_t status2);
void setArtPollReplyConfigShortName(const String &short_name);
void setArtPollReplyConfigLongName(const String &long_name);
void setArtPollReplyConfigNodeReport(const String &node_report);
void setArtPollReplyConfigSwIn(size_t index, uint8_t sw_in);
void setArtPollReplyConfigSwIn(uint8_t sw_in[4]);
void setArtPollReplyConfigSwIn(uint8_t sw_in_0, uint8_t sw_in_1, uint8_t sw_in_2, uint8_t sw_in_3);
```

## ArtTrigger

You can send/subscribe `ArtTrigger` using the follwing APIs. Please refer [here](https://art-net.org.uk/how-it-works/time-keeping-triggering/arttrigger/) for more information.

```C++
void sendArtTrigger(const String& ip, uint16_t oem = 0, uint8_t key = 0, uint8_t subkey = 0, const uint8_t *payload = nullptr, uint16_t size = 512);
void subscribeArtTrigger(const ArtTriggerCallback &func);
using ArtTriggerCallback = std::function<void(const ArtTriggerMetadata &metadata, const ArtNetRemoteInfo &remote)>;
```

## ArtSync

You can send/subscribe `ArtSync` using the follwing APIs. Please refer [here](https://art-net.org.uk/how-it-works/time-keeping-triggering/arttimesync/) for more information.

```C++
void sendArtSync(const String& ip);
void subscribeArtSync(const ArtSyncCallback &func);
using ArtSyncCallback = std::function<void(const ArtNetRemoteInfo &remote)>;
```

## APIs

### ArtnetSender

```C++
// streaming artdmx packet
void setArtDmxData(const uint8_t* const data, uint16_t size);
void setArtDmxData(uint16_t ch, uint8_t data);
void streamArtDmxTo(const String& ip, uint16_t universe15bit);
void streamArtDmxTo(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe);
void streamArtDmxTo(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, uint8_t physical);
// streaming artnzs packet
void setArtNzsData(const uint8_t* const data, uint16_t size);
void setArtNzsData(uint16_t ch, uint8_t data);
void streamArtNzsTo(const String& ip, uint16_t universe15bit);
void streamArtNzsTo(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe);
void streamArtNzsTo(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, uint8_t start_code);
// one-line artdmx sender
void sendArtDmx(const String& ip, uint16_t universe15bit, const uint8_t* const data, uint16_t size);
void sendArtDmx(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, const uint8_t* const data, uint16_t size);
void sendArtDmx(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, uint8_t physical, const uint8_t *data, uint16_t size);
// one-line artnzs sender
void sendArtNzs(const String& ip, uint16_t universe15bit, const uint8_t* const data, uint16_t size);
void sendArtNzs(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, const uint8_t* const data, uint16_t size);
void sendArtNzs(const String& ip, uint8_t net, uint8_t subnet, uint8_t universe, uint8_t start_code, const uint8_t *data, uint16_t size);
// send other packets
void sendArtTrigger(const String& ip, uint16_t oem = 0, uint8_t key = 0, uint8_t subkey = 0, const uint8_t *payload = nullptr, uint16_t size = 512);
void sendArtSync(const String& ip);
```

### ArtnetReceiver

```C++
using ArtDmxCallback = std::function<void(const uint8_t *data, uint16_t size, const ArtDmxMetadata &metadata, const ArtNetRemoteInfo &remote)>;
using ArtSyncCallback = std::function<void(const ArtNetRemoteInfo &remote)>;
using ArtTriggerCallback = std::function<void(const ArtTriggerMetadata &metadata, const RemoteInfo &remote)>;
```

```C++
struct ArtNetRemoteInfo
{
    IPAddress ip;
    uint16_t port;
};

struct ArtDmxMetadata
{
    uint8_t sequence;
    uint8_t physical;
    uint8_t net;
    uint8_t subnet;
    uint8_t universe;
};
```

```C++
OpCode parse()
// subscribe artdmx packet for specified net, subnet, and universe
void subscribeArtDmxUniverse(uint8_t net, uint8_t subnet, uint8_t universe, const ArtDmxCallback &func);
// subscribe artdmx packet for specified universe (15 bit)
void subscribeArtDmxUniverse(uint16_t universe, const ArtDmxCallback &func);
// subscribe artnzs packet for specified universe (15 bit)
auto subscribeArtNzsUniverse(uint16_t universe, const ArtNzsCallback &func);
// subscribe artdmx packet for all universes
void subscribeArtDmx(const ArtDmxCallback &func);
// subscribe other packets
void subscribeArtSync(const ArtSyncCallback &func);
void subscribeArtTrigger(const ArtTriggerCallback &func);
// unsubscribe callbacks
void unsubscribeArtDmxUniverse(uint8_t net, uint8_t subnet, uint8_t universe);
void unsubscribeArtDmxUniverse(uint16_t universe);
void unsubscribeArtDmxUniverses();
void unsubscribeArtDmx();
void unsubscribeArtNzsUniverse(uint16_t universe);
void unsubscribeArtSync();
void unsubscribeArtTrigger();
// set artdmx data to CRGB (FastLED) directly
void forwardArtDmxDataToFastLED(uint8_t net, uint8_t subnet, uint8_t universe, CRGB* leds, uint16_t num);
void forwardArtDmxDataToFastLED(uint16_t universe, CRGB* leds, uint16_t num);
// set information for artpollreply individually
// https://art-net.org.uk/downloads/art-net.pdf
void setArtPollReplyConfig(const ArtPollReplyConfig &cfg);
void setArtPollReplyConfig(uint16_t oem, uint16_t esta_man, uint8_t status1, uint8_t status2, const String &short_name, const String &long_name, const String &node_report, uint8_t sw_in[4]);
void setArtPollReplyConfigOem(uint16_t oem);
void setArtPollReplyConfigEstaMan(uint16_t esta_man);
void setArtPollReplyConfigStatus1(uint8_t status1);
void setArtPollReplyConfigStatus2(uint8_t status2);
void setArtPollReplyConfigShortName(const String &short_name);
void setArtPollReplyConfigLongName(const String &long_name);
void setArtPollReplyConfigNodeReport(const String &node_report);
void setArtPollReplyConfigSwIn(size_t index, uint8_t sw_in);
void setArtPollReplyConfigSwIn(uint8_t sw_in[4]);
void setArtPollReplyConfigSwIn(uint8_t sw_in_0, uint8_t sw_in_1, uint8_t sw_in_2, uint8_t sw_in_3);
// Set where debug output should go (e.g. setLogger(&Serial); default is nowhere)
void setLogger(Print*);
```

### Note

Some boards without enough memory (e.g. Uno, Nano, etc.) may not be able to use integrated sender/receiver because of the lack of enough memory. Please consider to use more powerful board or to use only sender OR receiver.

## Reference

- [Spec (Art-Net 4)](http://artisticlicence.com/WebSiteMaster/User%20Guides/art-net.pdf)
- [Packet Summary](https://art-net.org.uk/structure/packet-summary-2/)
- [Packet Definition](https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/)

## Dependent Libraries

- [ArxContainer](https://github.com/hideakitai/ArxContainer)
- [ArxTypeTraits](https://github.com/hideakitai/ArxTypeTraits)

## Embedded Libraries

- [TeensyDirtySTLErrorSolution v0.1.0](https://github.com/hideakitai/TeensyDirtySTLErrorSolution)

## License

MIT
