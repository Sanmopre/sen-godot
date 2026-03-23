#include "view_manager.h"

// godot
#include "../sen_node.h"
#include "godot_cpp/classes/packed_scene.hpp"
#include "godot_cpp/classes/resource_loader.hpp"

void ViewManager::setInterface(sen::Object* interface,  ComponentConfiguration* config)
{
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
