#pragma once

#include "root_manager.h"
#include "base_entity_manager.h"

// generated code
#include "godot_cpp/classes/camera3d.hpp"
#include "ig_interface/view.stl.h"

class ViewManager : public RootManager
{
    GDCLASS(ViewManager, RootManager)
protected:
    static void _bind_methods(){};

public:
    using InterfaceType = sen_ig_gateway::ViewInterface;

public: // RootManager implementation
    void setInterface(sen::Object* interface, ComponentConfiguration* config) override;
    void setEntityToAttach(BaseEntityManager* entity);

public:
    void componentUpdate(sen::kernel::RunApi* api) override;

public: // godot::Node implementation
    void _ready() override;

private:
    InterfaceType* interface_ = nullptr;
    const configuration::Viewport* currentViewport = nullptr;
    godot::Camera3D* camera_ = nullptr;
};

