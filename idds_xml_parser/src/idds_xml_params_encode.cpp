#include "idds_xml_params_encode.h"
#include <cstdint>
#include <vector>
#include <string>
#include <sstream>

template<>
std::string idds_xml_params_encode<int8_t>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><Byte>" << std::to_string(m_Param) << "</Byte></value>";
    ss << "</param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<std::vector<int8_t>>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><ListOfByte>";
    for (const auto& e: m_Param)
    {
        ss << "<Byte>" << std::to_string(e) << "</Byte>";
    }
    ss << "</ListOfByte></value></param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<uint8_t>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><SByte>" << std::to_string(m_Param) << "</SByte></value>";
    ss << "</param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<std::vector<uint8_t>>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><ListOfSByte>";
    for (const auto& e: m_Param)
    {
        ss << "<SByte>" << std::to_string(e) << "</SByte>";
    }
    ss << "</ListOfSByte></value></param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<int16_t>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><Int16>" << std::to_string(m_Param) << "</Int16></value>";
    ss << "</param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<std::vector<int16_t>>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><ListOfInt16>";
    for (const auto& e: m_Param)
    {
        ss << "<Int16>" << std::to_string(e) << "</Int16>";
    }
    ss << "</ListOfInt16></value></param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<uint16_t>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><UInt16>" << std::to_string(m_Param) << "</UInt16></value>";
    ss << "</param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<std::vector<uint16_t>>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><ListOfUInt16>";
    for (const auto& e: m_Param)
    {
        ss << "<UInt16>" << std::to_string(e) << "</UInt16>";
    }
    ss << "</ListOfUInt16></value></param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<int32_t>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><Int32>" << std::to_string(m_Param) << "</Int32></value>";
    ss << "</param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<std::vector<int32_t>>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><ListOfInt32>";
    for (const auto& e: m_Param)
    {
        ss << "<Int32>" << std::to_string(e) << "</Int32>";
    }
    ss << "</ListOfInt32></value></param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<uint32_t>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><UInt32>" << std::to_string(m_Param) << "</UInt32></value>";
    ss << "</param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<std::vector<uint32_t>>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><ListOfUInt32>";
    for (const auto& e: m_Param)
    {
        ss << "<UInt32>" << std::to_string(e) << "</UInt32>";
    }
    ss << "</ListOfUInt32></value></param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<int64_t>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><Int64>" << std::to_string(m_Param) << "</Int64></value>";
    ss << "</param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<std::vector<int64_t>>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><ListOfInt64>";
    for (const auto& e: m_Param)
    {
        ss << "<Int64>" << std::to_string(e) << "</Int64>";
    }
    ss << "</ListOfInt64></value></param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<uint64_t>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><UInt64>" << std::to_string(m_Param) << "</UInt64></value>";
    ss << "</param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<std::vector<uint64_t>>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><ListOfUInt64>";
    for (const auto& e: m_Param)
    {
        ss << "<UInt64>" << std::to_string(e) << "</UInt64>";
    }
    ss << "</ListOfUInt64></value></param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<float>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><Float>" << std::to_string(m_Param) << "</Float></value>";
    ss << "</param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<std::vector<float>>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><ListOfFloat>";
    for (const auto& e: m_Param)
    {
        ss << "<Float>" << std::to_string(e) << "</Float>";
    }
    ss << "</ListOfFloat></value></param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<double>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><Double>" << std::to_string(m_Param) << "</Double></value>";
    ss << "</param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<std::vector<double>>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><ListOfDouble>";
    for (const auto& e: m_Param)
    {
        ss << "<Double>" << std::to_string(e) << "</Double>";
    }
    ss << "</ListOfDouble></value></param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<bool>::get_parsed_string() const
{
    std::stringstream ss;
    std::string strBool = m_Param ? "true" : "false";
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><Boolean>" << strBool << "</Boolean></value>";
    ss << "</param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<std::vector<bool>>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><ListOfBoolean>";
    for (const auto& e: m_Param)
    {
        std::string strBool = e ? "true" : "false";
        ss << "<Boolean>" << strBool << "</Boolean>";
    }
    ss << "</ListOfBoolean></value></param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<std::string>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><String>" << m_Param << "</String></value>";
    ss << "</param>";
    return ss.str();
}

template<>
std::string idds_xml_params_encode<std::vector<std::string>>::get_parsed_string() const
{
    std::stringstream ss;
    ss << "<param>";
    ss << "<name>" << m_strParamName << "</name>";
    ss << "<value><ListOfString>";
    for (const auto& e: m_Param)
    {
        ss << "<String>" << e << "</String>";
    }
    ss << "</ListOfString></value></param>";
    return ss.str();
}
