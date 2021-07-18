#include "galenaDataProvenance.h"

namespace galena{
    int galena_hash_message(Ipv6Address from, Ipv6Address to, Ptr<Packet> pack, double rssi, hash_t hashDest){
        hash_t messageHash;
        uint8_t *message = new uint8_t[pack->GetSize()];
        pack->CopyData (message, pack->GetSize());

        key_t key{'\0'};
        from.Serialize(key);
        uint8_t u_rssi[] = {(uint8_t) rssi};
        *key = *key | *u_rssi;

        int ret = crypto_generichash(messageHash, sizeof messageHash,
                   message, pack->GetSize(),
                   key, sizeof key);

        std::memcpy(hashDest, messageHash, sizeof(hash_t));

        return ret;
    }

    int sign_message(Ipv6Address from, Ipv6Address to, Ptr<Packet> pack, double rssi){
        return galena_hash_message(from, to, pack, rssi, hmacs[{from, to}]);
    }

    bool verify_message(Ipv6Address from, Ipv6Address to, Ptr<Packet> pack, double rssi){
        hash_t calcHash, messageHash;
        galena_hash_message(from, to, pack, rssi, calcHash);
        std::memcpy(messageHash, hmacs[{from, to}], sizeof(hash_t));

        return std::equal(std::begin(calcHash), std::end(calcHash), std::begin(messageHash));
    }
}