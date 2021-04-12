#include "galenaTag.h"
#include <ns3/uinteger.h>


using namespace ns3;

namespace galena{
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */

    TypeId
    galenaTag::GetTypeId (void)
    {
    static TypeId tid = TypeId ("ns3::galenaTag")
        .SetParent<Tag> ()
        .AddConstructor<galenaTag> ()
        .AddAttribute ("SimpleValue",
                    "A simple value",
                    EmptyAttributeValue (),
                    MakeUintegerAccessor (&galenaTag::GetSimpleValue),
                    MakeUintegerChecker<uint8_t> ())
    ;
    return tid;
    }

    TypeId
    galenaTag::GetInstanceTypeId (void) const
    {
    return GetTypeId ();
    }

    uint32_t
    galenaTag::GetSerializedSize (void) const
    {
    return 1;
    }

    void
    galenaTag::Serialize (TagBuffer i) const
    {
    i.WriteU8 (m_simpleValue);
    }

    void
    galenaTag::Deserialize (TagBuffer i)
    {
    m_simpleValue = i.ReadU8 ();
    }

    void
    galenaTag::Print (std::ostream &os) const
    {
    //os << "v=" << (uint32_t)m_simpleValue;
    std::cout << "Tag " << (uint32_t)m_simpleValue << std::endl;
    }


    /**
     * Set the tag value
     * \param value The tag value.
     */
    void
    galenaTag::SetSimpleValue (uint8_t value)
    {
    m_simpleValue = value;
    }


    /**
     * Get the tag value
     * \return the tag value.
     */

    uint8_t
    galenaTag::GetSimpleValue (void) const
    {
    return m_simpleValue;
    }
}