#include "idds_xml_error.h"
#include "idds_xml_types.h"
#include "idds_xml_utils.h"

#include "pugixml.hpp"
#include <string>
#include <vector>

#ifndef IDDS_XML_RESPONSE_H
#define IDDS_XML_RESPONSE_H

class idds_xml_response
{
    public:
    explicit idds_xml_response();
    explicit idds_xml_response(const std::string& strXmlBody);

    // delete other kind of constructors and assingment operator
    idds_xml_response(idds_xml_response&&) = delete;
    idds_xml_response(const idds_xml_response&) = delete;
    idds_xml_response& operator=(const idds_xml_response&) = delete;

    ~idds_xml_response();

    void add_code(const int& nCode)
    {
        m_nCode = nCode;
    }

    void add_param(const std::string& strParam)
    {
        m_vecParams.push_back(strParam);
    }

    void add_opt_message(const std::string& strMessage)
    {
        m_strMessage = strMessage;
    }

    [[nodiscard]]
    std::string get_message() const
    {
        return m_strMessage;
    }

    [[nodiscard]]
    int get_code() const
    {
        return m_nCode;
    }

    [[nodiscard]]
    std::vector<std::string> get_params() const
    {
        return m_vecParams;
    }

    [[nodiscard]]
    std::tuple<idds_xml_error, std::string> build();

    [[nodiscard]]
    idds_xml_error parse();

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
    std::vector<std::string> m_vecParams;
    std::string m_strMessage;
    int m_nCode;
};

#endif // IDDS_XML_RESPONSE_H
