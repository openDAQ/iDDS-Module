#include "idds_xml_params_encode.h"
#include <cstdint>
#include <vector>
#include <string>

template<>
std::string idds_xml_params_encode<int8_t>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";
    strParam += "<value><Byte>" + std::to_string(m_Param) + "</Byte></value>";
    strParam += "</param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<std::vector<int8_t>>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";

    strParam += "<value><ListOfByte>";
    for (const auto& e: m_Param)
    {
        std::string innerStr = "<Byte>" + std::to_string(e) + "</Byte>";
         strParam += innerStr;
    }
    strParam += "</ListOfByte></value></param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<uint8_t>::get_parsed_string() const
{

    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";
    strParam += "<value><SByte>" + std::to_string(m_Param) + "</SByte></value>";
    strParam += "</param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<std::vector<uint8_t>>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";

    strParam += "<value><ListOfSByte>";
    for (const auto& e: m_Param)
    {
        std::string innerStr = "<SByte>" + std::to_string(e) + "</SByte>";
         strParam += innerStr;
    }
    strParam += "</ListOfSByte></value></param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<int16_t>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";
    strParam += "<value><Int16>" + std::to_string(m_Param) + "</Int16></value>";
    strParam += "</param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<std::vector<int16_t>>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";

    strParam += "<value><ListOfInt16>";
    for (const auto& e: m_Param)
    {
        std::string innerStr = "<Int16>" + std::to_string(e) + "</Int16>";
         strParam += innerStr;
    }
    strParam += "</ListOfInt16></value></param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<uint16_t>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";
    strParam += "<value><UInt16>" + std::to_string(m_Param) + "</UInt16></value>";
    strParam += "</param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<std::vector<uint16_t>>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";

    strParam += "<value><ListOfUInt16>";
    for (const auto& e: m_Param)
    {
        std::string innerStr = "<UInt16>" + std::to_string(e) + "</UInt16>";
         strParam += innerStr;
    }
    strParam += "</ListOfUInt16></value></param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<int32_t>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";
    strParam += "<value><Int32>" + std::to_string(m_Param) + "</Int32></value>";
    strParam += "</param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<std::vector<int32_t>>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";

    strParam += "<value><ListOfInt32>";
    for (const auto& e: m_Param)
    {
        std::string innerStr = "<Int32>" + std::to_string(e) + "</Int32>";
         strParam += innerStr;
    }
    strParam += "</ListOfInt32></value></param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<uint32_t>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";
    strParam += "<value><UInt32>" + std::to_string(m_Param) + "</UInt32></value>";
    strParam += "</param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<std::vector<uint32_t>>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";

    strParam += "<value><ListOfUInt32>";
    for (const auto& e: m_Param)
    {
        std::string innerStr = "<UInt32>" + std::to_string(e) + "</UInt32>";
         strParam += innerStr;
    }
    strParam += "</ListOfUInt32></value></param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<int64_t>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";
    strParam += "<value><Int64>" + std::to_string(m_Param) + "</Int64></value>";
    strParam += "</param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<std::vector<int64_t>>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";

    strParam += "<value><ListOfInt64>";
    for (const auto& e: m_Param)
    {
        std::string innerStr = "<Int64>" + std::to_string(e) + "</Int64>";
         strParam += innerStr;
    }
    strParam += "</ListOfInt64></value></param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<uint64_t>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";
    strParam += "<value><UInt64>" + std::to_string(m_Param) + "</UInt64></value>";
    strParam += "</param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<std::vector<uint64_t>>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";

    strParam += "<value><ListOfUInt64>";
    for (const auto& e: m_Param)
    {
        std::string innerStr = "<UInt64>" + std::to_string(e) + "</UInt64>";
         strParam += innerStr;
    }
    strParam += "</ListOfUInt64></value></param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<float>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";
    strParam += "<value><Float>" + std::to_string(m_Param) + "</Float></value>";
    strParam += "</param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<std::vector<float>>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";

    strParam += "<value><ListOfFloat>";
    for (const auto& e: m_Param)
    {
        std::string innerStr = "<Float>" + std::to_string(e) + "</Float>";
         strParam += innerStr;
    }
    strParam += "</ListOfFloat></value></param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<double>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";
    strParam += "<value><Double>" + std::to_string(m_Param) + "</Double></value>";
    strParam += "</param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<std::vector<double>>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";

    strParam += "<value><ListOfDouble>";
    for (const auto& e: m_Param)
    {
        std::string innerStr = "<Double>" + std::to_string(e) + "</Double>";
         strParam += innerStr;
    }
    strParam += "</ListOfDouble></value></param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<bool>::get_parsed_string() const
{
    std::string strBool = m_Param ? "true" : "false";

    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";
    strParam += "<value><Boolean>" + strBool + "</Boolean></value>";
    strParam += "</param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<std::vector<bool>>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";

    strParam += "<value><ListOfBoolean>";
    for (const auto& e: m_Param)
    {
        std::string strBool = e ? "true" : "false";
        std::string innerStr = "<Boolean>" + strBool + "</Boolean>";
        strParam += innerStr;
    }
    strParam += "</ListOfBoolean></value></param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<std::string>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";
    strParam += "<value><String>" + m_Param + "</String></value>";
    strParam += "</param>";
    return strParam;
}

template<>
std::string idds_xml_params_encode<std::vector<std::string>>::get_parsed_string() const
{
    std::string strParam = "<param>";
    strParam += "<name>" + m_strParamName + "</name>";

    strParam += "<value><ListOfString>";
    for (const auto& e: m_Param)
    {
        std::string innerStr = "<String>" + e + "</String>";
        strParam += innerStr;
    }
    strParam += "</ListOfString></value></param>";
    return strParam;
}
