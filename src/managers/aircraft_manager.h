#pragma once

// godot
#include "root_manager.h"
#include "src/fom/aircraft.stl.h"

class AircraftManager : public RootManager
{
    GDCLASS(AircraftManager, RootManager)
protected:
    static void _bind_methods(){}

public:
    using InterfaceType = std_fom::AircraftInterface;

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
