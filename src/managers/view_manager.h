#pragma once

#include "root_manager.h"
#include "base_entity_manager.h"

class ViewManager : public RootManager
{
    GDCLASS(ViewManager, RootManager)
protected:
    static void _bind_methods(){};

public:
    using InterfaceType = sen::Object;

public: // RootManager implementation
    void setInterface(sen::Object* interface, ComponentConfiguration* config) override;
    void setEntityToAttach(BaseEntityManager* entity);

public:
    void componentUpdate(sen::kernel::RunApi* api) override;

public: // godot::Node implementation
    void _ready() override;

private:
    InterfaceType* interface_ = nullptr;
};

