#ifndef IDDS_XML_ERROR_H
#define IDDS_XML_ERROR_H

const int c_invalidCode = -1;

enum class idds_xml_error {
    ok,
    invalid_xml,
    not_schema_compliant_mandatory_missing,
    not_schema_compliant_invalid_parent,
    not_schema_compliant_invalid_value,
    not_schema_compliant_multiple_occurrences,
    not_schema_compliant_too_many_child_nodes,
    not_schema_compliant_invalid_optional_node,
    param_parent_missing,
    param_name_keyword_missing,
    param_value_keyword_missing,
    param_value_type_keyword_missing,
    response_code_not_set,
    response_code_range_invalid,
    request_method_name_not_set,
};

#endif // IDDS_XML_ERROR_H
