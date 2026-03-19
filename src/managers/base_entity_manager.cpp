#include "base_entity_manager.h"
#include "../utils.h"

// godot
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <sen/kernel/component_api.h>


void BaseEntityManager::setInterface(sen::Object* interface, sen::impl::WorkQueue* queue)
{
    interface_ = dynamic_cast<InterfaceType*>(interface);
    sen::util::DrConfig drConfig;
    drConfig.smoothing = true;
    deadReckoner_ = std::make_unique<sen::util::DeadReckoner<InterfaceType>>(*interface_, drConfig);
    entityType = toString(interface_->getEntityType());

    // guards_.emplace_back(interface_->onSpatialChanged({queue, []()
    // {
    //     godot::UtilityFunctions::print("Position changed!");
    // }}));

    RootManager::setInterface(interface, queue);
}

void BaseEntityManager::setNewGeoreference(const godot::Vector3& georeference)
{
    georeference_ = georeference;
}

void BaseEntityManager::componentUpdate(sen::kernel::RunApi* api)
{
    if (!interface_)
    {
        godot::UtilityFunctions::push_error("BaseEntityManager: invalid interface cast");
        return;
    }

    const auto situation = deadReckoner_->situation(api->getTime());
    const godot::Vector3 ecefOrientation{-situation.orientation.phi, -situation.orientation.theta, situation.orientation.psi};
    const godot::Vector3 ecefLocation {static_cast<float>(situation.worldLocation.x.get()), static_cast<float>(situation.worldLocation.y.get()), static_cast<float>(situation.worldLocation.z.get())};
    const auto finalRelativePosition = ecefLocation - georeference_;

    this->call_deferred("set_position", finalRelativePosition);
    this->call_deferred("set_rotation", ecefOrientation);
}

void BaseEntityManager::_ready()
{
    const godot::Ref<godot::PackedScene> scene = godot::ResourceLoader::get_singleton()->load("res://assets/f18.glb");
    if (!scene.is_valid())
    {
        godot::UtilityFunctions::push_error("Error loading resource");
        return;
    }

    if (Node* model = scene->instantiate(); model != nullptr)
    {
        model->set_name("model");
        // Make the model rotation offset configurable
        godot::Object::cast_to<godot::Node3D>(model)->call_deferred("set_rotation", godot::Vector3{-90.0f, 0.0f, 90.0f});
        this->call_deferred("add_child", model);
    }
}

void BaseEntityManager::_physics_process(double p_delta)
{
}
