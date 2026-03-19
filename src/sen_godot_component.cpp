#include "sen_godot_component.h"

// godot
#include "sen_node.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include "managers/aircraft_manager.h"

SenGodotComponent::SenGodotComponent(godot::SenNode* senNode, godot::Node* georeferenceNode, const sen::Duration& tickDuration)
    : senNode_(senNode), georeferenceNode_(georeferenceNode), tickDuration_(tickDuration)
{
}

sen::kernel::FuncResult SenGodotComponent::load(sen::kernel::LoadApi&& load_api)
{
    return Component::load(std::move(load_api));
}

sen::kernel::PassResult SenGodotComponent::init(sen::kernel::InitApi&& api)
{
#ifdef _WIN32
    api.getTypes().addc(rpr::AircraftInterface::meta().shared_from_this());
#elif __linux__
    api.getTypes().add(rpr::AircraftInterface::meta());
#endif

    aircraftSubscription_ = api.selectAllFrom<rpr::AircraftInterface>("ig.result");
    std::ignore = aircraftSubscription_->list.onAdded([this, &api](const sen::ObjectList<rpr::AircraftInterface>::Iterators &objects)
    {
        for (auto object = objects.typedBegin; object != objects.typedEnd; ++object)
        {
            auto* newInstance = memnew(AircraftManager);
            newInstance->setInterface( dynamic_cast<sen::Object*>(*object), api.getWorkQueue());
            newInstance->set_name((*object)->asObject().getLocalName().c_str());
            senNode_->getGeoreferenceNode()->call_deferred("add_child", newInstance);
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
    for (const auto& [name, manager] : aircraftManagers_)
    {
        manager->setNewGeoreference(senNode_->getGeoreferenceEcefValue());
        manager->componentUpdate(&api);
    }
}

