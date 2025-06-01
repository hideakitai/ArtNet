#include <ArtnetWiFi.h>

// WiFi settings
const char* ssid = "your-ssid";
const char* password = "your-password";

// Art-Net settings
ArtnetWiFi artnet;
const String broadcast_ip = "192.168.1.255";

// RDM Controller state
struct RdmDevice {
    uint8_t uid[6];
    uint16_t address;
    bool active;
};

RdmDevice discovered_devices[32];
uint8_t device_count = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("WiFi connected, IP: ");
    Serial.println(WiFi.localIP());

    // Start Art-Net
    artnet.begin();

    // Subscribe to TOD Data responses
    artnet.subscribeArtTodData([](const uint8_t* uids, const uint8_t uid_count, const art_net::art_tod_data::Metadata& metadata, const art_net::RemoteInfo& remote) {
        Serial.println("=== TOD Data Received ===");
        Serial.print("From: ");
        Serial.print(remote.ip);
        Serial.print(" Port: ");
        Serial.println(metadata.port);
        Serial.print("Address: ");
        Serial.println(metadata.address);
        Serial.print("Total UIDs: ");
        Serial.println(metadata.uid_total);
        Serial.print("UIDs in this packet: ");
        Serial.println(uid_count);

        // Process discovered UIDs
        for (int i = 0; i < uid_count; i++) {
            Serial.print("  UID ");
            Serial.print(i);
            Serial.print(": ");
            for (int j = 0; j < 6; j++) {
                Serial.print(uids[i * 6 + j], HEX);
                if (j < 5) Serial.print(":");
            }
            Serial.println();

            // Store discovered device
            if (device_count < 32) {
                memcpy(discovered_devices[device_count].uid, &uids[i * 6], 6);
                discovered_devices[device_count].address = metadata.address;
                discovered_devices[device_count].active = true;
                device_count++;
            }
        }
    });

    // Subscribe to RDM responses
    artnet.subscribeArtRdm([](const uint8_t* data, const uint16_t size, const art_net::art_rdm::Metadata& metadata, const art_net::RemoteInfo& remote) {
        Serial.println("=== RDM Response Received ===");
        Serial.print("From: ");
        Serial.println(remote.ip);
        Serial.print("Size: ");
        Serial.println(size);

        // Parse RDM response (simplified)
        if (size >= 24) {  // Minimum RDM packet size
            uint8_t start_code = data[0];
            uint8_t sub_start_code = data[1];
            uint8_t message_length = data[2];

            Serial.print("Start Code: 0x");
            Serial.println(start_code, HEX);
            Serial.print("Sub-Start Code: 0x");
            Serial.println(sub_start_code, HEX);
            Serial.print("Message Length: ");
            Serial.println(message_length);

            // Extract UIDs
            Serial.print("Destination UID: ");
            for (int i = 3; i < 9; i++) {
                Serial.print(data[i], HEX);
                if (i < 8) Serial.print(":");
            }
            Serial.println();

            Serial.print("Source UID: ");
            for (int i = 9; i < 15; i++) {
                Serial.print(data[i], HEX);
                if (i < 14) Serial.print(":");
            }
            Serial.println();

            // Extract command info
            uint8_t response_type = data[16];
            uint16_t param_id = (data[21] << 8) | data[22];

            Serial.print("Response Type: 0x");
            Serial.println(response_type, HEX);
            Serial.print("Parameter ID: 0x");
            Serial.println(param_id, HEX);
        }
    });

    Serial.println("Art-Net RDM Controller Started");
    Serial.println("Commands:");
    Serial.println("  'd' - Start discovery");
    Serial.println("  'l' - List discovered devices");
    Serial.println("  'i' - Get device info from first device");
    Serial.println("  'f' - Flush TOD");
}

void loop() {
    // Parse incoming Art-Net packets
    artnet.parse();

    // Handle serial commands
    if (Serial.available()) {
        char cmd = Serial.read();
        switch (cmd) {
            case 'd':
                startDiscovery();
                break;
            case 'l':
                listDevices();
                break;
            case 'i':
                getDeviceInfo();
                break;
            case 'f':
                flushTod();
                break;
        }
    }
}

void startDiscovery() {
    Serial.println("\nStarting RDM discovery...");
    device_count = 0;  // Clear device list

    // Send TOD request for all addresses on net 0
    uint16_t addresses[] = {0x0000, 0x0001, 0x0002, 0x0003};  // First 4 addresses
    artnet.sendArtTodRequest(
        broadcast_ip,
        0x00,  // Net
        0x00,  // Command (TodFull)
        addresses,
        4      // Request 4 addresses
    );
}

void listDevices() {
    Serial.println("\nDiscovered RDM Devices:");
    if (device_count == 0) {
        Serial.println("  No devices found");
        return;
    }

    for (int i = 0; i < device_count; i++) {
        Serial.print("  Device ");
        Serial.print(i);
        Serial.print(" - UID: ");
        for (int j = 0; j < 6; j++) {
            Serial.print(discovered_devices[i].uid[j], HEX);
            if (j < 5) Serial.print(":");
        }
        Serial.print(" Address: ");
        Serial.println(discovered_devices[i].address);
    }
}

void getDeviceInfo() {
    if (device_count == 0) {
        Serial.println("\nNo devices discovered");
        return;
    }

    Serial.println("\nGetting device info from first device...");

    // Build RDM GET DEVICE_INFO packet
    uint8_t rdm_packet[24] = {
        0xCC,  // Start code
        0x01,  // Sub-start code (RDM)
        0x18,  // Message length (24 bytes)
    };

    // Destination UID (first discovered device)
    memcpy(&rdm_packet[3], discovered_devices[0].uid, 6);

    // Source UID (controller - use MAC address or fixed UID)
    uint8_t controller_uid[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    memcpy(&rdm_packet[9], controller_uid, 6);

    rdm_packet[15] = 0x01;  // Transaction number
    rdm_packet[16] = 0x01;  // Port ID / Response type
    rdm_packet[17] = 0x00;  // Message count
    rdm_packet[18] = 0x00;  // Sub-device MSB
    rdm_packet[19] = 0x00;  // Sub-device LSB
    rdm_packet[20] = 0x20;  // Command class (GET_COMMAND)
    rdm_packet[21] = 0x00;  // Parameter ID MSB (DEVICE_INFO)
    rdm_packet[22] = 0x60;  // Parameter ID LSB
    rdm_packet[23] = 0x00;  // Parameter data length

    // Send RDM packet
    artnet.sendArtRdm(
        broadcast_ip,
        0x00,  // Net
        discovered_devices[0].address,
        rdm_packet,
        sizeof(rdm_packet)
    );
}

void flushTod() {
    Serial.println("\nFlushing TOD...");

    // Send TOD Control flush command
    artnet.sendArtTodControl(
        broadcast_ip,
        0x00,   // Net
        0x01,   // Command (AtcFlush)
        0xFFFF  // All addresses
    );
}
