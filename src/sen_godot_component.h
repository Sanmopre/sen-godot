#pragma once

#include "managers/aircraft_manager.h"

// sen
#include <sen/kernel/test_kernel.h>
#include "sen/core/obj/object_mux.h"

// std
#include <memory>
#include <string>
#include <unordered_map>

// generated code
#include "rpr/rpr-physical_v2.0.xml.h"
#include "configuration.stl.h"


namespace godot
{
    class SenNode;
}

class SenGodotComponent : public sen::kernel::Component
{
public:
    SenGodotComponent(godot::SenNode* senNode, godot::Node* georeferenceNode, const sen::Duration& tickDuration);
    ~SenGodotComponent() override = default;

    sen::kernel::FuncResult load(sen::kernel::LoadApi&&) override;
    sen::kernel::PassResult init(sen::kernel::InitApi&& api) override;
    sen::kernel::FuncResult run(sen::kernel::RunApi& api) override;
    sen::kernel::FuncResult unload(sen::kernel::UnloadApi&& api) override;

private:
    void runImpl(sen::kernel::RunApi& api);
    void subscribeToQueries(sen::kernel::InitApi& api);
    void onObjectAdded(sen::Object* object);
    void onObjectRemoved(sen::Object* object);

private:
    godot::SenNode* senNode_;
    std::shared_ptr<configuration::EngineConfigurationBase> engineConfiguration_;
    godot::Node* georeferenceNode_;
    sen::Duration tickDuration_;
    sen::impl::WorkQueue* workQueue_;

private: // Sen subscription objects
    std::unordered_map<std::string, std::shared_ptr<sen::ObjectSource>> sources_;
    std::unique_ptr<sen::ObjectList<sen::Object>> trackedObjects_;
    std::unique_ptr<sen::ObjectMux> mux_;

private: // Managers
    std::unordered_map<std::string, BaseEntityManager*> baseEntityManagers_;
};


