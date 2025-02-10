#include "idds_xml_response.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <sstream>

idds_xml_response::idds_xml_response() :
    m_nCode(c_invalidCode)
{
}

idds_xml_response::idds_xml_response(const std::string& strXmlBody) :
    m_xmlBody(strXmlBody),
    m_nCode(c_invalidCode)
{
}

idds_xml_response::~idds_xml_response()
{
}

idds_xml_error idds_xml_response::parse()
{
    pugi::xml_document xmlDoc;
    if(auto err = is_valid(xmlDoc); err != idds_xml_error::ok)
        return err;

    // parse method name
    m_strMessage  = xmlDoc.child("methodResponse").child("message").text().as_string();

    // parse method name
    m_nCode = xmlDoc.child("methodResponse").child("code").text().as_int();

    if (!xmlDoc.child("methodResponse").child("params").children().empty())
    {
        for (pugi::xml_node tool: xmlDoc.child("methodResponse").child("params").children())
        {
            std::ostringstream strParam;
            tool.print(strParam);
            m_vecParams.emplace_back(strParam.str());
        }
    }

    return idds_xml_error::ok;
}

idds_xml_error idds_xml_response::load_xml(pugi::xml_document& xmlDoc) const
{
    pugi::xml_parse_result xmlRes = xmlDoc.load_string(m_xmlBody.c_str());
    return xmlRes.status == pugi::status_ok ? idds_xml_error::ok : idds_xml_error::invalid_xml;
}

idds_xml_error idds_xml_response::is_valid(pugi::xml_document& xmlDoc) const
{
    auto err = load_xml(xmlDoc);
    if (err != idds_xml_error::ok)
        return err;

    // validate if is according to schema
    err = validate_schema(xmlDoc);
    if (err != idds_xml_error::ok)
        return err;

    return idds_xml_error::ok;
}

idds_xml_error idds_xml_response::validate_schema(pugi::xml_document &xmlDoc) const
{
    // validate the overall schema

    //  <methodResponse>                 -> mandatory
    //    <code>0</code>                 -> mandatory
    //    <message>lorem ipsum</message> -> optional
    //    <params>                       -> optional
    //      <param>
    //        <value><Double>18.24668429131</Double></value>
    //      </param>
    //    </params>
    //  </methodReponse>

    // check mandatory nodes
    idds_xml_error err = validate_mandatory_nodes(xmlDoc);
    if (err != idds_xml_error::ok)
        return err;

    // check optional nodes
    err = validate_optional_nodes(xmlDoc);
    if (err != idds_xml_error::ok)
        return err;

    return idds_xml_error::ok;
}

idds_xml_error idds_xml_response::validate_mandatory_nodes(pugi::xml_document &xmlDoc) const
{
     idds_xml_walker walker = idds_xml_walker({
        {"methodResponse", 0},
        {"code", 0}
    });

    xmlDoc.traverse(walker);
    for (const auto& node: walker.m_Occurrences)
    {
        if (node.first == "methodResponse" || node.first == "code")
        {
            if (node.second > 1)
                return idds_xml_error::not_schema_compliant_multiple_occurrences;
        }
    }

    // check if methodResponse is present
    if (xmlDoc.child("methodResponse").empty())
        return idds_xml_error::not_schema_compliant_mandatory_missing;

    // check if root node is methodResponse
    if (xmlDoc.child("methodResponse").type() == pugi::node_document)
        return idds_xml_error::not_schema_compliant_invalid_value;

    // check if code is present
    if (xmlDoc.child("methodResponse").child("code").empty())
        return idds_xml_error::not_schema_compliant_mandatory_missing;

    // check if code value is a number
    if (xmlDoc.child("methodResponse").child("code").text().as_int(ERR_CONV_DEFAULT)
            == ERR_CONV_DEFAULT)
        return idds_xml_error::not_schema_compliant_invalid_value;

    return idds_xml_error::ok;
}

idds_xml_error idds_xml_response::validate_optional_nodes(pugi::xml_document &xmlDoc) const
{
    std::vector<std::string> vecChildren;

    for (const auto& it: xmlDoc.child("methodResponse"))
        vecChildren.emplace_back(it.name());

    // Check if all children are valid
    if (vecChildren.size() > MAX_ALLOWED_RESPONSE_ROOT_CHILDREN)
        return idds_xml_error::not_schema_compliant_too_many_child_nodes;

    int nAllowedParams = 1; // code is already in, due to validate_mandatory_nodes
    for (const auto& e: std::vector<std::string>({"params", "message"}))
        // maxOccurs of params or message must be always 1, lets only count if that is the case
        if (int nCount = std::count(vecChildren.cbegin(), vecChildren.cend(), e); nCount == 1)
            nAllowedParams += nCount;

    if (nAllowedParams != vecChildren.size())
        return idds_xml_error::not_schema_compliant_invalid_optional_node;

    return idds_xml_error::ok;
}

std::tuple<idds_xml_error, std::string> idds_xml_response::build()
{
    if (m_nCode == c_invalidCode)
        return {idds_xml_error::response_code_not_set, ""};
    else if (m_nCode < 0 || m_nCode > 255)
        return {idds_xml_error::response_code_range_invalid, ""};

    m_xmlBody = "<methodResponse>";

    // Add code
    m_xmlBody += "<code>" + std::to_string(m_nCode) + "</code>";

    // Add message
    if (m_strMessage.size() == 0)
        m_xmlBody += "<message />";
    else
        m_xmlBody += "<message>" + m_strMessage + "</message>";

    // Add params
    if (m_vecParams.size() == 0)
        m_xmlBody += "<params />";
    else
    {
        m_xmlBody += "<params>";
        for (const auto& param: m_vecParams)
            m_xmlBody += param;
        m_xmlBody += "</params>";
    }

    m_xmlBody += "</methodResponse>";

    pugi::xml_document xmlDoc;
    if (auto iddsError = is_valid(xmlDoc); iddsError != idds_xml_error::ok)
        return {iddsError, ""};

    return {idds_xml_error::ok, m_xmlBody};
}
