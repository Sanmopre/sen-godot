#pragma once

// std_fom
#include "rpr/rpr-base_v2.0.xml.h"

// generated code
#include "configuration.stl.h"

[[nodiscard]] inline std::string toString(const rpr::EntityTypeStruct& entityType)
{
    return std::to_string(entityType.category)
    + ":" + std::to_string(entityType.countryCode)
    + ":" + std::to_string(entityType.domain)
    + ":" + std::to_string(entityType.entityKind)
    + ":" + std::to_string(entityType.extra)
    + ":" + std::to_string(entityType.specific)
    + ":" + std::to_string(entityType.subcategory);
}

[[nodiscard]] inline configuration::Model getModel(const rpr::EntityTypeStruct& entityType, const configuration::ModelMappingList& mappings)
{
    const auto typeString = toString(entityType);

    for (const auto& [disTypes, model] : mappings)
    {
        if (const auto it = std::find(disTypes.begin(), disTypes.end(), typeString); it != disTypes.end())
        {
            return model;
        }
    }

    return configuration::Model::defaultModel;
}

[[nodiscard]] inline std::string_view getModelPath(const configuration::Model& model)
{
    switch (model)
    {
    case configuration::Model::defaultModel:
        return "";
    case configuration::Model::a400:
        return "a400";
    case configuration::Model::f18:
        return "f18";
    default:
        return "";
    }
    return "";
}
