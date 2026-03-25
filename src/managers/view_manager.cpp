#include "view_manager.h"
#include "../sen_node.h"

// godot
#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/script.hpp"
#include "godot_cpp/classes/viewport.hpp"

void ViewManager::setInterface(sen::Object* interface,  ComponentConfiguration* config)
{
    godot::UtilityFunctions::push_error("setInterface CALLED");

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

    if (const auto* attachedView = std::get_if<sen_ig_gateway::AttachedCamera>(&interface_->getViewType()); attachedView != nullptr)
    {
        const auto it = getConfig()->baseEntityManagers_->find(attachedView->entity);
        if (it == getConfig()->baseEntityManagers_->end())
        {
            return;
        }

        subViewContainer_->add_child(viewport_);
        viewport_->add_child(camera_);
        it->second->getEntityPivots()->roll->add_child(subViewContainer_);
    }

    RootManager::_ready();
}
