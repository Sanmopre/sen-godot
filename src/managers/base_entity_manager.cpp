#include "base_entity_manager.h"
#include "../utils.h"

// godot
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/core/class_db.hpp>


void BaseEntityManager::setInterface(sen::Object* interface, sen::impl::WorkQueue* queue)
{
    interface_ = dynamic_cast<InterfaceType*>(interface);
    entityType = toString(interface_->getEntityType());

    // guards_.emplace_back(interface_->onSpatialChanged({queue, []()
    // {
    //     godot::UtilityFunctions::print("Position changed!");
    // }}));

    RootManager::setInterface(interface, queue);
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
        this->call_deferred("add_child", model);
    }
}

void BaseEntityManager::_physics_process(double p_delta)
{
    if (!interface_)
    {
        godot::UtilityFunctions::push_error("BaseEntityManager: invalid interface cast");
        return;
    }

    std::ignore = interface_->getSpatial();

    RootManager::_physics_process(p_delta);
}
