#pragma once

// godot
#include "root_manager.h"
#include "rpr/rpr-physical_v2.0.xml.h"

class AircraftManager : public RootManager
{
    GDCLASS(AircraftManager, RootManager)
protected:
    static void _bind_methods(){}

public:
    using InterfaceType = rpr::AircraftInterface;

public: // RootManager implementation
    void setInterface(sen::Object* interface) override;

public: // godot::Node implementation
    //void _physics_process(double p_delta) override;
   // void _enter_tree() override;
    //void _exit_tree() override;
   // void _ready() override;

private:
    InterfaceType* interface_ = nullptr;
};
