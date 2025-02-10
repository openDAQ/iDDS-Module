#include "idds_xml_params_decode.h"
#include <string>

template<>
std::tuple<idds_xml_error, std::string> idds_xml_params_decode<std::string>::get_value() const
{
    auto xmlEntry = m_xmlDoc.child("param").child("value").child("String");
    if (xmlEntry.empty())
        return {idds_xml_error::param_name_keyword_missing, ""};

    return {idds_xml_error::ok, xmlEntry.text().as_string()};
}

template<>
std::tuple<idds_xml_error, float> idds_xml_params_decode<float>::get_value() const
{
    auto xmlEntry = m_xmlDoc.child("param").child("value").child("Float");
    if (xmlEntry.empty())
        return {idds_xml_error::param_name_keyword_missing, 0.0f};

    return {idds_xml_error::ok, xmlEntry.text().as_float()};
}
