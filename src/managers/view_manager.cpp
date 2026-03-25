#include "view_manager.h"

// godot
#include "../sen_node.h"
#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/classes/resource_loader.hpp"

void ViewManager::setInterface(sen::Object* interface,  ComponentConfiguration* config)
{
    interface_ = dynamic_cast<InterfaceType*>(interface);
    camera_ = memnew(godot::Camera3D);

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

     if (const auto* attachedView = std::get_if<sen_ig_gateway::AttachedCamera>(&interface_->getViewType()); attachedView != nullptr)
     {
         const auto it = config->baseEntityManagers_->find(attachedView->entity);
         if (it == config->baseEntityManagers_->end())
         {
             return;
         }

         it->second->call_deferred("add_child", this);
         this->call_deferred("add_child", camera_);
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
    RootManager::_ready();
}
