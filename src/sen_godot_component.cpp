#include "sen_godot_component.h"

// godot
#include <sen/kernel/bootloader.h>

#include "sen_node.h"
#include "godot_cpp/classes/project_settings.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "managers/aircraft_manager.h"

// sen
#include "managers/expendable_manager.h"
#include "managers/munition_manager.h"
#include "sen/core/meta/class_type.h"

SenGodotComponent::SenGodotComponent(godot::SenNode* senNode, godot::Node* georeferenceNode, const sen::Duration& tickDuration)
    :
    senNode_(senNode)
    , georeferenceNode_(georeferenceNode)
    , tickDuration_(tickDuration)
    , trackedObjects_(std::make_unique<sen::ObjectList<sen::Object>>())
    ,mux_(std::make_unique<sen::ObjectMux>())
{
}

sen::kernel::FuncResult SenGodotComponent::load(sen::kernel::LoadApi&& load_api)
{
    return Component::load(std::move(load_api));
}

sen::kernel::PassResult SenGodotComponent::init(sen::kernel::InitApi&& api)
{
#ifdef _WIN32
    api.getTypes().add(rpr::AircraftInterface::meta().shared_from_this());
    api.getTypes().add(rpr::MunitionInterface::meta().shared_from_this());
    api.getTypes().add(rpr::ExpendablesInterface::meta().shared_from_this());
#elif __linux__
    api.getTypes().add(rpr::AircraftInterface::meta());
    api.getTypes().add(rpr::MunitionInterface::meta());
    api.getTypes().add(rpr::ExpendablesInterface::meta());
#endif

    config_.workQueue_ = api.getWorkQueue();
    const auto configGlobalPath = godot::ProjectSettings::get_singleton()->globalize_path("res://config/configuration.yaml");
    const auto varMap = sen::kernel::getConfigAsVarFromYaml(configGlobalPath.utf8().get_data(), false);
    config_.engineConfiguration_ = std::make_shared<configuration::EngineConfigurationBase>("engine_config", varMap);
    subscribeToQueries(api);
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
    for (const auto& [name, manager] : baseEntityManagers_)
    {
        manager->componentUpdate(&api);
    }
}

void SenGodotComponent::subscribeToQueries(sen::kernel::InitApi& api)
{
    std::vector<std::shared_ptr<sen::Interest>> interests;
    const auto& inputQueries = config_.engineConfiguration_->getInputQueries();
    interests.reserve(inputQueries.size());

    for (const auto& query : inputQueries)
    {
        interests.emplace_back(sen::Interest::make(query, api.getTypes()));
    }

    for (const auto& interest : interests)
    {
        auto busName = sen::asString(interest->getBusCondition().value());
        auto itr = sources_.find(busName);
        if (itr == sources_.end())
        {
            auto [newItr, added] = sources_.try_emplace(busName, api.getSource(busName));
            itr = newItr;
        }

        itr->second->addSubscriber(interest, mux_.get(), true);
    }

    std::ignore = trackedObjects_->onAdded([this](const auto& iterators)
    {
        for (auto itr = iterators.untypedBegin; itr != iterators.untypedEnd; ++itr)
        {
            onObjectAdded((*itr).get());
        }
    });

    std::ignore = trackedObjects_->onRemoved([this](const auto& iterators)
    {
        for (auto itr = iterators.untypedBegin; itr != iterators.untypedEnd; ++itr)
        {
            onObjectRemoved((*itr).get());
        }
    });

    mux_->addListener(trackedObjects_.get(), true);
}

void SenGodotComponent::onObjectAdded(sen::Object* object)
{
    if (object->getClass()->isSameOrInheritsFrom(*rpr::AircraftInterface::meta().type()))
    {
        auto* newInstance = memnew(AircraftManager);
        newInstance->setInterface( object, &config_);
        newInstance->set_name(object->getLocalName().c_str());
        senNode_->getGeoreferenceNode()->call_deferred("add_child", newInstance);
        baseEntityManagers_.try_emplace(object->getLocalName(), newInstance);
    }
    else if (object->getClass()->isSameOrInheritsFrom(*rpr::ExpendablesInterface::meta().type()))
    {
        auto* newInstance = memnew(ExpendableManager);
        newInstance->setInterface( object, &config_);
        newInstance->set_name(object->getLocalName().c_str());
        senNode_->getGeoreferenceNode()->call_deferred("add_child", newInstance);
        baseEntityManagers_.try_emplace(object->getLocalName(), newInstance);
    }
    else if (object->getClass()->isSameOrInheritsFrom(*rpr::MunitionInterface::meta().type()))
    {
        auto* newInstance = memnew(MunitionManager);
        newInstance->setInterface( object, &config_);
        newInstance->set_name(object->getLocalName().c_str());
        senNode_->getGeoreferenceNode()->call_deferred("add_child", newInstance);
        baseEntityManagers_.try_emplace(object->getLocalName(), newInstance);
    }
}

void SenGodotComponent::onObjectRemoved(sen::Object* object)
{
    if (object->getClass()->isSameOrInheritsFrom(*rpr::BaseEntityInterface::meta().type()))
    {
        baseEntityManagers_.at(object->getLocalName())->queue_free();
        baseEntityManagers_.erase(object->getLocalName());
    }
}

