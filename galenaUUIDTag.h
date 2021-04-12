#pragma once

#include "ns3/tag.h"
#include "ns3/type-id.h"
#include "ns3/string.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_serialize.hpp>
#include <boost/uuid/uuid_generators.hpp>

using namespace ns3;

namespace galena{
    class uuidTag : public Tag
    {
    public:

        static TypeId 		GetTypeId (void);
        virtual TypeId 		GetInstanceTypeId (void) const;
        virtual uint32_t 	GetSerializedSize (void) const;
        virtual void 		Serialize (TagBuffer i) const;
        virtual void 		Deserialize (TagBuffer i);
        virtual void 		Print (std::ostream &os) const;
        void 				SetSimpleValue (std::string value);
        std::string 		GetSimpleValue (void) const;

        void 				SetUuidValue (boost::uuids::uuid value);
        boost::uuids::uuid	GetUuidValue (void) const;

    private:
        std::string 		m_uuidStringValue;  //!< tag value
        boost::uuids::uuid  uuid;
    };
}