#pragma once
#include "pugixml.hpp"
#include "idds_xml_error.h"
#include "idds_xml_types.h"

#include <string>
#include <vector>

class idds_xml_request
{
    public:
    explicit idds_xml_request(const std::string& strXmlBody);
    explicit idds_xml_request();

    // delete other kind of constructors and assingment operator
    idds_xml_request(idds_xml_request&&) = delete;
    idds_xml_request(const idds_xml_request&) = delete;
    idds_xml_request& operator=(const idds_xml_request&) = delete;

    ~idds_xml_request();

    [[nodiscard]]
    idds_xml_error parse();

    [[nodiscard]]
    std::tuple<idds_xml_error, std::string> build();

    [[nodiscard]]
    std::vector<std::string> get_params()
    {
        return m_vecParams;
    }

    [[nodiscard]]
    std::string get_method_name() const
    {
        return m_strMethodName;
    }

    int get_return_code()
    {
        return m_retCode;
    }

    void add_method_name(const std::string& strMethodName)
    {
        m_strMethodName = strMethodName;
    }

    void add_param(const std::string& strParam)
    {
        m_vecParams.push_back(strParam);
    }

    private:

    [[nodiscard]]
    idds_xml_error is_valid(pugi::xml_document& xmlDoc) const;

    [[nodiscard]]
    idds_xml_error load_xml(pugi::xml_document& xmlDoc) const;

    [[nodiscard]]
    idds_xml_error validate_schema(pugi::xml_document &xmlDoc) const;

    [[nodiscard]]
    idds_xml_error validate_mandatory_nodes(pugi::xml_document &xmlDoc) const;

    [[nodiscard]]
    idds_xml_error validate_optional_nodes(pugi::xml_document &xmlDoc) const;

    private:
    std::string m_xmlBody;
    std::string m_strMethodName;
    std::vector<std::string> m_vecParams;
    pugi::xml_document m_xmlDoc;
    int m_retCode;
};
