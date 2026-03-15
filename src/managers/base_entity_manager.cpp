#include "base_entity_manager.h"
#include "../utils.h"

// godot
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/core/class_db.hpp>


void BaseEntityManager::setInterface(sen::Object* interface)
{
    interface_ = dynamic_cast<InterfaceType*>(interface);
    entityType = toString(interface_->getEntityType());

    RootManager::setInterface(interface);
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
        add_child(model);
    }
}
