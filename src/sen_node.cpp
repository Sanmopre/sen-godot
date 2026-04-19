// ================================
//
// sen_godot
// File: sen_node.cpp
//
// Author: Sanmopre
// Copyright (c) 2026 Santiago Moliner
//
// Licensed under the MIT License.
// See LICENSE file for details.
//
// ================================

#include "sen_node.h"

// sen
#include "sen/kernel/test_kernel.h"
#include "sen/kernel/bootloader.h"

// godot
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/classes/resource_loader.hpp"

namespace godot
{

void SenNode::set_georeference_path(const godot::NodePath& path)
{
    georeferencePath_ = path;
}

NodePath SenNode::get_georeference_path() const
{
    return georeferencePath_;
}

Node* SenNode::getGeoreferenceNode() const noexcept
{
    return georeferenceNode_;
}

void SenNode::_ready()
{
    uiComponents_ = std::make_unique<UI_Components>();

    if (!item_list_path_.is_empty())
    {
        uiComponents_->itemList = get_node<ItemList>(item_list_path_);

        if (!uiComponents_->itemList)
        {
            UtilityFunctions::push_error("ItemList path is not an ItemList!");
        }
    }

    if (!debug_mode_path_.is_empty())
    {
        uiComponents_->debugMode = get_node<CheckButton>(debug_mode_path_);

        if (!uiComponents_->debugMode)
        {
            UtilityFunctions::push_error("DebugMode path is not a CheckButton!");
        }
    }


    georeferenceNode_ = get_node_or_null(georeferencePath_);
    if (!georeferenceNode_)
    {
        UtilityFunctions::push_error("Georeference node not set");
    }

    const auto millisecondsPerTick = static_cast<i64>(1000.0 / Engine::get_singleton()->get_physics_ticks_per_second());
    component_ = std::make_shared<SenGodotComponent>(this,uiComponents_.get(), georeferenceNode_, std::chrono::milliseconds(millisecondsPerTick));

    const auto bootConfigGlobalPath = godot::ProjectSettings::get_singleton()->globalize_path("res://config/boot.yaml");
    const auto bootLoader = sen::kernel::Bootloader::fromYamlFile(bootConfigGlobalPath.utf8().get_data(), false);

    sen::kernel::ComponentContext component;
    component.instance = component_.get();
    component.info.name = "sen_godot";
    component.info.description = "Sen-Godot extension";
    component.info.buildInfo = {};
    component.config.cpuAffinity = 0xFF;
    component.config.group = 2;
    component.config.priority = sen::kernel::Priority::nominalMin;
    component.config.stackSize = 0;
    component.config.inQueue.evictionPolicy = sen::kernel::QueueEvictionPolicy::dropOldest;
    component.config.inQueue.maxSize = 0;
    component.config.outQueue.evictionPolicy = sen::kernel::QueueEvictionPolicy::dropOldest;
    component.config.outQueue.maxSize = 0;
    component.config.sleepPolicy = sen::kernel::SystemSleep{};

    sen::kernel::ComponentConfig config;
    config.cpuAffinity = 0xFF;
    config.group = 2;
    config.priority = sen::kernel::Priority::nominalMin;
    config.stackSize = 0;
    config.inQueue.evictionPolicy = sen::kernel::QueueEvictionPolicy::dropOldest;
    config.inQueue.maxSize = 0;
    config.outQueue.evictionPolicy = sen::kernel::QueueEvictionPolicy::dropOldest;
    config.outQueue.maxSize = 0;
    config.sleepPolicy = sen::kernel::SystemSleep{};

    const sen::kernel::KernelConfig::ComponentToLoad componentConfig
    {
        std::move(component),
        config,
        {} // empty params
    };

    bootLoader->getConfig().addToLoad(componentConfig);
    kernel_ = std::make_unique<sen::kernel::TestKernel>(bootLoader->getConfig());
}

void SenNode::_process(double p_delta)
{
    kernel_->step();
    Node::_process(p_delta);
}

void SenNode::_bind_methods()
{
    godot::ClassDB::bind_method(
        godot::D_METHOD("set_georeference_path", "path"),
        &SenNode::set_georeference_path
    );
    godot::ClassDB::bind_method(
        godot::D_METHOD("get_georeference_path"),
        &SenNode::get_georeference_path
    );
    ClassDB::bind_method(D_METHOD("set_item_list_path", "path"), &SenNode::set_item_list_path);
    ClassDB::bind_method(D_METHOD("get_item_list_path"), &SenNode::get_item_list_path);

    ClassDB::bind_method(D_METHOD("set_debug_mode_path", "path"), &SenNode::set_debug_mode_path);
    ClassDB::bind_method(D_METHOD("get_debug_mode_path"), &SenNode::get_debug_mode_path);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "item_list_path"),
                 "set_item_list_path", "get_item_list_path");

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "debug_mode_path_"),
                 "set_debug_mode_path", "get_debug_mode_path");

    ADD_PROPERTY(
        godot::PropertyInfo(godot::Variant::NODE_PATH, "georeference_path"),
        "set_georeference_path",
        "get_georeference_path"
    );
}

}
