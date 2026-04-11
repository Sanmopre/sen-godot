#include "view_manager.h"
#include "../sen_node.h"

// godot
#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/classes/script.hpp"
#include "godot_cpp/classes/viewport.hpp"

void ViewManager::setInterface(sen::Object* interface,  ComponentConfiguration* config)
{
    interface_ = dynamic_cast<InterfaceType*>(interface);

    // Find the viewport configuration
    const auto& viewports = config->engineConfiguration_->getViewports();
    for (const auto& viewport : viewports)
    {
        if (interface->getName() == viewport.view)
        {
            currentViewport = &viewport;
        }
    }

    if (currentViewport == nullptr)
    {
        godot::UtilityFunctions::push_warning("View '",interface->getName().c_str(),"' not found in any viewport configuration");
        return;
    }

    RootManager::setInterface(interface, config);
}

void ViewManager::setEntityToAttach(BaseEntityManager* entity)
{
}

void ViewManager::componentUpdate(sen::kernel::RunApi* api)
{
    RootManager::componentUpdate(api);
}

void ViewManager::_ready()
{
    if (!interface_)
    {
        godot::UtilityFunctions::push_error("interface_ is NULL in _ready()");
        return;
    }

    if (!getConfig() || !getConfig()->senNode_)
    {
        godot::UtilityFunctions::push_error("Config or senNode is NULL");
        return;
    }

    // Register callbacks
    guards_.emplace_back(interface_->onFovChanged({getConfig()->workQueue_, [this](){fovChanged_ = true;}}));
    guards_.emplace_back(interface_->onViewTypeChanged({getConfig()->workQueue_, [this](){viewTypeChanged_ = true;}}));

    camera_ = memnew(godot::Camera3D);
    subViewContainer_ = memnew(godot::SubViewportContainer);
    viewport_ = memnew(godot::SubViewport);

    // set camera script
    godot::Ref<godot::Script> cam_script =
    godot::ResourceLoader::get_singleton()->load(
    "res://scripts/camera.gd"
    );

    if (cam_script.is_null())
    {
        godot::UtilityFunctions::push_error("Failed to load camera script!");
        return;
    }

    camera_->set_script(cam_script);

    // Add camera and viewport to the scene
    this->add_child(subViewContainer_);
    subViewContainer_->add_child(viewport_);
    viewport_->add_child(camera_);

    // Set the tileset for the view
    godot::TilesetConfiguration config;
    config.name = interface_->asObject().getName() + "_tileset";
    getConfig()->senNode_->createNewTileset(config);

    // Set the tileset name in the script
    camera_->set("tileset_name", config.name.c_str());

    get_viewport()->connect(
    "size_changed",
    callable_mp(this, &ViewManager::updateViewportLayout));
    updateViewportLayout();

    // invoke all property callbacks after the callbacks has been installed
    interface_->asObject().invokeAllPropertyCallbacks();

    RootManager::_ready();
}

void ViewManager::_process(double p_delta)
{
    if (fovChanged_)
    {
        // Do something
    }

    if (viewTypeChanged_)
    {
        manageViewTypeChange(interface_->getViewType());
    }

    // If the entity is attached and view and the entity has not been found yet try to find it
    if (!entityFound_ && attachedEntity_.has_value() && getConfig())
    {
        auto* entities = getConfig()->baseEntityManagers_;
        if (const auto it = entities->find(attachedEntity_.value()); it != entities->end())
        {
            // Only attach when the model node is available
            if (const auto* modelNode = it->second->getModelNode(); modelNode)
            {
                entityFound_ = true;
                const godot::NodePath path = modelNode->get_path();
                camera_->set("target_path", path);
            }
        }
    }

    fovChanged_ = false;
    viewTypeChanged_ = false;
    RootManager::_process(p_delta);
}

void ViewManager::updateViewportLayout()
{
    if (currentViewport)
    {
        const godot::Vector2 screen_size = get_viewport()->get_visible_rect().size;
        const godot::Vector2 pos = godot::Vector2(currentViewport->left, currentViewport->top) * screen_size;
        const godot::Vector2 size = godot::Vector2(currentViewport->right - currentViewport->left, currentViewport->bottom - currentViewport->top) * screen_size;

        subViewContainer_->set_position(pos);
        subViewContainer_->set_size(size);
        viewport_->set_size(size);
    }
}

void ViewManager::manageViewTypeChange(const sen_ig_gateway::ViewType& viewType)
{
    std::visit(
         sen::Overloaded {[&](const sen_ig_gateway::FreeCamera& val) { manageFreeCamera(val); },
                          [&](const sen_ig_gateway::AttachedCamera& val) { manageAttachedCamera(val); },
                          [&](const sen_ig_gateway::OrbitCamera& val) { manageOrbitCamera(val); }},
         viewType);
}

void ViewManager::manageFreeCamera(const sen_ig_gateway::FreeCamera& view)
{
    attachedEntity_ = std::nullopt;
    godot::UtilityFunctions::push_warning("FreeCamera not yet managed");
}

void ViewManager::manageAttachedCamera(const sen_ig_gateway::AttachedCamera& view)
{
    // Set the offset position
    std::visit(
     sen::Overloaded {[&](const sen_ig_gateway::ViewPoint& val)
     {
         godot::UtilityFunctions::push_warning("Viewpoint not yet managed");
         viewOffset_ = {0,0,0};
     },
      [&](const sen_ig_gateway::Situation& val)
      {
          viewOffset_.x = val.position.x;
          viewOffset_.y = val.position.y;
          viewOffset_.z = val.position.z;
          yaw_ = val.orientation.yaw;
          pitch_ = val.orientation.pitch;
          roll_ = val.orientation.roll;
      }
     },
     view.situation);

    // Set the properties
    camera_->set("offset", viewOffset_);
    camera_->set("yaw_offset", yaw_);
    camera_->set("pitch_offset", pitch_);
    camera_->set("roll_offset", roll_);

    // Check if the attached entity has changed
    if (attachedEntity_.has_value())
    {
        if (attachedEntity_.value() == view.entity)
        {
            return;
        }
    }

    // Entity has changed
    entityFound_ = false;
    attachedEntity_ = view.entity;
    auto* entities = getConfig()->baseEntityManagers_;
    if (const auto it = entities->find(attachedEntity_.value()); it != entities->end())
    {
        entityFound_ = true;
        const godot::NodePath path = it->second->getModelNode()->get_path();
        camera_->set("target_path", path);
    }
    else
    {
        godot::UtilityFunctions::push_warning("Entity ", view.entity.c_str(), " not found!");
    }

}

void ViewManager::manageOrbitCamera(const sen_ig_gateway::OrbitCamera& view)
{
    godot::UtilityFunctions::push_warning("OrbitCamera not yet managed");
}
