#pragma once

#include "ns3/tag.h"
#include "ns3/type-id.h"
#include "ns3/ipv6-address.h"

#include <boost/uuid/uuid.hpp>

using namespace ns3;

namespace galena{
    class galenaSybil : public Tag
    {
        public:

        static TypeId 		    GetTypeId (void);
        virtual TypeId 		    GetInstanceTypeId (void) const;
        virtual uint32_t 		GetSerializedSize (void) const;
        virtual void 			Serialize (TagBuffer i) const;
        virtual void 			Deserialize (TagBuffer i);
        virtual void 			Print (std::ostream &os) const;
        void 					SetSimpleValue (Ipv6Address value);
        Ipv6Address 			GetSimpleValue (void) const;

        private:
        Ipv6Address 			m_simpleValue;  //!< tag value
    };
}