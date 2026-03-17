#pragma once

#include "root_manager.h"

// std_fom
#include "rpr/rpr-base_v2.0.xml.h"

// sen
#include "sen/util/dr/dead_reckoner.h"

class BaseEntityManager : public RootManager
{
    GDCLASS(BaseEntityManager, RootManager)
protected:
    static void _bind_methods(){}

public:
    using InterfaceType = rpr::BaseEntityInterface;

public: // RootManager implementation
    void setInterface(sen::Object* interface, sen::impl::WorkQueue* queue) override;

public: // godot::Node implementation
    void _ready() override;
    void _physics_process(double p_delta) override;
    //void _exit_tree() override;
    // void _ready() override;

private:
    InterfaceType* interface_ = nullptr;
    std::vector<sen::ConnectionGuard> guards_;
    std::string entityType;
};

