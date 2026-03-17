#pragma once

// std_fom
#include "rpr/rpr-base_v2.0.xml.h"

[[nodiscard]] inline std::string toString(const rpr::EntityTypeStruct entityType)
{
    return std::to_string(entityType.category)
    + ":" + std::to_string(entityType.countryCode)
    + ":" + std::to_string(entityType.domain)
    + ":" + std::to_string(entityType.entityKind)
    + ":" + std::to_string(entityType.extra)
    + ":" + std::to_string(entityType.specific)
    + ":" + std::to_string(entityType.subcategory);
}