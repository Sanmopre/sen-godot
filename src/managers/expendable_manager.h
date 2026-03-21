#pragma once

#include "physical_entity_manager.h"

// std_fom
#include "rpr/rpr-physical_v2.0.xml.h"

class ExpendableManager : public PhysicalEntityManager
{
    GDCLASS(ExpendableManager, PhysicalEntityManager)
protected:
    static void _bind_methods(){}

public:
    using InterfaceType = rpr::ExpendablesInterface;

public: // RootManager implementation
    void setInterface(sen::Object* interface, sen::impl::WorkQueue* queue) override
    {
        interface_ = dynamic_cast<InterfaceType*>(interface);
        PhysicalEntityManager::setInterface(interface, queue);
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
