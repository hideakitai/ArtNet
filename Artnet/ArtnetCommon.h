#pragma once
#ifndef ARTNET_COMMON_H
#define ARTNET_COMMON_H

namespace arx {
namespace artnet {
    // Packet Summary : https://art-net.org.uk/structure/packet-summary-2/
    // Packet Definition : https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/

    enum class OpCode : uint16_t {
        // Device Discovery
        Poll = 0x2000,
        PollReply = 0x2100,
        // Device Configuration
        Address = 0x6000,
        Input = 0x7000,
        IpProg = 0xF800,
        IpProgReply = 0xF900,
        Command = 0x2400,
        // Streaming Control
        Dmx = 0x5000,
        Nzs = 0x5100,
        Sync = 0x5200,
        // RDM
        TodRequest = 0x8000,
        TodData = 0x8100,
        TodControl = 0x8200,
        Rdm = 0x8300,
        RdmSub = 0x8400,
        // Time-Keeping
        TimeCode = 0x9700,
        TimeSync = 0x9800,
        // Triggering
        Trigger = 0x9900,
        // Diagnostics
        DiagData = 0x2300,
        // File Transfer
        FirmwareMaster = 0xF200,
        FirmwareReply = 0xF300,
        Directory = 0x9A00,
        DirectoryReply = 0x9B00,
        FileTnMaster = 0xF400,
        FileFnMaster = 0xF500,
        FileFnReply = 0xF600,
        // N/A
        NA = 0x0000,
    };

    constexpr uint16_t OPC(OpCode op) {
        return static_cast<uint16_t>(op);
    }

    enum class Index : uint16_t {
        ID = 0,
        OP_CODE_L = 8,
        OP_CODE_H = 9,
        PROTOCOL_VER_H = 10,
        PROTOCOL_VER_L = 11,
        SEQUENCE = 12,
        PHYSICAL = 13,
        SUBUNI = 14,
        NET = 15,
        LENGTH_H = 16,
        LENGTH_L = 17,
        DATA = 18
    };

    constexpr uint16_t IDX(Index i) {
        return static_cast<uint16_t>(i);
    }

    constexpr uint16_t DEFAULT_PORT {6454};  // 0x1936
    constexpr uint16_t HEADER_SIZE {18};
    constexpr uint16_t PACKET_SIZE {530};
    constexpr uint16_t PROTOCOL_VER {0x0014};
    constexpr uint8_t ID_LENGTH {8};
    constexpr char ID[ID_LENGTH] {"Art-Net"};
    constexpr float DEFAULT_FPS {40.};
    constexpr uint32_t DEFAULT_INTERVAL_MS {(uint32_t)(1000. / DEFAULT_FPS)};

    static constexpr uint8_t NUM_PIXELS_PER_UNIV {170};

    using CallbackAllType = std::function<void(const uint32_t universe, const uint8_t* data, const uint16_t size)>;
    using CallbackType = std::function<void(const uint8_t* data, const uint16_t size)>;

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
    template <uint16_t SIZE>
    using Array = std::array<uint8_t, SIZE>;
    using IntervalMap = std::map<uint32_t, uint32_t>;
    using CallbackMap = std::map<uint32_t, CallbackType>;
    using namespace std;
#else
    template <uint16_t SIZE>
    using Array = arx::vector<uint8_t, SIZE>;
    using IntervalMap = arx::map<uint32_t, uint32_t>;
    using CallbackMap = arx::map<uint32_t, CallbackType, 4>;
    using namespace arx;
#endif

    union ArtPollReply {
        struct {
            uint8_t id[8];
            uint8_t op_code_l;
            uint8_t op_code_h;
            uint8_t ip[4];
            uint8_t port_l;
            uint8_t port_h;
            uint8_t ver_h;
            uint8_t ver_l;
            uint8_t net_sw;
            uint8_t sub_sw;
            uint8_t oem_h;
            uint8_t oem_l;
            uint8_t ubea_ver;
            uint8_t status_1;
            uint8_t esta_man_l;
            uint8_t esta_man_h;
            uint8_t short_name[18];
            uint8_t long_name[64];
            uint8_t node_report[64];
            uint8_t num_ports_h;
            uint8_t num_ports_l;
            uint8_t port_types[4];
            uint8_t good_input[4];
            uint8_t good_output[4];
            uint8_t sw_in[4];
            uint8_t sw_out[4];
            uint8_t sw_video;
            uint8_t sw_macro;
            uint8_t sw_remote;
            uint8_t spare[3];
            uint8_t style;
            uint8_t mac[6];
            uint8_t bind_ip[4];
            uint8_t bind_index;
            uint8_t status_2;
            uint8_t filler[26];
        };
        uint8_t b[239];
    };

    template <typename S>
    class Sender_ {
        Array<PACKET_SIZE> packet;
        uint8_t target_net {0};
        uint8_t target_subnet {0};
        uint8_t target_universe {0};
        uint8_t seq {0};
        uint8_t phy {0};

        IntervalMap intervals;
        S* stream;

    public:
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
#else
        Sender_() {
            packet.resize(PACKET_SIZE);
        }
#endif
        virtual ~Sender_() {}

        // streaming packet
        void streaming_data(const uint8_t* const data, const uint16_t size) {
            memcpy((&packet[IDX(Index::DATA)]), data, size);
        }
        void streaming_data(const uint16_t ch, const uint8_t data) {
            packet[IDX(Index::DATA) + ch] = data;
        }

        void streaming(const String& ip, const uint32_t universe_) {
            uint32_t now = millis();
            if (intervals.find(universe_) == intervals.end()) {
                intervals.insert(make_pair(universe_, now));
            }
            if (now >= intervals[universe_] + DEFAULT_INTERVAL_MS) {
                set_universe(universe_);
                send_packet(ip);
                intervals[universe_] = now;
            }
        }
        void streaming(const String& ip, const uint8_t net_, const uint8_t subnet_, const uint8_t universe_) {
            uint32_t u = ((uint32_t)net_ << 8) | ((uint32_t)subnet_ << 4) | (uint32_t)universe_;
            streaming(ip, u);
        }

        // one-line sender
        void send(const String& ip, const uint32_t universe_, const uint8_t* const data, const uint16_t size) {
            set_universe(universe_);
            streaming_data(data, size);
            send_packet(ip);
        }
        void send(
            const String& ip,
            const uint8_t net_,
            const uint8_t subnet_,
            const uint8_t universe_,
            const uint8_t* const data,
            const uint16_t size) {
            set_universe(net_, subnet_, universe_);
            streaming_data(data, size);
            send_packet(ip);
        }

        void physical(const uint8_t i) {
            phy = constrain(i, 0, 3);
        }

        uint8_t sequence() const {
            return seq;
        }

    protected:
        void attach(S& s) {
            stream = &s;
            for (size_t i = 0; i < ID_LENGTH; i++) packet[IDX(Index::ID) + i] = static_cast<uint8_t>(ID[i]);
            packet[IDX(Index::OP_CODE_H)] = (OPC(OpCode::Dmx) >> 8) & 0x00FF;
            packet[IDX(Index::OP_CODE_L)] = (OPC(OpCode::Dmx) >> 0) & 0x00FF;
            packet[IDX(Index::PROTOCOL_VER_H)] = (PROTOCOL_VER >> 8) & 0x00FF;
            packet[IDX(Index::PROTOCOL_VER_L)] = (PROTOCOL_VER >> 0) & 0x00FF;
        }

        void send_packet(const String& ip) {
            packet[IDX(Index::SEQUENCE)] = seq++;
            packet[IDX(Index::PHYSICAL)] = phy;
            packet[IDX(Index::NET)] = target_net;
            packet[IDX(Index::SUBUNI)] = (target_subnet << 4) | target_universe;
            packet[IDX(Index::LENGTH_H)] = (512 >> 8) & 0xFF;
            packet[IDX(Index::LENGTH_L)] = (512 >> 0) & 0xFF;
#ifdef ARTNET_ENABLE_WIFI
            if (WiFi.status() != WL_CONNECTED) return;
#endif
            stream->beginPacket(ip.c_str(), DEFAULT_PORT);
            stream->write(packet.data(), packet.size());
            stream->endPacket();
        }

        void set_universe(const uint32_t universe_) {
            target_net = (universe_ >> 8) & 0x7F;
            target_subnet = (universe_ >> 4) & 0x0F;
            target_universe = universe_ & 0x0F;
        }
        void set_universe(const uint8_t net_, const uint8_t subnet_, const uint8_t universe_) {
            target_net = net_ & 0x7F;
            target_subnet = subnet_ & 0x0F;
            target_universe = universe_ & 0x0F;
        }
    };

    template <typename S>
    class Receiver_ {
        Array<PACKET_SIZE> packet;
        IPAddress remote_ip;
        uint16_t remote_port;
        uint8_t net_switch;  // net of universe
        uint8_t sub_switch;  // subnet of universe
        String short_name {"Arduino ArtNet"};
        String long_name {"Ardino ArtNet Protocol by hideakitai/ArtNet"};
        String node_report {""};
        CallbackMap callbacks;
        CallbackAllType callback_all;
        S* stream;
        bool b_verbose {false};

    public:
#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11
#else
        Receiver_() {
            packet.resize(PACKET_SIZE);
        }
#endif

        virtual ~Receiver_() {}

        OpCode parse() {
#ifdef ARTNET_ENABLE_WIFI
            if (WiFi.status() != WL_CONNECTED) return OpCode::NA;
#endif
            const size_t size = stream->parsePacket();
            if (size == 0) return OpCode::NA;

            uint8_t* d = new uint8_t[size];
            stream->read(d, size);

            OpCode op_code = OpCode::NA;
            if (checkID(d)) {
                switch (opcode(d)) {
                    case OPC(OpCode::Dmx): {
                        memcpy(packet.data(), d, size);
                        remote_ip = stream->S::remoteIP();
                        remote_port = (uint16_t)stream->S::remotePort();
                        if (callback_all) callback_all(universe15bit(), data(), size - HEADER_SIZE);
                        for (auto& c : callbacks)
                            if (universe15bit() == c.first) c.second(data(), size - HEADER_SIZE);
                        op_code = OpCode::Dmx;
                        break;
                    }
                    case OPC(OpCode::Poll): {
                        remote_ip = stream->S::remoteIP();
                        remote_port = (uint16_t)stream->S::remotePort();
                        poll_reply();
                        op_code = OpCode::Poll;
                        break;
                    }
                    default: {
                        if (b_verbose) {
                            Serial.print(F("Unsupported OpCode: "));
                            Serial.println(opcode(d), HEX);
                        }
                        op_code = OpCode::NA;
                        break;
                    }
                }
            }
            delete[] d;
            return op_code;
        }

        inline const IPAddress& ip() const {
            return remote_ip;
        }
        inline uint16_t port() const {
            return remote_port;
        }

        inline String id() const {
            String str;
            for (uint8_t i = 0; i < ID_LENGTH; ++i) str += packet[IDX(Index::ID) + i];
            return str;
        }
        inline uint16_t opcode() const {
            return (packet[IDX(Index::OP_CODE_H)] << 8) | packet[IDX(Index::OP_CODE_L)];
        }
        inline uint16_t opcode(const uint8_t* p) const {
            return (p[IDX(Index::OP_CODE_H)] << 8) | p[IDX(Index::OP_CODE_L)];
        }
        inline uint16_t version() const {
            return (packet[IDX(Index::PROTOCOL_VER_H)] << 8) | packet[IDX(Index::PROTOCOL_VER_L)];
        }
        inline uint8_t sequence() const {
            return packet[IDX(Index::SEQUENCE)];
        }
        inline uint8_t physical() const {
            return packet[IDX(Index::PHYSICAL)];
        }
        uint8_t net() const {
            return packet[IDX(Index::NET)] & 0x7F;
        }
        uint8_t subnet() const {
            return (packet[IDX(Index::SUBUNI)] >> 4) & 0x0F;
        }
        inline uint8_t universe() const {
            return packet[IDX(Index::SUBUNI)] & 0x0F;
        }
        inline uint16_t universe15bit() const {
            return (packet[IDX(Index::NET)] << 8) | packet[IDX(Index::SUBUNI)];
        }
        inline uint16_t length() const {
            return (packet[IDX(Index::LENGTH_H)] << 8) | packet[IDX(Index::LENGTH_L)];
        }
        inline uint16_t size() const {
            return length();
        }
        inline uint8_t* data() {
            return &(packet[HEADER_SIZE]);
        }
        inline uint8_t data(const uint16_t i) const {
            return packet[HEADER_SIZE + i];
        }

        template <typename Fn>
        inline auto subscribe(const uint8_t universe, Fn&& func) -> std::enable_if_t<arx::is_callable<Fn>::value> {
            if (callbacks.size() >= 4) {
                if (b_verbose) {
                    Serial.println(F("too many callbacks"));
                }
            } else {
                if (universe > 0xF) {
                    if (b_verbose) {
                        Serial.println(F("universe out of bounds"));
                    }
                    return;
                } else {
                    uint32_t u = ((uint32_t)net_switch << 8) | ((uint32_t)sub_switch << 4) | (uint32_t)universe;
                    callbacks.insert(make_pair(u, arx::function_traits<Fn>::cast(func)));
                }
            }
        }
        template <typename Fn>
        inline auto subscribe(const uint8_t universe, Fn* func) -> std::enable_if_t<arx::is_callable<Fn>::value> {
            if (callbacks.size() >= 4) {
                if (b_verbose) {
                    Serial.println(F("too many callbacks"));
                }
            } else {
                if (universe > 0xF) {
                    if (b_verbose) {
                        Serial.println(F("universe out of bounds"));
                    }
                } else {
                    uint32_t u = ((uint32_t)net_switch << 8) | ((uint32_t)sub_switch << 4) | (uint32_t)universe;
                    callbacks.insert(make_pair(u, arx::function_traits<Fn>::cast(func)));
                }
            }
        }
        template <typename Fn>
        inline auto subscribe15bit(const uint16_t universe, Fn&& func)
            -> std::enable_if_t<arx::is_callable<Fn>::value> {
            if (callbacks.size() >= 4) {
                if (b_verbose) {
                    Serial.println(F("too many callbacks"));
                }
            } else {
                callbacks.insert(make_pair(universe, arx::function_traits<Fn>::cast(func)));
            }
        }
        template <typename Fn>
        inline auto subscribe15bit(const uint16_t universe, Fn* func)
            -> std::enable_if_t<arx::is_callable<Fn>::value> {
            if (callbacks.size() >= 4) {
                if (b_verbose) {
                    Serial.println(F("too many callbacks"));
                }
            } else {
                callbacks.insert(make_pair(universe, arx::function_traits<Fn>::cast(func)));
            }
        }
        template <typename F>
        inline auto subscribe(F&& func) -> std::enable_if_t<arx::is_callable<F>::value> {
            callback_all = arx::function_traits<F>::cast(func);
        }
        template <typename F>
        inline auto subscribe(F* func) -> std::enable_if_t<arx::is_callable<F>::value> {
            callback_all = arx::function_traits<F>::cast(func);
        }

        inline void unsubscribe(const uint8_t universe) {
            auto it = callbacks.find(universe);
            if (it != callbacks.end()) callbacks.erase(it);
        }
        inline void unsubscribe() {
            callback_all = nullptr;
        }

        inline void clear_subscribers() {
            unsubscribe();
            callbacks.clear();
        }

#ifdef FASTLED_VERSION
        inline void forward(const uint8_t universe, CRGB* leds, const uint16_t num) {
            subscribe(universe, [&](const uint8_t* data, const uint16_t size) {
                if (size < num * 3) {
                    Serial.println(F("ERROR: Too many LEDs to forward"));
                }
                for (size_t pixel = 0; pixel < num; ++pixel) {
                    size_t idx = pixel * 3;
                    leds[pixel].r = data[idx + 0];
                    leds[pixel].g = data[idx + 1];
                    leds[pixel].b = data[idx + 2];
                }
            });
        }
#endif
        void shortname(const String& sn) {
            short_name = sn;
        }

        void longname(const String& ln) {
            long_name = ln;
        }

        void nodereport(const String& nr) {
            node_report = nr;
        }

        void verbose(const bool b) {
            b_verbose = b;
        }

    protected:
        void attach(S& s, const uint8_t subscribe_net = 0, const uint8_t subscribe_subnet = 0) {
            stream = &s;
            if (subscribe_net > 128) {
                if (b_verbose) {
                    Serial.println(F("Net must be < 128"));
                }
            } else {
                net_switch = subscribe_net;
            }
            if (subscribe_subnet > 16) {
                if (b_verbose) {
                    Serial.println(F("Subnet must be < 16"));
                }
            } else {
                sub_switch = subscribe_subnet;
            }
        }

    private:
        inline bool checkID() const {
            const char* idptr = reinterpret_cast<const char*>(packet.data());
            return !strcmp(ID, idptr);
        }
        inline bool checkID(const uint8_t* p) const {
            const char* idptr = reinterpret_cast<const char*>(p);
            return !strcmp(ID, idptr);
        }

        void poll_reply() {
            ArtPollReply r;
            for (size_t i = 0; i < ID_LENGTH; i++) r.id[i] = static_cast<uint8_t>(ID[i]);
            r.op_code_l = ((uint16_t)OpCode::PollReply >> 0) & 0x00FF;
            r.op_code_h = ((uint16_t)OpCode::PollReply >> 8) & 0x00FF;
            IPAddress my_ip = localIP();
            IPAddress my_subnet = subnetMask();
            macAddress(r.mac);
            for (size_t i = 0; i < 4; ++i) r.ip[i] = my_ip[i];
            r.port_l = (DEFAULT_PORT >> 0) & 0xFF;
            r.port_h = (DEFAULT_PORT >> 8) & 0xFF;
            r.ver_h = (PROTOCOL_VER >> 8) & 0x00FF;
            r.ver_l = (PROTOCOL_VER >> 0) & 0x00FF;
            r.oem_h = 0;        // https://github.com/tobiasebsen/ArtNode/blob/master/src/Art-NetOemCodes.h
            r.oem_l = 0xFF;     // OemUnknown
            r.ubea_ver = 0;     // UBEA not programmed
            r.status_1 = 0x00;  // Unknown / Normal
            r.esta_man_l = 0;   // No EATA manufacture code
            r.esta_man_h = 0;   // No ESTA manufacture code
            memset(r.short_name, 0, 18);
            memset(r.long_name, 0, 64);
            memset(r.node_report, 0, 64);
            memcpy(r.short_name, short_name.c_str(), short_name.length());
            memcpy(r.long_name, long_name.c_str(), long_name.length());
            memcpy(r.node_report, node_report.c_str(), node_report.length());
            r.num_ports_h = 0;                 // Reserved
            r.num_ports_l = callbacks.size();  // This library implements only 4 port
            memset(r.sw_in, 0, 4);
            memset(r.sw_out, 0, 4);
            memset(r.port_types, 0, 4);
            memset(r.good_input, 0, 4);
            memset(r.good_output, 0, 4);
            r.net_sw = net_switch & 0x7F;
            r.sub_sw = sub_switch & 0x0F;
            size_t i = 0;
            for (const auto& pair : callbacks) {
                r.sw_in[i] = pair.first & 0x0F;
                r.sw_out[i] = i;          // dummy: output port is flexible
                r.port_types[i] = 0xC0;   // I/O available by DMX512
                r.good_input[i] = 0x80;   // Data received without error
                r.good_output[i] = 0x80;  // Data transmitted without error
                if (++i >= 4) break;
            }
            r.sw_video = 0;   // Video display shows local data
            r.sw_macro = 0;   // No support for macro key inputs
            r.sw_remote = 0;  // No support for remote trigger inputs
            memset(r.spare, 0x00, 3);
            r.style = 0x00;  // A DMX to / from Art-Net device
            for (size_t i = 0; i < 4; ++i) r.bind_ip[i] = my_ip[i];
            r.bind_index = 0;
            r.status_2 = 0x08;  // sACN capable
            memset(r.filler, 0x00, 26);

            static const IPAddress local_broadcast_addr = IPAddress((uint32_t)my_ip | ~(uint32_t)my_subnet);
            stream->beginPacket(local_broadcast_addr, DEFAULT_PORT);
            stream->write(r.b, sizeof(ArtPollReply));
            stream->endPacket();
        }

#ifdef ARTNET_ENABLE_WIFI
        template <typename T = S>
        auto localIP() -> std::enable_if_t<std::is_same<T, WiFiUDP>::value, IPAddress> {
            return WiFi.localIP();
        }
        template <typename T = S>
        auto subnetMask() -> std::enable_if_t<std::is_same<T, WiFiUDP>::value, IPAddress> {
            return WiFi.subnetMask();
        }
        template <typename T = S>
        auto macAddress(uint8_t* mac) -> std::enable_if_t<std::is_same<T, WiFiUDP>::value> {
            WiFi.macAddress(mac);
        }
#endif  // ARTNET_ENABLE_WIFI

#ifdef ARTNET_ENABLE_ETHER
        template <typename T = S>
        auto localIP() -> std::enable_if_t<std::is_same<T, EthernetUDP>::value, IPAddress> {
            return Ethernet.localIP();
        }
        template <typename T = S>
        auto subnetMask() -> std::enable_if_t<std::is_same<T, EthernetUDP>::value, IPAddress> {
            return Ethernet.subnetMask();
        }
        template <typename T = S>
        auto macAddress(uint8_t* mac) -> std::enable_if_t<std::is_same<T, EthernetUDP>::value> {
#ifndef ESP8266
            Ethernet.MACAddress(mac);
#endif
        }
#endif  // ARTNET_ENABLE_ETHER
    };  // namespace artnet

    template <typename S>
    class Manager : public Sender_<S>, public Receiver_<S> {
        S stream;

    public:
        void begin(
            const uint8_t subscribe_net = 0,
            const uint8_t subscribe_subnet = 0,
            const uint16_t recv_port = DEFAULT_PORT) {
            stream.begin(recv_port);
            this->Sender_<S>::attach(stream);
            this->Receiver_<S>::attach(stream, subscribe_net, subscribe_subnet);
        }

        void parse() {
            this->Receiver_<S>::parse();
        }
    };

    template <typename S>
    class Sender : public Sender_<S> {
        S stream;

    public:
        void begin() {
            stream.begin(DEFAULT_PORT);
            this->Sender_<S>::attach(stream);
        }
    };

    template <typename S>
    class Receiver : public Receiver_<S> {
        S stream;

    public:
        void begin(
            const uint8_t subscribe_net = 0,
            const uint8_t subscribe_subnet = 0,
            const uint16_t recv_port = DEFAULT_PORT) {
            stream.begin(recv_port);
            this->Receiver_<S>::attach(stream, subscribe_net, subscribe_subnet);
        }
    };
}  // namespace artnet
}  // namespace arx

#endif  // ARTNET_COMMON_H
