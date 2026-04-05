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
    godot::UtilityFunctions::push_error("begin setInterface()");
    interface_ = dynamic_cast<InterfaceType*>(interface);

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
    godot::UtilityFunctions::push_error("end setInterface()");
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

    camera_ = memnew(godot::Camera3D);
    subViewContainer_ = memnew(godot::SubViewportContainer);
    viewport_ = memnew(godot::Viewport);

    // set camera script
    godot::Ref<godot::Script> cam_script =
    godot::ResourceLoader::get_singleton()->load(
    "res://scripts/camera.gd"
    );

    if (cam_script.is_null()) {
        godot::UtilityFunctions::push_error("Failed to load camera script!");
        return;
    }

    camera_->set_script(cam_script);

    //getConfig()->senNode_->createNewTileset({interface_->asObject().getName()});
    godot::Array tilesets;
    tilesets.push_back(getConfig()->senNode_->getTileset(interface_->asObject().getName()));

    //camera_->set("globe_node", getConfig()->senNode_->getGeoreferenceNode());
    camera_->set("tilesets", tilesets);
    //camera_->set("render_atmosphere", true);

    // Add camera and viewport to the scene
    this->add_child(subViewContainer_);
    subViewContainer_->add_child(viewport_);
    viewport_->add_child(camera_);

    RootManager::_ready();
}

void ViewManager::_process(double p_delta)
{
    //
    // if (viewTypeChanged_)
    // {
    //     godot::UtilityFunctions::push_warning("VIEW TYPE CHANGED");
    // }


    fovChanged_ = false;
    viewTypeChanged_ = false;
    RootManager::_process(p_delta);
}
