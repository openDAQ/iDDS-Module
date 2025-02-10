#include "idds_xml_request.h"
#include "idds_xml_error.h"
#include "idds_xml_utils.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

idds_xml_request::idds_xml_request(const std::string& strXmlBody)
    : m_xmlBody(strXmlBody)
{
}

idds_xml_request::idds_xml_request()
{
}

idds_xml_request::~idds_xml_request()
{
}

idds_xml_error idds_xml_request::load_xml(pugi::xml_document& xmlDoc) const
{
    pugi::xml_parse_result xmlRes = xmlDoc.load_string(m_xmlBody.c_str());
    return xmlRes.status == pugi::status_ok ? idds_xml_error::ok : idds_xml_error::invalid_xml;
}

idds_xml_error idds_xml_request::is_valid(pugi::xml_document& xmlDoc) const
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

idds_xml_error idds_xml_request::validate_schema(pugi::xml_document &xmlDoc) const
{
    // validate the overall schema

    //  <methodCall>                             -> mandatory
    //    <methodName>NameOfCommand</methodName> -> mandatory
    //    <params>                               -> optional
    //      <param><name>NameOfParameter</name>
    //      <value><Int16>18</Int16></value>
    //      </param>
    //    </params>
    //  </methodCall>

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

idds_xml_error idds_xml_request::validate_mandatory_nodes(pugi::xml_document &xmlDoc) const
{
    idds_xml_walker walker = idds_xml_walker({
            {"methodCall", 0},
            {"methodName", 0}
            });

    xmlDoc.traverse(walker);
    for (const auto& node: walker.m_Occurrences)
    {
        if (node.first == "methodCall" || node.first == "methodName")
        {
            if (node.second > 1)
                return idds_xml_error::not_schema_compliant_multiple_occurrences;
        }
    }

    // check if methodResponse is present
    if (xmlDoc.child("methodCall").empty())
        return idds_xml_error::not_schema_compliant_mandatory_missing;

    // check if root node is methodResponse
    if (xmlDoc.child("methodCall").type() == pugi::node_document)
        return idds_xml_error::not_schema_compliant_invalid_value;

    // check if code is present
    if (xmlDoc.child("methodCall").child("methodName").empty())
        return idds_xml_error::not_schema_compliant_mandatory_missing;

    // check if code value is a number
    if (!strcmp(xmlDoc.child("methodCall").child("methodName").text().as_string(), ""))
        return idds_xml_error::not_schema_compliant_invalid_value;

    return idds_xml_error::ok;
}

idds_xml_error idds_xml_request::validate_optional_nodes(pugi::xml_document &xmlDoc) const
{
    std::vector<std::string> vecChildren;

    for (const auto& it: xmlDoc.child("methodCall"))
        vecChildren.emplace_back(it.name());

    // Check if all children are valid
    if (vecChildren.size() > MAX_ALLOWED_REQUEST_ROOT_CHILDREN)
        return idds_xml_error::not_schema_compliant_too_many_child_nodes;
    else if (vecChildren.size() == MAX_ALLOWED_REQUEST_ROOT_CHILDREN)
    {
        if(std::count(vecChildren.cbegin(), vecChildren.cend(), "params") == 0)
            return idds_xml_error::not_schema_compliant_invalid_optional_node;
    }

    return idds_xml_error::ok;
}

idds_xml_error idds_xml_request::parse()
{
    pugi::xml_document xmlDoc;
    if(auto err = is_valid(xmlDoc); err != idds_xml_error::ok)
        return err;

    // parse method name
    m_strMethodName = xmlDoc.child("methodCall").child("methodName").text().as_string();

    if (!xmlDoc.child("methodCall").child("params").children().empty())
    {
        for (pugi::xml_node tool: xmlDoc.child("methodCall").child("params").children())
        {
            std::ostringstream strParam;
            tool.print(strParam);
            m_vecParams.emplace_back(strParam.str());
        }
    }

    return idds_xml_error::ok;
}

std::tuple<idds_xml_error, std::string> idds_xml_request::build()
{
    if (m_strMethodName.empty())
        return {idds_xml_error::request_method_name_not_set, ""};

    m_xmlBody = "<methodCall>";

    // Add code
    m_xmlBody += "<methodName>" + m_strMethodName + "</methodName>";

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

    m_xmlBody += "</methodCall>";

    pugi::xml_document xmlDoc;
    if (auto iddsError = is_valid(xmlDoc); iddsError != idds_xml_error::ok)
        return {iddsError, ""};

    return {idds_xml_error::ok, m_xmlBody};
}
