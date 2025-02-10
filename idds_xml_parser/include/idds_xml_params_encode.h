#include "idds_xml_error.h"

#include <string>
#include <tuple>

#ifndef IDDS_XML_PARAMS_ENCODE_H
#define IDDS_XML_PARAMS_ENCODE_H

template <typename T>
class idds_xml_params_encode
{
    public:
    explicit idds_xml_params_encode(std::string strParamName, T value) :
        m_strParamName(strParamName),
        m_Param(value)
    {
    };

    [[nodiscard]]
    std::string get_parsed_string() const;

    private:
    T m_Param;
    std::string m_strParamName;
};

#endif // IDDS_XML_PARAMS_ENCODE_H
