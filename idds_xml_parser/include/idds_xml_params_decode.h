#pragma once
#include "pugixml.hpp"
#include "idds_xml_error.h"

#include <string>
#include <iostream>
#include <tuple>

template <typename T>
class idds_xml_params_decode
{
    public:
    explicit idds_xml_params_decode(std::string strXmlEntry)
    {
        m_parseResult = m_xmlDoc.load_string(strXmlEntry.c_str());
    };

    [[nodiscard]]
    std::tuple<idds_xml_error, T> get_value() const;

    [[nodiscard]]
    std::tuple<idds_xml_error, std::string> get_name() const
    {
        if (is_valid())
            return {idds_xml_error::invalid_xml, ""};

        if (m_xmlDoc.child("param").empty())
            return {idds_xml_error::param_parent_missing, ""};

        if (m_xmlDoc.child("param").child("name").empty())
            return {idds_xml_error::param_name_keyword_missing, ""};

        if (m_xmlDoc.child("param").child("value").empty())
            return {idds_xml_error::param_value_keyword_missing, ""};

        return {idds_xml_error::ok, m_xmlDoc.child("param").child("name").text().as_string()};
    }

    [[nodiscard]]
    bool is_valid() const
    {
        return m_parseResult.status != pugi::status_ok;
    }

    private:
    pugi::xml_document m_xmlDoc;
    pugi::xml_parse_result m_parseResult;
};
