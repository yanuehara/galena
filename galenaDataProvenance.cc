#include "ns3/ipv6-header.h"

#include "galenaDataProvenance.h"

namespace galena{
    int galena_hash_message(Ipv6Address from, Ipv6Address to, const Ptr< const Packet > pack, double rssi, hash_t hashDest){
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

    int sign_message(Ipv6Address from, Ipv6Address to, const Ptr< const Packet > pack, double rssi){
        return galena_hash_message(from, to, pack, rssi, hmacs[{from, to}]);
    }

    bool verify_message(Ipv6Address from, Ipv6Address to, const Ptr< const Packet > pack, double rssi){
        hash_t calcHash, messageHash;
        galena_hash_message(from, to, pack, rssi, calcHash);
        std::memcpy(messageHash, hmacs[{from, to}], sizeof(hash_t));

        return std::equal(std::begin(calcHash), std::end(calcHash), std::begin(messageHash));
    }
    
    Ipv6Header* GetPacketHeader(const Ptr< const Packet > packet){
        Ipv6Header* ipv6Header = nullptr;

        Ptr<Packet> q = packet->Copy();
        PacketMetadata::ItemIterator metadataIterator = q->BeginItem();
        PacketMetadata::Item item;

        while (metadataIterator.HasNext()){
            item = metadataIterator.Next();
            if(item.tid.GetName() == "ns3::Ipv6Header"){
                Callback<ObjectBase *> constr = item.tid.GetConstructor();
                NS_ASSERT(!constr.IsNull());
                // Ptr<> and DynamicCast<> won't work here as all headers are from ObjectBase, not Object
                ObjectBase *instance = constr();
                NS_ASSERT(instance != 0);
                ipv6Header = dynamic_cast<Ipv6Header*>(instance);
                NS_ASSERT(ipv6Header != 0);
                ipv6Header->Deserialize(item.current);
                break;
            }

        }

        return ipv6Header;
    }

    Ipv6Address GetPacketSource(const Ptr< const Packet > packet){
        Ipv6Address from;

        auto ipv6Header = GetPacketHeader(packet);
        from = ipv6Header->GetSourceAddress();

        return from;
    }

    Ipv6Address GetPacketDestination(const Ptr< const Packet > packet){
        Ipv6Address to;

        auto ipv6Header = GetPacketHeader(packet);
        to = ipv6Header->GetDestinationAddress();

        return to;
    }
}