#include "sen_godot_component.h"

// godot
#include "sen_node.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include "managers/aircraft_manager.h"

SenGodotComponent::SenGodotComponent(godot::SenNode* senNode, const sen::Duration& tickDuration)
    : senNode_(senNode), tickDuration_(tickDuration)
{
}

sen::kernel::FuncResult SenGodotComponent::load(sen::kernel::LoadApi&& load_api)
{
    return Component::load(std::move(load_api));
}

sen::kernel::PassResult SenGodotComponent::init(sen::kernel::InitApi&& api)
{
    aircraftSubscription_ = api.selectAllFrom<std_fom::AircraftInterface>("test.bus");
    std::ignore = aircraftSubscription_->list.onAdded([this](const auto& iterator)
    {
        auto* object = memnew(AircraftManager);
        senNode_->addNode<AircraftManager>(object);
        aircraftManagers_.try_emplace("babbui", object);
    });

    std::ignore = aircraftSubscription_->list.onRemoved([this](const auto& iterator)
    {

    });

    return Component::init(std::move(api));
}

sen::kernel::FuncResult SenGodotComponent::run(sen::kernel::RunApi& api)
{
    return api.execLoop(tickDuration_, [this, &api](){ this->runImpl(api);});
}

sen::kernel::FuncResult SenGodotComponent::unload(sen::kernel::UnloadApi&& api)
{
    return Component::unload(std::move(api));
}

void SenGodotComponent::runImpl(sen::kernel::RunApi& api)
{
   //godot::UtilityFunctions::print("Seconds since component start: ", api.getTime().sinceEpoch().toSeconds());
}

