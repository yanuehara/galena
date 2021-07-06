#include "galenaSybil.h"
#include <ns3/uinteger.h>


using namespace ns3;

namespace galena{
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */

    TypeId
    galenaSybil::GetTypeId (void)
    {
    static TypeId tid = TypeId ("ns3::galenaSybil")
        .SetParent<Tag> ()
        .AddConstructor<galenaSybil> ()
        .AddAttribute ("SimpleValue",
                    "A simple value",
                    EmptyAttributeValue (),
                    MakeIpv6AddressAccessor (&galenaSybil::GetSimpleValue),
                    MakeIpv6AddressChecker())
    ;
    return tid;
    }

    TypeId
    galenaSybil::GetInstanceTypeId (void) const
    {
    return GetTypeId ();
    }

    uint32_t
    galenaSybil::GetSerializedSize (void) const
    {
        return 16;
    }

    void
    galenaSybil::Serialize (TagBuffer i) const
    {
        uint8_t buffer[16];
        m_simpleValue.GetBytes(buffer);

        i.Write(buffer, 16);
    }

    void
    galenaSybil::Deserialize (TagBuffer i)
    {
        uint8_t buffer[16];
        i.Read(buffer, 16);
        
        m_simpleValue = Ipv6Address(buffer);
    }

    void
    galenaSybil::Print (std::ostream &os) const
    {
        os << "v=" << m_simpleValue;
    }


    /**
     * Set the tag value
     * \param value The tag value.
     */
    void
    galenaSybil::SetSimpleValue (Ipv6Address value)
    {
        m_simpleValue = value;
    }


    /**
     * Get the tag value
     * \return the tag value.
     */

    Ipv6Address
    galenaSybil::GetSimpleValue (void) const
    {
    return m_simpleValue;
    }
}