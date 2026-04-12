#pragma once

#include "managers/aircraft_manager.h"

// godot
#include "godot_cpp/classes/item_list.hpp"

// sen
#include <sen/kernel/test_kernel.h>
#include "sen/core/obj/object_mux.h"

// std
#include <memory>
#include <string>
#include <unordered_map>

// generated code
#include "configuration.stl.h"

namespace godot
{
    class SenNode;
}

struct UI_Components
{
    godot::ItemList* itemList = nullptr;
};

class SenGodotComponent : public sen::kernel::Component
{
public:
    SenGodotComponent(godot::SenNode* senNode, UI_Components* uiComponents, godot::Node* georeferenceNode, const sen::Duration& tickDuration);
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
    ComponentConfiguration config_;
    UI_Components* uiComponents_;
    godot::Node* georeferenceNode_;
    sen::Duration tickDuration_;

private: // Sen subscription objects
    std::unordered_map<std::string, std::shared_ptr<sen::ObjectSource>> sources_;
    std::unique_ptr<sen::ObjectList<sen::Object>> trackedObjects_;
    std::unique_ptr<sen::ObjectMux> mux_;

private: // Managers
    std::unordered_map<std::string, BaseEntityManager*> baseEntityManagers_;
};


