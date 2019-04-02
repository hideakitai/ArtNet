# ArtNet

Art-Net Sender/Receiver for Arduino (Ethernet, WiFi)

## Feature

- support Art-Net with both Ethernet and WiFi
- register multiple callbacks depending on universe
- flexible net/subnet/universe setting
- etc.

### Supported Platforms

- ESP32, ESP8266 (WiFi)
- Teensy 3.x (Ethernet)
- Arduino Uno/Mega etc. (Ethernet)

#### Note

Some boards without enough memory may not be able to use integrated sender/receiver.
Please use only sender OR receiver.

## Usage

This library has following Art-Net controller options.
Please use them depending on the situation.

- ArtnetSender
- ArtnetReveiver
- Artnet (Integrated Sender/Receiver)

### Art-Net Sender

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

### Art-Net Receiver

```C++
#include <Artnet.h>

// declarations for Ethernet/WiFi

ArtnetSender artnet;

void callback(uint8_t* data, uint16_t size)
{
    // you can also use pre-defined callbacks
}

void setup()
{
    // setup Ethernet/WiFi...

    artnet.begin(); // waiting for Art-Net in default port

    // if Artnet packet comes to this universe, this function is called
    artnet.subscribe(universe1, [](uint8_t* data, uint16_t size)
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

### Art-Net Sender

```C++
#include <Artnet.h>

// declarations for Ethernet/WiFi

Artnet artnet;

void setup()
{
    // setup Ethernet/WiFi...

    artnet.begin("127.0.0.1"); // send to localhost and listen to default port

    // if Artnet packet comes to this universe, this function is called
    artnet.subscribe(universe, [&](uint8_t* data, uint16_t size)
    {
        // do something with data coming to universe
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
void net(uint8_t n)
void subnet(uint8_t s)
void universe(uint8_t u)
void universe15bit(uint8_t u)
void set(const uint8_t* const data, uint16_t size = 512)
void set(const uint32_t universe_, const uint8_t* const data, uint16_t size = 512)
void set(const uint8_t net_, const uint8_t subnet_, const uint8_t universe_, const uint8_t* const data, uint16_t size = 512)
void send()
void send(const uint8_t* const data, uint16_t size = 512)
void send(const uint32_t universe_, const uint8_t* const data, uint16_t size = 512)
void send(const uint8_t net_, const uint8_t subnet_, const uint8_t universe_, const uint8_t* const data, uint16_t size = 512)
void streaming()
void physical(uint8_t i)
uint8_t sequence() const
```

### ArtnetReceiver

```C++
bool parse()
inline const IPAddress& ip() const { return remote_ip; }
uint16_t port() const { return remote_port; }
String id() const
uint16_t opcode() const
uint16_t opcode(const uint8_t* p) const
uint16_t version() const
uint8_t sequence() const
uint8_t physical() const
uint8_t net() const
uint8_t subnet() const
uint8_t universe() const
uint16_t universe15bit() const
uint16_t length() const
uint16_t size() const
uint8_t* data()
uint8_t data(const uint16_t i) const
void subscribe(const uint32_t universe, const CallbackType& func)
void subscribe(const CallbackType& func)
void subscribe(const uint8_t net, const uint8_t subnet, const uint8_t universe, const CallbackType& func)
```

## License

MIT
