#pragma once

#include "managers/aircraft_manager.h"

// sen
#include <sen/kernel/test_kernel.h>

// std
#include <memory>
#include <string>
#include <unordered_map>

// generated code
#include "fom/aircraft.stl.h"

namespace godot
{
    class SenNode;
}

class SenGodotComponent : public sen::kernel::Component
{
public:
    SenGodotComponent(godot::SenNode* senNode, const sen::Duration& tickDuration);
    ~SenGodotComponent() override = default;

    sen::kernel::FuncResult load(sen::kernel::LoadApi&&) override;
    sen::kernel::PassResult init(sen::kernel::InitApi&& api) override;
    sen::kernel::FuncResult run(sen::kernel::RunApi& api) override;
    sen::kernel::FuncResult unload(sen::kernel::UnloadApi&& api) override;

private:
    void runImpl(sen::kernel::RunApi& api);

private:
    godot::SenNode* senNode_;
    sen::Duration tickDuration_;
    std::unordered_map<std::string, std::shared_ptr<sen::ObjectSource>> sources_;
    std::shared_ptr<sen::Subscription<std_fom::AircraftInterface>> aircraftSubscription_;
    std::unordered_map<std::string, AircraftManager*> aircraftManagers_;
};


