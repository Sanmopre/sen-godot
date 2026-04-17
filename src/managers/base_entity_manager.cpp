// ================================
//
// sen_godot
// File: base_entity_manager.cpp
//
// Author: Sanmopre
// Copyright (c) 2026 Santiago Moliner
//
// Licensed under the MIT License.
// See LICENSE file for details.
//
// ================================

#include "base_entity_manager.h"
#include "../utils.h"

// godot
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/label3d.hpp>
#include <godot_cpp/variant/node_path.hpp>
// sen
#include <sen/kernel/component_api.h>

#include "../sen_godot_component.h"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/classes/window.hpp"

void BaseEntityManager::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("align_belly_to_origin"), &BaseEntityManager::align_belly_to_origin);
    godot::ClassDB::bind_method(godot::D_METHOD("on_item_selected", "index"), &BaseEntityManager::on_item_selected);
}

void BaseEntityManager::setInterface(sen::Object* interface, ComponentConfiguration* config)
{
    interface_ = dynamic_cast<InterfaceType*>(interface);
    sen::util::DrConfig drConfig;
    drConfig.smoothing = true;
    deadReckoner_ = std::make_unique<sen::util::DeadReckoner<InterfaceType>>(*interface_, drConfig);

    RootManager::setInterface(interface, config);
}

void BaseEntityManager::align_belly_to_origin()
{
    const godot::Vector3 toCenter = (godot::Vector3(0, 0, 0) - get_global_position()).normalized();
    if (toCenter.length_squared() < 1e-12) {
        return;
    }

    const godot::Vector3 up = -toCenter;
    godot::Vector3 ref_forward(0, 0, -1);

    if (godot::Math::abs(up.dot(ref_forward)) > 0.99) {
        ref_forward = godot::Vector3(1, 0, 0);
    }

    const godot::Vector3 right = up.cross(ref_forward).normalized();
    const godot::Vector3 forward = right.cross(up).normalized();

    set_global_basis(godot::Basis(right, up, -forward));
}

Pivot* BaseEntityManager::getEntityPivots() noexcept
{
    return &pivot_;
}

godot::Node* BaseEntityManager::getModelNode() const noexcept
{
    return model_;
}

void BaseEntityManager::on_item_selected(int32_t index) const
{
    const auto metadata = getConfig()->uiComponents_->itemList->get_item_metadata(index);
    if (godot::String(metadata) == itemMetadataKey_)
    {
        cameraNode_->set("entity_to_follow", model_);
        // pitch, yaw, roll
        cameraNode_->set("follow_rotation_offset_degrees", godot::Vector3(0,90,0));
        cameraNode_->set("follow_offset", godot::Vector3(30,5,0));
    }
}

void BaseEntityManager::componentUpdate(sen::kernel::RunApi* api)
{
    if (!interface_)
    {
        godot::UtilityFunctions::push_error("BaseEntityManager: invalid interface cast");
        return;
    }

    // Set the ECEF position of the entity (Currently working when georeference node is set to true origin)
    const auto situation = deadReckoner_->situation(api->getTime());
    ecefLocation_ = godot::Vector3{situation.worldLocation.x.get(), situation.worldLocation.y.get(), situation.worldLocation.z.get()};

    // Set the orientation of the aircraft setting all the euler pivots
    if (model_ && pivot_.yaw && pivot_.pitch && pivot_.roll)
    {
        const auto geodeticSituation = deadReckoner_->geodeticSituation(api->getTime());
        rotation_.x = -geodeticSituation.orientation.phi;
        rotation_.y = geodeticSituation.orientation.psi;
        rotation_.z = geodeticSituation.orientation.theta;
    }
}

void BaseEntityManager::_ready()
{
    // Add the entity to the list of entities list
    const auto itemId = getConfig()->uiComponents_->itemList->add_item(interface_->asObject().getName().c_str());
    itemMetadataKey_ = godot::String(interface_->asObject().getLocalName().data());
    getConfig()->uiComponents_->itemList->set_item_metadata(itemId, itemMetadataKey_);

    getConfig()->uiComponents_->itemList->connect(
        "item_selected",
        godot::Callable(this, "on_item_selected"));

    pivot_.yaw = memnew(Node3D);
    pivot_.yaw->set_name("yaw_pivot");
    this->add_child(pivot_.yaw);

    pivot_.pitch = memnew(Node3D);
    pivot_.pitch->set_name("pitch_pivot");
    pivot_.yaw->add_child(pivot_.pitch);

    pivot_.roll = memnew(Node3D);
    pivot_.roll->set_name("roll_pivot");
    pivot_.pitch->add_child(pivot_.roll);

    const auto modelPath = getModelPath(getModel(interface_->getEntityType(),getConfig()->engineConfiguration_->getModelMappings()));

    const godot::Ref<godot::PackedScene> scene = godot::ResourceLoader::get_singleton()->load(modelPath.data());
    if (!scene.is_valid())
    {
        godot::UtilityFunctions::push_error("Error loading ", modelPath.data());
        return;
    }

    if (model_ = scene->instantiate(); model_ != nullptr)
    {
        model_->set_name("model");
        pivot_.roll->add_child(model_);
    }

    // Get node path
    cameraNode_ = get_tree()->get_root()->get_node_or_null(godot::NodePath("main/Camera"));

    // TODO: Add debug mode and show the labels, and make it always the same size
    // godot::Label3D* label = memnew(godot::Label3D);
    // label->set_text(interface_->asObject().getName().c_str());
    // label->set_position(godot::Vector3(0.0, 7.5, 0.0));
    // label->set_billboard_mode(godot::BaseMaterial3D::BILLBOARD_ENABLED);
    // label->set_pixel_size(1.0f);
    // label->set_draw_flag(godot::Label3D::FLAG_DISABLE_DEPTH_TEST, true);
    // pivot_.roll->add_child(label);
    // label->set_owner(pivot_.roll->get_owner());
}

void BaseEntityManager::_process(double p_delta)
{
    // Set ECEF position
    set_position(ecefLocation_);

    // Set orientation
    pivot_.yaw->set_rotation(godot::Vector3(0.0, rotation_.y, 0.0));
    pivot_.pitch->set_rotation(godot::Vector3(0.0, 0.0, rotation_.z));
    pivot_.roll->set_rotation(godot::Vector3(rotation_.x, 0.0, 0.0));

    // Make the entity have the belly point toward the center of the earth
    align_belly_to_origin();
}

void BaseEntityManager::_exit_tree()
{
    // Remove the entity from the entity list
    auto *item_list = getConfig()->uiComponents_->itemList;

    for (int i = 0; i < item_list->get_item_count(); i++)
    {
        if (const godot::Variant metadata = item_list->get_item_metadata(i); godot::String(metadata) == itemMetadataKey_)
        {
            item_list->remove_item(i);
            RootManager::_exit_tree();
            return;
        }
    }

    RootManager::_exit_tree();
}
