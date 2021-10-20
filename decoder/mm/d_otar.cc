#include "mm.h"

using namespace Tetra;

/**
 * @brief MM D-OTAR sub-type - A.8.58 (EN 300 392-7)
 *
 */

void Mm::parseDOtar(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar", pdu.toString().c_str());

    uint32_t pos = 4;                                                           // pdu type
    uint32_t otarSubtype = pdu.getValue(pos, 4);

    // Table A.85 (downlink only)

    switch (otarSubtype)
    {
    case 0b0000:
        parseDOtarCckProvide(pdu);
        break;

    case 0b0001:
        //parseDOtarCckReject(pdu);                                             // couldn't find documentation
        break;

    case 0b0010:
        parseDOtarSckProvide(pdu);
        break;

    case 0b0011:
        parseDOtarSckReject(pdu);
        break;

    case 0b0100:
        parseDOtarGckProvide(pdu);
        break;

    case 0b0101:
        parseDOtarGckReject(pdu);
        break;

    case 0b0110:
        parseDOtarKeyAssociateDemand(pdu);
        break;

    case 0b0111:
        parseDOtarNewcell(pdu);
        break;

    case 0b1000:
        parseDOtarGskoProvide(pdu);
        break;

    case 0b1001:
        parseDOtarGskoReject(pdu);
        break;

    case 0b1010:
        parseDOtarKeyDeleteDemand(pdu);
        break;

    case 0b1011:
        parseDOtarKeyStatusDemand(pdu);
        break;

    case 0b1100:
        parseDOtarCmgGtsiProvide(pdu);
        break;

    case 0b1101:
        parseDOtarDmSckActivate(pdu);
        break;

    default:                                                                    // reserved
        break;
    }
}

/**
 * @brief MM D-OTAR CCK Provide - A.2.1 (EN 300 392-7)
 *
 */

void Mm::parseDOtarCckProvide(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar_cck_provide", pdu.toString().c_str());

    m_report->start("MM", "D-OTAR CCK Provide", m_tetraTime, m_macAddress);

    uint32_t pos = 8;                                                           // pdu type
    uint32_t cckProvisionFlag = pdu.getValue(pos, 1);
    m_report->add("cck provision flag", pdu.getValue(pos, 1));
    pos += 1;

    if (cckProvisionFlag)
    {
        // TODO parse
    }

    m_report->send();
}

/**
 * @brief MM D-OTAR SCK Provide - A.2.7 (EN 300 392-7)
 *
 */

void Mm::parseDOtarSckProvide(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar_sck_provide", pdu.toString().c_str());

    m_report->start("MM", "D-OTAR SCK Provide", m_tetraTime, m_macAddress);

    uint32_t pos = 8;                                                           // pdu type
    uint32_t acknowledgementFlag = pdu.getValue(pos, 1);
    m_report->add("acknowledgement flag", pdu.getValue(pos, 1));
    pos += 1;

    if (acknowledgementFlag)
    {
        m_report->add("explicit response", pdu.getValue(pos, 1));
    }
    else
    {
        m_report->add("reserved", pdu.getValue(pos, 1));
    }
    pos += 1;

    m_report->add("max response timer value", pdu.getValue(pos, 16));
    pos += 16;

    uint32_t sessionKey = pdu.getValue(pos, 1);
    m_report->add("session key", pdu.getValue(pos, 1));
    pos += 1;

    if (sessionKey)                                                             // encrypted with group encryption session key
    {
        m_report->add("gsko-vn", pdu.getValue(pos, 16));
        pos += 16;
    }
    else                                                                        // encrypted with individual encryption session key
    {
        m_report->add("random seed for otar", pdu.getValue(pos, 80));
        pos += 80;
    }

    uint32_t numberOfScks = pdu.getValue(pos, 3);
    m_report->add("number of scks provided", pdu.getValue(pos, 3));
    pos += 3;

    for (uint8_t cnt = 1; cnt <= numberOfScks; cnt++)
    {
        m_report->add("sck key and identifier", pdu.getValue(pos, 143));
        pos += 143;
    }

    m_report->add("ksg number", pdu.getValue(pos, 4));
    pos += 4;

    m_report->add("otar retry interval", pdu.getValue(pos, 3));
    pos += 3;

    // TODO type 2/3 elements

    m_report->send();
}

/**
 * @brief MM D-OTAR SCK Reject - A.2.9a (EN 300 392-7)
 *
 */

void Mm::parseDOtarSckReject(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar_sck_reject", pdu.toString().c_str());

    m_report->start("MM", "D-OTAR SCK Reject", m_tetraTime, m_macAddress);

    uint32_t pos = 8;                                                           // pdu type
    uint32_t numberOfScksRejected = pdu.getValue(pos, 3);
    m_report->add("number of scks rejected", pdu.getValue(pos, 3));
    pos += 3;

    for (uint8_t cnt = 1; cnt <= numberOfScksRejected; cnt++)
    {
        m_report->add("sck rejected", pdu.getValue(pos, 8));
        pos += 8;
    }

    m_report->add("otar retry interval", pdu.getValue(pos, 3));
    pos += 3;

    // TODO type 2/3 elements

    m_report->send();
}

/**
 * @brief MM D-OTAR GCK Provide - A.2.4 (EN 300 392-7)
 *
 */

void Mm::parseDOtarGckProvide(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar_gck_provide", pdu.toString().c_str());

    m_report->start("MM", "D-OTAR GCK Provide", m_tetraTime, m_macAddress);

    uint32_t pos = 8;                                                           // pdu type
    uint32_t acknowledgementFlag = pdu.getValue(pos, 1);
    m_report->add("acknowledgement flag", pdu.getValue(pos, 1));
    pos += 1;

    if (acknowledgementFlag)
    {
        m_report->add("explicit response", pdu.getValue(pos, 1));
    }
    else
    {
        m_report->add("reserved", pdu.getValue(pos, 1));
    }
    pos += 1;

    m_report->add("max response timer value", pdu.getValue(pos, 16));
    pos += 16;

    uint32_t sessionKey = pdu.getValue(pos, 1);
    m_report->add("session key", pdu.getValue(pos, 1));
    pos += 1;

    if (sessionKey)                                                             // encrypted with group encryption session key
    {
        m_report->add("gsko-vn", pdu.getValue(pos, 16));
        pos += 16;
    }
    else                                                                        // encrypted with individual encryption session key
    {
        m_report->add("random seed for otar", pdu.getValue(pos, 80));
        pos += 80;
    }

    uint32_t numberOfGcks = pdu.getValue(pos, 3);
    m_report->add("number of gcks provided", pdu.getValue(pos, 3));
    pos += 3;

    for (uint8_t cnt = 1; cnt <= numberOfGcks; cnt++)
    {
        m_report->add("gck key and identifier", pdu.getValue(pos, 152));
        pos += 152;
    }

    m_report->add("ksg number", pdu.getValue(pos, 4));
    pos += 4;

    uint8_t groupAssociation = pdu.getValue(pos, 1);
    m_report->add("group association", pdu.getValue(pos, 1));
    pos += 1;

    if (groupAssociation)
    {
        m_report->add("gssi", pdu.getValue(pos, 24));
        pos += 24;
    }

    m_report->add("otar retry interval", pdu.getValue(pos, 3));
    pos += 3;

    // TODO type 2/3 elements

    m_report->send();
}

/**
 * @brief MM D-OTAR GCK Reject - A.2.6a (EN 300 392-7)
 *
 */

void Mm::parseDOtarGckReject(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar_gck_reject", pdu.toString().c_str());

    m_report->start("MM", "D-OTAR GCK Reject", m_tetraTime, m_macAddress);

    uint32_t pos = 8;                                                           // pdu type
    uint32_t numberOfGcksRejected = pdu.getValue(pos, 3);
    m_report->add("number of gcks rejected", pdu.getValue(pos, 3));
    pos += 3;

    // gck reject length varies, so do not parse
    /*
    for (uint8_t cnt = 1; cnt <= numberOfGcksRejected; cnt++)
    {
        m_report->add("gck rejected", pdu.getValue(pos, 8));
        pos += 8;
    }
    */

    m_report->send();
}

/**
 * @brief MM D-OTAR KEY ASSOCIATE demand - A.3.1 (EN 300 392-7)
 *
 */

void Mm::parseDOtarKeyAssociateDemand(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar_key_associate_demand", pdu.toString().c_str());

    m_report->start("MM", "D-OTAR KEY ASSOCIATE demand", m_tetraTime, m_macAddress);

    uint32_t pos = 8;                                                           // pdu type
    uint32_t acknowledgementFlag = pdu.getValue(pos, 1);
    m_report->add("acknowledgement flag", pdu.getValue(pos, 1));
    pos += 1;

    if (acknowledgementFlag)
    {
        m_report->add("explicit response", pdu.getValue(pos, 1));
    }
    else
    {
        // invalid field, do not parse
    }
    pos += 1;

    m_report->add("max response timer value", pdu.getValue(pos, 16));
    pos += 16;

    uint8_t keyAssociationType = pdu.getValue(pos, 1);
    m_report->add("key association type", pdu.getValue(pos, 1));
    pos += 1;

    if (keyAssociationType)                                                     // gck
    {
        m_report->add("gck select number", pdu.getValue(pos, 17));
        pos += 17;
    }
    else                                                                        // sck
    {
        m_report->add("sck select number", pdu.getValue(pos, 6));
        pos += 6;
        m_report->add("sck subset grouping type", pdu.getValue(pos, 4));
        pos += 4;
    }

    uint32_t numberOfGroups = pdu.getValue(pos, 5);
    m_report->add("number of groups", pdu.getValue(pos, 5));
    pos += 5;

    for (uint8_t cnt = 1; cnt <= numberOfGroups; cnt++)
    {
        m_report->add("gssi", pdu.getValue(pos, 24));
        pos += 24;
    }

    // TODO type 2

    m_report->send();
}

/**
 * @brief MM D-OTAR NEWCELL - A.5.3 (EN 300 392-7)
 *
 */

void Mm::parseDOtarNewcell(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar_newcell", pdu.toString().c_str());

    m_report->start("MM", "D-OTAR NEWCELL", m_tetraTime, m_macAddress);

    uint32_t pos = 8;                                                           // pdu type
    m_report->add("dck forwarding result", pdu.getValue(pos, 1));
    pos += 1;

    m_report->add("cck provision flag", pdu.getValue(pos, 1));
    pos += 1;

    // variable length, do not parse

    m_report->send();
}

/**
 * @brief MM D-OTAR GSKO Provide - A.2.10 (EN 300 392-7)
 *
 */

void Mm::parseDOtarGskoProvide(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar_gsko_provide", pdu.toString().c_str());

    m_report->start("MM", "D-OTAR GSKO Provide", m_tetraTime, m_macAddress);

    uint32_t pos = 8;                                                           // pdu type
    m_report->add("random seed for otar", pdu.getValue(pos, 80));
    pos += 80;

    m_report->add("gsko-vn", pdu.getValue(pos, 16));
    pos += 16;

    m_report->add("sealed gsko", pdu.getValue(pos, 120));
    pos += 120;

    m_report->add("gssi", pdu.getValue(pos, 24));
    pos += 24;

    // TODO type 2/3 elements

    m_report->send();
}

/**
 * @brief MM D-OTAR GSKO Reject - A.2.12a (EN 300 392-7)
 *
 */

void Mm::parseDOtarGskoReject(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar_gsko_reject", pdu.toString().c_str());

    m_report->start("MM", "D-OTAR GSKO Reject", m_tetraTime, m_macAddress);

    uint32_t pos = 8;                                                           // pdu type
    m_report->add("otar reject reason", pdu.getValue(pos, 3));
    pos += 3;

    m_report->add("gssi", pdu.getValue(pos, 24));
    pos += 24;

    m_report->add("otar retry interval", pdu.getValue(pos, 3));
    pos += 3;

    // TODO type 2/3 elements

    m_report->send();
}

/**
 * @brief MM D-OTAR KEY DELETE demand - A.4a.1 (EN 300 392-7)
 *
 */

void Mm::parseDOtarKeyDeleteDemand(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar_key_delete_demand", pdu.toString().c_str());

    m_report->start("MM", "D-OTAR KEY DELETE demand", m_tetraTime, m_macAddress);

    uint32_t pos = 8;                                                           // pdu type
    uint32_t keyDeleteType = pdu.getValue(pos, 3);
    m_report->add("key delete type", pdu.getValue(pos, 3));
    pos += 3;

    if (keyDeleteType == 0 or keyDeleteType == 1)
    {
        uint32_t numberOfScksDeleted = pdu.getValue(pos, 5);
        m_report->add("number of scks deleted", pdu.getValue(pos, 5));
        pos += 5;

        for (uint8_t cnt = 1; cnt <= numberOfScksDeleted; cnt++)
        {
            m_report->add("sckn", pdu.getValue(pos, 5));
            pos += 5;
        }
    }
    if (keyDeleteType == 2)
    {
        m_report->add("sck subset grouping type", pdu.getValue(pos, 4));
        pos += 4;
        m_report->add("sck subset number", pdu.getValue(pos, 5));
        pos += 5;
    }
    if (keyDeleteType == 3)
    {
        uint32_t numberOfGcksDeleted = pdu.getValue(pos, 4);
        m_report->add("number of gcks deleted", pdu.getValue(pos, 4));
        pos += 4;

        for (uint8_t cnt = 1; cnt <= numberOfGcksDeleted; cnt++)
        {
            m_report->add("gckn", pdu.getValue(pos, 16));
            pos += 16;
        }
    }

    // TODO type 2 element

    m_report->send();
}

/**
 * @brief MM D-OTAR KEY STATUS demand - A.4b.1 (EN 300 392-7)
 *
 */

void Mm::parseDOtarKeyStatusDemand(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar_key_status_demand", pdu.toString().c_str());

    m_report->start("MM", "D-OTAR KEY STATUS demand", m_tetraTime, m_macAddress);

    uint32_t pos = 8;                                                           // pdu type
    uint32_t acknowledgementFlag = pdu.getValue(pos, 1);
    m_report->add("acknowledgement flag", pdu.getValue(pos, 1));
    pos += 1;

    if (acknowledgementFlag)
    {
        m_report->add("explicit response", pdu.getValue(pos, 1));
    }
    else
    {
        // invalid field, do not parse
    }
    pos += 1;

    m_report->add("max response timer value", pdu.getValue(pos, 16));
    pos += 16;

    uint32_t keyStatusType = pdu.getValue(pos, 3);
    m_report->add("key status type", pdu.getValue(pos, 3));
    pos += 3;

    if (keyStatusType == 0)
    {
        m_report->add("sckn", pdu.getValue(pos, 5));
        pos += 5;
    }
    if (keyStatusType == 1)
    {
        m_report->add("sck subset grouping type", pdu.getValue(pos, 4));
        pos += 4;
        m_report->add("sck subset number", pdu.getValue(pos, 5));
        pos += 5;
    }
    if (keyStatusType == 3)
    {
        m_report->add("gckn", pdu.getValue(pos, 16));
        pos += 16;
    }

    // TODO type 2 element

    m_report->send();
}

/**
 * @brief MM D-OTAR CMG GTSI PROVIDE - A.5.4 (EN 300 392-7)
 *
 */

void Mm::parseDOtarCmgGtsiProvide(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar_cmg_gtsi_provide", pdu.toString().c_str());

    m_report->start("MM", "D-OTAR CMG GTSI PROVIDE", m_tetraTime, m_macAddress);

    uint32_t pos = 8;
    m_report->add("gssi", pdu.getValue(pos, 24));
    pos += 24;

    // TODO type 2 element

    m_report->send();
}

/**
 * @brief MM D-DM-SCK ACTIVATE DEMAND - A.5.3 (EN 300 392-7)
 *
 */

void Mm::parseDOtarDmSckActivate(Pdu pdu)
{
    m_log->print(LogLevel::HIGH, "DEBUG ::%-44s - pdu = %s\n", "mm_parse_d_otar_dm_sck_activate", pdu.toString().c_str());

    m_report->start("MM", "D-DM-SCK ACTIVATE DEMAND", m_tetraTime, m_macAddress);

    uint32_t pos = 8;                                                           // pdu type
    m_report->add("acknowledgement flag", pdu.getValue(pos, 1));
    pos += 1;

    uint32_t numberOfScksChanged = pdu.getValue(pos, 4);
    m_report->add("number of scks changed", pdu.getValue(pos, 4));
    pos += 4;

    if (numberOfScksChanged == 0)
    {
        m_report->add("sck subset grouping type", pdu.getValue(pos, 4));
        pos += 4;
        m_report->add("sck subset number", pdu.getValue(pos, 5));
        pos += 5;
        m_report->add("sck-vn", pdu.getValue(pos, 16));
        pos += 16;
    }
    else
    {
        for (uint8_t cnt = 1; cnt <= numberOfScksChanged; cnt++)
        {
            m_report->add("sck data", pdu.getValue(pos, 21));
            pos += 21;
        }
    }

    uint8_t timeType = pdu.getValue(pos, 2);
    m_report->add("time type", pdu.getValue(pos, 2));
    pos += 2;

    if (timeType == 0)                                                          // absolute IV
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
    if (timeType == 1)                                                          // network time
    {
        m_report->add("network time", pdu.getValue(pos, 48));
        pos += 48;
    }

    m_report->add("address extension", pdu.getValue(pos, 24));
    pos += 24;

    m_report->send();
}
