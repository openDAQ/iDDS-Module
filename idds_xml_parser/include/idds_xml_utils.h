#pragma once
#include "pugixml.hpp"
#include <map>

struct idds_xml_walker: pugi::xml_tree_walker
{
    idds_xml_walker(std::map<std::string, int> mapOccurrence)
        : m_Occurrences(mapOccurrence)
    {
    }

    virtual bool for_each(pugi::xml_node& node)
    {
        m_Occurrences[node.name()]++;
        return true;
    }

    std::map<std::string, int> m_Occurrences;
};

