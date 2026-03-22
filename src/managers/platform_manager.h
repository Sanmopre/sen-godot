#pragma once

#include "physical_entity_manager.h"

// std_fom
#include "rpr/rpr-physical_v2.0.xml.h"

class PlatformManager : public PhysicalEntityManager
{
    GDCLASS(PlatformManager, PhysicalEntityManager)
protected:
    static void _bind_methods(){}

public:
    using InterfaceType = rpr::PlatformInterface;

public: // RootManager implementation
    void setInterface(sen::Object* interface, ComponentConfiguration* config) override
    {
        interface_ = dynamic_cast<InterfaceType*>(interface);
        PhysicalEntityManager::setInterface(interface, config);
    }

public: // godot::Node implementation
    //void _physics_process(double p_delta) override;
    // void _enter_tree() override;
    //void _exit_tree() override;
    // void _ready() override;

private:
    InterfaceType* interface_ = nullptr;
    std::string entityType;
};
