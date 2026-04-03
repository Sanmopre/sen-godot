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
    godot::UtilityFunctions::push_error("setInterface CALLED");

    interface_ = dynamic_cast<InterfaceType*>(interface);


    // Callbacks
    guards_.emplace_back(interface_->onFovChanged({config->workQueue_, [this]()
    {
        const auto fovValue = interface_->getFov().left.get() + interface_->getFov().right.get();
        camera_->call_deferred("set_fov", fovValue);
    }}));

    guards_.emplace_back(interface_->onViewTypeChanged({config->workQueue_, [this]()
    {
        std::visit(
                 sen::Overloaded
                 {
                     [](const sen_ig_gateway::FreeCamera& freeCamera){},
                     [](const sen_ig_gateway::AttachedCamera& attachedCamera){},
                     [](const sen_ig_gateway::OrbitCamera& orbitCamera){},
                 },
                 interface_->getViewType());

    }}));

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
    camera_ = memnew(godot::Camera3D);
    subViewContainer_ = memnew(godot::SubViewportContainer);
    viewport_ = memnew(godot::Viewport);

    // set camera script
    godot::Ref<godot::Script> cam_script =
    godot::ResourceLoader::get_singleton()->load(
        "res://addons/cesium_godot/scripts/camera_controllers/AbstractCesiumCamera.gd"
    );
    camera_->set_script(cam_script);

    getConfig()->senNode_->createNewTileset({interface_->asObject().getName()});
    godot::Array tilesets;
    tilesets.push_back(getConfig()->senNode_->getTileset(interface_->asObject().getName()));

    camera_->set("globe_node", getConfig()->senNode_->getGeoreferenceNode());
    camera_->set("tilesets", tilesets);
    camera_->set("render_atmosphere", true);

    // Add camera and viewport to the scene
    get_tree()->get_current_scene()->add_child(subViewContainer_);
    subViewContainer_->add_child(viewport_);
    viewport_->add_child(camera_);

    RootManager::_ready();
}
