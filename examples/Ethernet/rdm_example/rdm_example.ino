#include <ArtnetEther.h>

// Ethernet settings
uint8_t mac[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};  // Set your MAC address
IPAddress ip(192, 168, 1, 100);  // Set your IP address

// Art-Net settings
ArtnetEther artnet;
const String target_ip = "192.168.1.255";  // Broadcast address

// RDM Device UID (6 bytes)
uint8_t my_uid[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Initialize Ethernet
    Ethernet.begin(mac, ip);

    Serial.print("Ethernet connected, IP: ");
    Serial.println(Ethernet.localIP());

    // Start Art-Net
    artnet.begin();

    // Subscribe to RDM packets
    artnet.subscribeArtRdm([](const uint8_t* data, const uint16_t size, const art_net::art_rdm::Metadata& metadata, const art_net::RemoteInfo& remote) {
        Serial.println("=== ArtRdm Received ===");
        Serial.print("From: ");
        Serial.print(remote.ip);
        Serial.print(":");
        Serial.println(remote.port);
        Serial.print("Net: 0x");
        Serial.println(metadata.net, HEX);
        Serial.print("Address: ");
        Serial.println(metadata.address);
        Serial.print("Command: 0x");
        Serial.println(metadata.command, HEX);
        Serial.print("RDM Data Size: ");
        Serial.println(size);

        // Print first few bytes of RDM data
        Serial.print("RDM Data: ");
        for (int i = 0; i < min(size, 16); i++) {
            Serial.print(data[i], HEX);
            Serial.print(" ");
        }
        if (size > 16) Serial.print("...");
        Serial.println();
    });

    // Subscribe to TOD Request packets
    artnet.subscribeArtTodRequest([](const uint16_t* addresses, const uint8_t count, const art_net::art_tod_request::Metadata& metadata, const art_net::RemoteInfo& remote) {
        Serial.println("=== ArtTodRequest Received ===");
        Serial.print("From: ");
        Serial.print(remote.ip);
        Serial.print(":");
        Serial.println(remote.port);
        Serial.print("Net: 0x");
        Serial.println(metadata.net, HEX);
        Serial.print("Command: 0x");
        Serial.println(metadata.command, HEX);
        Serial.print("Address Count: ");
        Serial.println(count);

        // Print requested addresses
        for (int i = 0; i < count; i++) {
            Serial.print("  Address ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println(addresses[i]);
        }

        // Respond with TOD Data if our address is requested
        // In a real implementation, check if our address matches
        sendTodResponse(remote.ip, metadata.net, addresses[0]);
    });

    // Subscribe to TOD Control packets
    artnet.subscribeArtTodControl([](const art_net::art_tod_control::Metadata& metadata, const art_net::RemoteInfo& remote) {
        Serial.println("=== ArtTodControl Received ===");
        Serial.print("From: ");
        Serial.print(remote.ip);
        Serial.print(":");
        Serial.println(remote.port);
        Serial.print("Net: 0x");
        Serial.println(metadata.net, HEX);
        Serial.print("Command: 0x");
        Serial.println(metadata.command, HEX);
        Serial.print("Address: ");
        Serial.println(metadata.address);

        // Handle TOD control commands
        switch (metadata.command) {
            case 0x01:  // AtcFlush
                Serial.println("TOD Flush requested");
                break;
            default:
                Serial.println("Unknown TOD Control command");
                break;
        }
    });

    // Subscribe to RDM Sub packets
    artnet.subscribeArtRdmSub([](const uint8_t* data, const uint16_t size, const art_net::art_rdm_sub::Metadata& metadata, const art_net::RemoteInfo& remote) {
        Serial.println("=== ArtRdmSub Received ===");
        Serial.print("From: ");
        Serial.print(remote.ip);
        Serial.print(":");
        Serial.println(remote.port);
        Serial.print("UID: ");
        for (int i = 0; i < 6; i++) {
            Serial.print(metadata.uid[i], HEX);
            if (i < 5) Serial.print(":");
        }
        Serial.println();
        Serial.print("Command Class: 0x");
        Serial.println(metadata.command_class, HEX);
        Serial.print("Parameter ID: 0x");
        Serial.println(metadata.parameter_id, HEX);
        Serial.print("Sub Device: ");
        Serial.println(metadata.sub_device);
        Serial.print("Data Size: ");
        Serial.println(size);
    });

    Serial.println("Art-Net RDM Example Started (Ethernet)");
}

void loop() {
    // Parse incoming Art-Net packets
    artnet.parse();

    // Send a test RDM packet every 5 seconds
    static unsigned long last_send = 0;
    if (millis() - last_send > 5000) {
        last_send = millis();
        sendTestRdmPacket();
    }
}

void sendTestRdmPacket() {
    Serial.println("Sending test RDM packet...");

    // Example RDM packet data (simplified)
    uint8_t rdm_data[] = {
        0xCC,  // Start code
        0x01,  // Sub-start code
        0x18,  // Message length
        0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC,  // Destination UID
        0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC,  // Source UID
        0x00,  // Transaction number
        0x01,  // Port/Response type
        0x00,  // Message count
        0x00, 0x00,  // Sub-device
        0x20,  // Command class (GET_COMMAND)
        0x00, 0x60,  // Parameter ID (DEVICE_INFO)
        0x00   // Parameter data length
    };

    artnet.sendArtRdm(target_ip, 0x00, 0x0001, rdm_data, sizeof(rdm_data));
}

void sendTodResponse(IPAddress ip, uint8_t net, uint16_t address) {
    Serial.println("Sending TOD response...");

    // Send our UID in response to TOD request
    artnet.sendArtTodData(
        ip.toString(),  // Destination IP
        net,            // Net
        address,        // Address
        0,              // Port (0 for all ports)
        1,              // Total UIDs
        0,              // Block count (0 for single block)
        my_uid,         // Our UID
        1               // UID count in this packet
    );
}
