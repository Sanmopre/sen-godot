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

    godot::UtilityFunctions::push_warning("Entity with dis type ", typeString.data(), " not mapped to any model");
    return configuration::Model::defaultModel;
}

[[nodiscard]] inline std::string_view getModelPath(const configuration::Model& model)
{
    switch (model)
    {
    case configuration::Model::defaultModel:
        return "res://assets/f18/f18.tscn";
    case configuration::Model::a400:
        return "res://assets/f18/f18.tscn";
    case configuration::Model::f18:
        return "res://assets/f18/f18.tscn";
    case configuration::Model::chaff:
        return "res://assets/chaff/chaff.tscn";
    case configuration::Model::flare:
        return "res://assets/chaff/chaff.tscn";
    case configuration::Model::c17:
        return "res://assets/c-17/c-17.tscn";
    case configuration::Model::aim9:
        return "res://assets/aim-9/aim-9.tscn";
    default:
        return "res://assets/f18/f18.tscn";
    }
}
