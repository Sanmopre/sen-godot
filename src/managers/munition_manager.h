#pragma once

#include "physical_entity_manager.h"

// std_fom
#include "rpr/rpr-physical_v2.0.xml.h"

class MunitionManager : public PhysicalEntityManager
{
    GDCLASS(MunitionManager, PhysicalEntityManager)
protected:
    static void _bind_methods(){}

public:
    using InterfaceType = rpr::MunitionInterface;

public: // RootManager implementation
    void setInterface(sen::Object* interface, ComponentConfiguration* config) override
    {
        interface_ = dynamic_cast<InterfaceType*>(interface);
        PhysicalEntityManager::setInterface(interface, config);
    }

private:
    InterfaceType* interface_ = nullptr;
};
