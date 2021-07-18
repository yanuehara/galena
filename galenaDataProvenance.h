#pragma once

#include <map>
#include <algorithm>
#include <iterator>
#include <sodium.h>

#include "ns3/packet.h"
#include "ns3/ipv6-address.h"

using namespace ns3;
using namespace std;

namespace galena{
    using hash_t = unsigned char[crypto_generichash_BYTES];
    using key_t = unsigned char[crypto_generichash_KEYBYTES_MIN];

    static std::map<std::pair<Ipv6Address, Ipv6Address>, hash_t> hmacs;

    int galena_hash_message(Ipv6Address from, Ipv6Address to, Ptr<Packet> pack, double rssi, hash_t hashDest);
    int sign_message(Ipv6Address from, Ipv6Address to, Ptr<Packet> pack, double rssi);
    bool verify_message(Ipv6Address from, Ipv6Address to, Ptr<Packet> pack, double rssi);
}