#include "galenaUUIDTag.h"
#include <ns3/uinteger.h>

#include <vector>


using namespace ns3;


namespace galena{
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */

    TypeId
    uuidTag::GetTypeId (void)
    {
    static TypeId tid = TypeId ("ns3::uuidTag")
        .SetParent<Tag> ()
        .AddConstructor<uuidTag> ()
        .AddAttribute ("UUIDStringValue",
                    "A uuid string value",
                    EmptyAttributeValue (),
                    MakeStringAccessor(&uuidTag::GetSimpleValue),
                    MakeStringChecker()
                    )
    ;
    return tid;
    }

    TypeId
    uuidTag::GetInstanceTypeId (void) const
    {
    return GetTypeId ();
    }

    uint32_t
    uuidTag::GetSerializedSize (void) const
    {
        return 36;
    }

    void
    uuidTag::Serialize (TagBuffer i) const
    {
        i.Write((uint8_t*) this->m_uuidStringValue.c_str(), this->GetSerializedSize());
    }

    void
    uuidTag::Deserialize (TagBuffer i)
    {
        uint8_t buffer[37]{'\0'};
        i.Read(buffer, this->GetSerializedSize());
        this->m_uuidStringValue = std::string((char*)buffer);
        
        boost::uuids::string_generator gen;
        this->uuid = gen(this->m_uuidStringValue);
    }

    void
    uuidTag::Print (std::ostream &os) const
    {
        std::cout << "Tag " << m_uuidStringValue << std::endl;
    }

    /**
     * Set the tag value
     * \param value The tag value.
     */
    void
    uuidTag::SetSimpleValue (std::string value)
    {
        this->m_uuidStringValue = value;

        boost::uuids::string_generator gen;
        this->uuid = gen(this->m_uuidStringValue);
    }

    /**
     * Get the tag value
     * \return the tag value.
     */

    std::string
    uuidTag::GetSimpleValue (void) const
    {
        return  m_uuidStringValue;
    }

    /**
     * Set the tag value as uuid value
     * \param value The tag value.
     */
    void
    uuidTag::SetUuidValue (boost::uuids::uuid value)
    {
        this->uuid = value;
        this->m_uuidStringValue = to_string(uuid);
    }

    /**
     * Get the tag value
     * \return the tag value.
     */

    boost::uuids::uuid
    uuidTag::GetUuidValue (void) const
    {
        return  uuid;
    }
}