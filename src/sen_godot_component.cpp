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
    //api.getTypes().add(rpr::AircraftInterface::meta());
    // Windows only?
    api.getTypes().add(rpr::AircraftInterface::meta().shared_from_this());

    aircraftSubscription_ = api.selectAllFrom<rpr::AircraftInterface>("test.bus");
    std::ignore = aircraftSubscription_->list.onAdded([this](const sen::ObjectList<rpr::AircraftInterface>::Iterators &objects)
    {
        for (auto object = objects.typedBegin; object != objects.typedEnd; ++object)
        {
            auto* newInstance = memnew(AircraftManager);
            newInstance->setInterface( dynamic_cast<sen::Object*>(*object));
            senNode_->addNode<AircraftManager>(newInstance);
            aircraftManagers_.try_emplace((*object)->asObject().getLocalName(), newInstance);
        }
    });

    std::ignore = aircraftSubscription_->list.onRemoved([this](const sen::ObjectList<rpr::AircraftInterface>::Iterators &objects)
    {
        for (auto object = objects.typedBegin; object != objects.typedEnd; ++object)
        {
            aircraftManagers_.at((*object)->asObject().getLocalName())->queue_free();
            aircraftManagers_.erase((*object)->asObject().getLocalName());
        }
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

