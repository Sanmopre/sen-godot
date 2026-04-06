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

void SenNode::set_tileset_paths(const godot::Array& tilesets)
{
    tilesetsPaths_ = tilesets;
    tilesetsNodes_.clear();
    for (int i = 0; i < tilesetsPaths_.size(); ++i)
    {
        godot::NodePath path = tilesetsPaths_[i];
        godot::Node *node = get_node_or_null(path);
        if (node)
        {
            tilesetsNodes_.push_back(node);
        }
    }
}

Array SenNode::get_tileset_paths() const
{
    return tilesetsPaths_;
}

Node* SenNode::getGeoreferenceNode() const noexcept
{
    return georeferenceNode_;
}

Node* SenNode::getTileset(const std::string& name) noexcept
{
    if (const auto it = tilesets_.find(name); it != tilesets_.end())
    {
        return tilesets_.at(name);
    }

    UtilityFunctions::push_error("Tileset ", name.c_str(), " not found");
    return nullptr;
}


void SenNode::createNewTileset(const TilesetConfiguration& config)
{
    Object *obj = ClassDB::instantiate("Cesium3DTileset");
    if (!obj)
    {
        UtilityFunctions::push_error("Can not create Cesium3DTileset");
    }

    if (const auto it = tilesets_.find(config.name); it != tilesets_.end())
    {
        UtilityFunctions::print("Tileset for ",config.name.c_str()," already exists");
        return;
    }


    auto* newTileset = Object::cast_to<Node>(obj);

    newTileset->set_name(config.name.c_str());
    newTileset->set("maximum_screen_space_error", config.maximumScreenSpaceError);
    newTileset->set("maximum_simultaneous_tile_loads", config.maximumSimultaneousTileLoads);
    newTileset->set("ion_asset_id", config.assetId);
    newTileset->set("loading_descendant_limit", config.loadingDescendantLimit);
    newTileset->set("preload_ancestors", true);
    newTileset->set("preload_siblings", true);
    newTileset->set("forbid_holes", true);
    newTileset->set("create_physics_meshes", false);
    georeferenceNode_->add_child(newTileset);
    tilesets_.try_emplace(config.name, newTileset);
}

void SenNode::deleteTileset(const std::string& name)
{
    if (const auto it = tilesets_.find(name); it == tilesets_.end())
    {
        UtilityFunctions::push_warning("Trying to remove tileset ",name.c_str() ," that doesn't exists");
        return;
    }

    tilesets_.at(name)->queue_free();
    tilesets_.erase(name);
}

void SenNode::_ready()
{
    georeferenceNode_ = get_node_or_null(georeferencePath_);
    if (!georeferenceNode_)
    {
        UtilityFunctions::push_error("Georeference node not set");
    }

    const auto millisecondsPerTick = static_cast<i64>(1000.0 / Engine::get_singleton()->get_physics_ticks_per_second());
    component_ = std::make_shared<SenGodotComponent>(this, georeferenceNode_, std::chrono::milliseconds(millisecondsPerTick));

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

    godot::ClassDB::bind_method(
        godot::D_METHOD("set_tileset_paths", "paths"),
        &SenNode::set_tileset_paths
    );
    godot::ClassDB::bind_method(
        godot::D_METHOD("get_tileset_paths"),
        &SenNode::get_tileset_paths
    );

    ADD_PROPERTY(
        godot::PropertyInfo(godot::Variant::NODE_PATH, "georeference_path"),
        "set_georeference_path",
        "get_georeference_path"
    );

    ADD_PROPERTY(
        godot::PropertyInfo(
            godot::Variant::ARRAY,
            "tileset_paths",
            godot::PROPERTY_HINT_TYPE_STRING,
            godot::String::num(godot::Variant::NODE_PATH) + ":"
        ),
        "set_tileset_paths",
        "get_tileset_paths"
    );
}

}
