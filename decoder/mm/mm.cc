#include "mm.h"

using namespace Tetra;

/**
 * @brief Constructor
 *
 */

Mm::Mm(Log * log, Report * report) : Layer(log, report)
{

}

/**
 * @brief Destructor
 *
 */

Mm::~Mm()
{

}

/**
 * @brief MM service entry point - 16.9
 *
 */

void Mm::service(Pdu pdu, const MacLogicalChannel macLogicalChannel, TetraTime tetraTime, MacAddress macAddress)
{
    Layer::service(macLogicalChannel, tetraTime, macAddress);

    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - mac_channel = %s pdu = %s\n", "service_mm", macLogicalChannelName(macLogicalChannel).c_str(), pdu.toString().c_str());

    std::string txt = pdu.toString();

    uint32_t pos = 0;
    uint8_t pduType = pdu.getValue(pos, 4);

    // PDU type - 16.10.39 (EN 300 392-2) and A.8.59 (EN 300 392-7)

    switch (pduType)
    {
    case 0b0000:
        parseDOtar(pdu);
        break;

    case 0b0001:
        parseDAuthentication(pdu);
        break;

    case 0b0010:
        parseDCkChangeDemand(pdu);
        break;

    case 0b0011:
        parseDDisable(pdu);
        break;

    case 0b0100:
        parseDEnable(pdu);
        break;

    case 0b0101:
        parseDLocationUpdateAccept(pdu);
        break;

    case 0b0110:
        parseDLocationUpdateCommand(pdu);
        break;

    case 0b0111:
        parseDLocationUpdateReject(pdu);
        break;

    case 0b1001:
        parseDLocationUpdateProceeding(pdu);
        break;

    case 0b1010:
        parseDAttachDetachGroupIdentity(pdu);
        break;

    case 0b1011:
        parseDAttachDetachGroupIdentityAck(pdu);
        break;

    case 0b1100:
        parseDMmStatus(pdu);
        break;

    case 0b1111:
        parseMmPduNotSupported(pdu);
        break;

    default:                                                                    // reserved
        break;
    }
    
    printf("serv_mm_sub : TN/FN/MN = %2u/%2u/%2u  %-20s  len=%3lu \n", m_tetraTime.tn, m_tetraTime.fn, m_tetraTime.mn, txt.c_str(), pdu.size());
}


/**
 * @brief MM D-CK CHANGE DEMAND - A.4.1 (EN 300 392-7)
 *
 */

void Mm::parseDCkChangeDemand(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_ck_change_demand", pdu.toString().c_str());

    m_report->start("MM", "D-CK CHANGE DEMAND", m_tetraTime, m_macAddress);

    uint32_t pos = 4;                                                           // pdu type

    m_report->add("acknowledgement flag", pdu.getValue(pos, 1));
    pos += 1;

    m_report->add("change of security class", pdu.getValue(pos, 2));
    pos += 2;

    uint32_t keyChangeType = pdu.getValue(pos, 3);
    m_report->add("key change type", pdu.getValue(pos, 3));
    pos += 3;

    if (keyChangeType == 0)                                                     // SCK
    {
        uint32_t sckUse = pdu.getValue(pos, 1);
        m_report->add("sck use", pdu.getValue(pos, 1));
        pos += 1;

        uint32_t numberOfScksChanged = pdu.getValue(pos, 4);
        m_report->add("number of scks changed", pdu.getValue(pos, 4));
        pos += 4;

        if (sckUse == 1 && numberOfScksChanged == 0)                            // DMO
        {
            m_report->add("sck subset grouping type", pdu.getValue(pos, 4));
            pos += 4;
            m_report->add("sck subset number", pdu.getValue(pos, 5));
            pos += 5;
            m_report->add("sck-vn", pdu.getValue(pos, 16));
            pos += 16;
        }

        if (numberOfScksChanged != 0)
        {
            for (uint8_t cnt = 1; cnt <= numberOfScksChanged; cnt++)
            {
                m_report->add("sck data", pdu.getValue(pos, 21));
                pos += 21;
            }
        }
    }

    if (keyChangeType == 1 || keyChangeType == 3)                               // CCK or Class 3 CCK and GCK activation
    {
        m_report->add("cck-id", pdu.getValue(pos, 16));
        pos += 16;
    }

    if (keyChangeType == 2)                                                     // GCK
    {
        uint32_t numberOfGcksChanged = pdu.getValue(pos, 4);
        m_report->add("number of gcks changed", pdu.getValue(pos, 4));
        pos += 4;
        for (uint8_t cnt = 1; cnt <= numberOfGcksChanged; cnt++)
        {
            m_report->add("gck data", pdu.getValue(pos, 32));
            pos += 32;
        }
    }

    if (keyChangeType == 3 || keyChangeType == 4)                               // All GCKs or Class 3 CCK and GCK activation
    {
        m_report->add("gck-vn", pdu.getValue(pos, 16));
        pos += 16;
    }

    uint32_t timeType = pdu.getValue(pos, 2);
    m_report->add("time type", pdu.getValue(pos, 2));
    pos += 2;

    if (timeType == 0)                                                          // Absolute IV
    {
        m_report->add("slot number", pdu.getValue(pos, 2));
        pos += 2;

        m_report->add("frame number", pdu.getValue(pos, 5));
        pos += 5;

        m_report->add("multiframe number", pdu.getValue(pos, 6));
        pos += 6;

        m_report->add("hyperframe number", pdu.getValue(pos, 16));
        pos += 16;
    }

    if (timeType == 1)
    {
        m_report->add("network time", pdu.getValue(pos, 48));
        pos += 48;
    }

    m_report->send();
}


/**
 * @brief MM D-DISABLE - A.6.1 (EN 300 392-7)
 *
 */

void Mm::parseDDisable(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_disable", pdu.toString().c_str());

    m_report->start("MM", "D-DISABLE", m_tetraTime, m_macAddress);

    uint32_t pos = 4;                                                           // pdu type

    m_report->add("intent/confirm", pdu.getValue(pos, 1));
    pos += 1;

    m_report->add("disabling type", pdu.getValue(pos, 1));
    pos += 1;

    bool equipmentDisable = pdu.getValue(pos, 1);
    m_report->add("equipment disable", pdu.getValue(pos, 1));
    pos += 1;

    if (equipmentDisable)
    {
        m_report->add("tetra equipment identity", pdu.getValue(pos, 60));
        pos += 60;
    }

    bool subscriptionDisable = pdu.getValue(pos, 1);
    m_report->add("subscription disable", pdu.getValue(pos, 1));
    pos += 1;

    if (subscriptionDisable)
    {
        pos = parseAddressExtension(pdu, pos);
        m_report->add("ssi", pdu.getValue(pos, 24));
        pos += 24;
    }

    m_report->add("authentication challenge", pdu.getValue(pos, 160));
    pos += 160;

    m_report->send();
}

/**
 * @brief MM D-ENABLE - A.6.2 (EN 300 392-7)
 *
 */

void Mm::parseDEnable(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_enable", pdu.toString().c_str());

    m_report->start("MM", "D-ENABLE", m_tetraTime, m_macAddress);

    uint32_t pos = 4;                                                           // pdu type

    m_report->add("intent/confirm", pdu.getValue(pos, 1));
    pos += 1;

    bool equipmentEnable = pdu.getValue(pos, 1);
    m_report->add("equipment enable", pdu.getValue(pos, 1));
    pos += 1;

    if (equipmentEnable)
    {
        m_report->add("tetra equipment identity", pdu.getValue(pos, 60));
        pos += 60;
    }

    bool subscriptionEnable = pdu.getValue(pos, 1);
    m_report->add("subscription enable", pdu.getValue(pos, 1));
    pos += 1;

    if (subscriptionEnable)
    {
        pos = parseAddressExtension(pdu, pos);
        m_report->add("ssi", pdu.getValue(pos, 24));
        pos += 24;
    }

    m_report->add("authentication challenge", pdu.getValue(pos, 160));
    pos += 160;

    m_report->send();
}

/**
 * @brief MM D-LOCATION UPDATE ACCEPT - 16.9.2.7
 *
 */

void Mm::parseDLocationUpdateAccept(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_location_update_accept", pdu.toString().c_str());

    m_report->start("MM", "D-LOCATION UPDATE ACCEPT", m_tetraTime, m_macAddress);

    uint32_t pos = 4;                                                           // pdu type

    std::string txt = valueToString("location update accept type", pdu.getValue(pos, 3));
    m_report->add("location update accept type", pdu.getValue(pos, 3));
    m_report->add("location update accept type val", txt);
    pos += 3;

    // type 2 elements (Table E.11)

    bool oBit = pdu.getValue(pos, 1);                                           // o-bit
    pos += 1;
    if (oBit)                                                                   // there are type 2/3/4 elements
    {
        // each type 2 element has a p-bit

        bool pBit = pdu.getValue(pos, 1);                                       // p-bit for ssi element
        pos += 1;
        if (pBit)
        {
            m_report->add("ssi", pdu.getValue(pos, 24));
            pos += 24;
        }

        pBit = pdu.getValue(pos, 1);                                            // p-bit for address extension
        pos += 1;
        if (pBit)
        {
            pos = parseAddressExtension(pdu, pos);
        }

        pBit = pdu.getValue(pos, 1);                                            // p-bit for subscriber class
        pos += 1;
        if (pBit)
        {
            m_report->add("subscriber class", pdu.getValue(pos, 16));
            pos += 16;
        }

        pBit = pdu.getValue(pos, 1);                                            // p-bit for energy saving information
        pos += 1;
        if (pBit)
        {
            pos = parseEnergySavingInformation(pdu, pos);
        }

        pBit = pdu.getValue(pos, 1);                                            // p-bit for scch information and distribution
        pos += 1;
        if (pBit)
        {
            pos = parseScchInformationAndDistribution(pdu, pos);
        }

        bool mBit = pdu.getValue(pos, 1);                                       // type 3/4 elements

        if (mBit)
        {
            pos = parseType34Elements(pdu, pos);
        }
    }

    m_report->send();
}

/**
 * @brief MM D-LOCATION UPDATE COMMAND - 16.9.2.8
 *
 */

void Mm::parseDLocationUpdateCommand(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_location_update_command", pdu.toString().c_str());

    m_report->start("MM", "D-LOCATION UPDATE COMMAND", m_tetraTime, m_macAddress);

    uint32_t pos = 4;                                                           // pdu type

    m_report->add("group identity report", pdu.getValue(pos, 1));
    pos += 1;

    bool cipherControl = pdu.getValue(pos, 1);
    m_report->add("cipher control", pdu.getValue(pos, 1));
    pos += 1;

    if (cipherControl)
    {
        pos = parseCipheringParameters(pdu, pos);
    }

    pos = parseAddressExtension(pdu, pos);

    m_report->send();
}

/**
 * @brief MM D-LOCATION UPDATE REJECT - 16.9.2.9
 *
 */

void Mm::parseDLocationUpdateReject(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_location_update_reject", pdu.toString().c_str());

    m_report->start("MM", "D-LOCATION UPDATE REJECT", m_tetraTime, m_macAddress);

    uint32_t pos = 4;                                                           // pdu type

    std::string txt = valueToString("location update type", pdu.getValue(pos, 3));
    m_report->add("location update type", pdu.getValue(pos, 3));
    m_report->add("location update type val", txt);
    pos += 3;

    std::string rejectCauseTxt = valueToString("reject cause", pdu.getValue(pos, 5));
    m_report->add("reject cause", pdu.getValue(pos, 5));
    m_report->add("reject cause val", rejectCauseTxt);
    pos += 5;

    bool cipherControl = pdu.getValue(pos, 1);
    m_report->add("cipher control", pdu.getValue(pos, 1));
    pos += 1;

    if (cipherControl)
    {
        pos = parseCipheringParameters(pdu, pos);
    }

    pos = parseAddressExtension(pdu, pos);

    m_report->send();
}


/**
 * @brief MM D-LOCATION UPDATE PROCEEDING - 16.9.2.10
 *
 */

void Mm::parseDLocationUpdateProceeding(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_location_update_proceeding", pdu.toString().c_str());

    m_report->start("MM", "D-LOCATION UPDATE PROCEEDING", m_tetraTime, m_macAddress);

    uint32_t pos = 4;                                                           // pdu type

    m_report->add("ssi", pdu.getValue(pos, 24));
    pos += 24;

    pos = parseAddressExtension(pdu, pos);

    m_report->send();
}


/**
 * @brief MM D-ATTACH/DETACH GROUP IDENTITY - 16.9.2.1
 *
 */

void Mm::parseDAttachDetachGroupIdentity(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_attach_detach_group_identity", pdu.toString().c_str());

    m_report->start("MM", "D-ATTACH/DETACH GROUP IDENTITY", m_tetraTime, m_macAddress);

    uint32_t pos = 4;                                                           // pdu type

    m_report->add("group identity report", pdu.getValue(pos, 1));
    pos += 1;

    m_report->add("group identity acknowledgement request", pdu.getValue(pos, 1));
    pos += 1;

    m_report->add("group identity attach/detach mode", pdu.getValue(pos, 1));
    pos += 1;

    bool oBit = pdu.getValue(pos, 1);                                           // o-bit
    pos += 1;

    if (oBit)                                                                   // there are type 2/3/4 elements
    {
        bool mBit = pdu.getValue(pos, 1);                                       // type 3/4 elements

        if (mBit)
        {
            parseType34Elements(pdu, pos);
        }
    }

    m_report->send();
}

/**
 * @brief MM D-ATTACH/DETACH GROUP IDENTITY ACKNOWLEDGEMENT - 16.9.2.2
 *
 */

void Mm::parseDAttachDetachGroupIdentityAck(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_attach_detach_group_identity_ack", pdu.toString().c_str());

    m_report->start("MM", "D-ATTACH/DETACH GROUP IDENTITY ACKNOWLEDGEMENT", m_tetraTime, m_macAddress);

    uint32_t pos = 4;                                                           // pdu type

    m_report->add("group identity accept/reject", pdu.getValue(pos, 1));
    pos += 1;

    // reserved
    pos += 1;

    bool oBit = pdu.getValue(pos, 1);                                           // o-bit
    pos += 1;

    if (oBit)                                                                   // there are type 2/3/4 elements
    {
        bool mBit = pdu.getValue(pos, 1);                                       // type 3/4 elements

        if (mBit)
        {
            parseType34Elements(pdu, pos);
        }
    }

    m_report->send();
}

/**
 * @brief MM PDU/FUNCTION NOT SUPPORTED - 16.9.4.1
 *
 */

void Mm::parseMmPduNotSupported(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_mm_pdu_function_not_supported", pdu.toString().c_str());

    m_report->start("MM", "MM PDU/FUNCTION NOT SUPPORTED", m_tetraTime, m_macAddress);

    uint32_t pos = 4;

    std::string txt = valueToString("pdu type", pdu.getValue(pos, 4));
    m_report->add("not-supported pdu type", pdu.getValue(pos, 4));
    m_report->add("not-supported pdu type", txt);
    pos += 4;

    // variable data
    /*
    m_report->add("not-supported sub pdu type", pdu.getValue(pos, 4));
    pos += 4;

    m_report->add("length of the copied pdu", pdu.getValue(pos, 8));
    pos += 8;
    */

    m_report->send();
}

