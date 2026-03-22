#include "base_entity_manager.h"
#include "../utils.h"

// godot
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <sen/kernel/component_api.h>


void BaseEntityManager::_bind_methods()
{
    godot::ClassDB::bind_method(godot::D_METHOD("align_belly_to_origin"), &BaseEntityManager::align_belly_to_origin);
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

void BaseEntityManager::componentUpdate(sen::kernel::RunApi* api)
{
    if (!interface_)
    {
        godot::UtilityFunctions::push_error("BaseEntityManager: invalid interface cast");
        return;
    }

    // Set the ECEF position of the entity (Currently working when georeference node is set to true origin)
    const auto situation = deadReckoner_->situation(api->getTime());
    const godot::Vector3 ecefLocation {situation.worldLocation.x.get(), situation.worldLocation.y.get(), situation.worldLocation.z.get()};
    this->call_deferred("set_position", ecefLocation);

    // Set the orientation of the aircraft setting all the euler pivots
    if (model_ && pivot_.yaw && pivot_.pitch && pivot_.roll)
    {
        const auto geodeticSituation = deadReckoner_->geodeticSituation(api->getTime());
        pivot_.yaw->call_deferred("set_rotation", godot::Vector3(0.0, geodeticSituation.orientation.psi, 0.0));
        pivot_.pitch->call_deferred("set_rotation", godot::Vector3(0.0, 0.0, geodeticSituation.orientation.theta));
        pivot_.roll->call_deferred("set_rotation", godot::Vector3(-geodeticSituation.orientation.phi, 0.0, 0.0));
    }

    // Make the entity have the belly point toward the center of the earth
    call_deferred("align_belly_to_origin");
}

void BaseEntityManager::_ready()
{
    pivot_.yaw = memnew(Node3D);
    pivot_.yaw->set_name("yaw_pivot");
    this->call_deferred("add_child", pivot_.yaw);

    pivot_.pitch = memnew(Node3D);
    pivot_.pitch->set_name("pitch_pivot");
    pivot_.yaw->call_deferred("add_child", pivot_.pitch);

    pivot_.roll = memnew(Node3D);
    pivot_.roll->set_name("roll_pivot");
    pivot_.pitch->call_deferred("add_child", pivot_.roll);

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
        pivot_.roll->call_deferred("add_child", model_);
    }
}
