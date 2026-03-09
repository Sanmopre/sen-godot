#include "sen_godot_component.h"

// godot
#include "godot_cpp/variant/utility_functions.hpp"

SenGodotComponent::SenGodotComponent(const sen::Duration& tickDuration)
    : tickDuration_(tickDuration)
{
}

sen::kernel::FuncResult SenGodotComponent::load(sen::kernel::LoadApi&& load_api)
{
    return Component::load(std::move(load_api));
}

sen::kernel::PassResult SenGodotComponent::init(sen::kernel::InitApi&& api)
{
    sources_["test.bus"] = api.getSource("test.bus");

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

