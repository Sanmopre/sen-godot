#pragma once

#include "platform_manager.h"

// godot
#include "rpr/rpr-physical_v2.0.xml.h"

class AircraftManager : public PlatformManager
{
    GDCLASS(AircraftManager, PlatformManager)
protected:
    static void _bind_methods(){}

public:
    using InterfaceType = rpr::AircraftInterface;

public: // RootManager implementation
    void setInterface(sen::Object* interface, ComponentConfiguration* config) override
    {
        interface_ = dynamic_cast<InterfaceType*>(interface);
        PlatformManager::setInterface(interface, config);
    }

public: // godot::Node implementation
    //void _physics_process(double p_delta) override;
   // void _enter_tree() override;
    //void _exit_tree() override;
   // void _ready() override;

private:
    InterfaceType* interface_ = nullptr;
};
