#pragma once
#ifndef ARTNET_RECEIVER_TRAITS_H
#define ARTNET_RECEIVER_TRAITS_H

namespace art_net {

template <typename T> struct LocalIP;
template <typename T> struct SubnetMask;
template <typename T> struct MacAddress;

template <typename T>
IPAddress localIP(T&& x) {
    return LocalIP<std::decay_t<T>>::localIP(std::forward<T>(x));
}
template <typename T>
IPAddress subnetMask(T&& x) {
    return SubnetMask<std::decay_t<T>>::subnetMask(std::forward<T>(x));
}
template <typename T>
void macAddress(T&& x, uint8_t mac[6]) {
    MacAddress<std::decay_t<T>>::macAddress(std::forward<T>(x), mac);
}

template <typename T>
IPAddress getLocalIP();
template <typename T>
IPAddress getSubnetMask();
template <typename T>
void getMacAddress(uint8_t mac[6]);

} // namespace art_net

#endif // ARTNET_RECEIVER_TRAITS_H
