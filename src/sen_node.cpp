#include "sen_node.h"

// sen
#include "sen/kernel/test_kernel.h"
#include "sen/kernel/bootloader.h"

// godot
#include "godot_cpp/variant/utility_functions.hpp"

namespace godot
{

void SenNode::_process(double delta)
{
    kernel_->step();
}

i64 SenNode::get_tick_duration_milliseconds() const noexcept
{
    return tickDurationMilliseconds_;
}

void SenNode::set_tick_duration_milliseconds(i64 tick_duration_milliseconds)
{
    tickDurationMilliseconds_ = tick_duration_milliseconds;
    component_->updateTickDuration(std::chrono::milliseconds(tickDurationMilliseconds_));
}

void SenNode::_ready()
{
    component_ = std::make_shared<SenGodotComponent>(std::chrono::milliseconds(tickDurationMilliseconds_));
    const auto bootLoader = sen::kernel::Bootloader::fromYamlString("", false);

    sen::kernel::ComponentContext component;
    component.instance = component_.get();
    component.info.name = "sen_godot";
    component.info.description = "Sen-Godot extension";
    component.info.buildInfo = {}; // TODO: Fill accordingly
    component.config.cpuAffinity = 0xFF;
    component.config.group = 2;
    component.config.priority = sen::kernel::Priority::nominalMin;
    component.config.stackSize = 0;
    //component.config.inQueue
    //component.config.outQueue
    //component.config.sleepPolicy

    sen::kernel::ComponentConfig config;
    config.cpuAffinity = 0xFF;
    config.group = 2;
    config.priority = sen::kernel::Priority::nominalMin;
    config.stackSize = 0;
    //config.inQueue
    //config.outQueue
    //config.sleepPolicy

    const sen::kernel::KernelConfig::ComponentToLoad componentConfig
    {
        std::move(component),
        config,
        {} // params
    };

    bootLoader->getConfig().addToLoad(componentConfig);

    kernel_ = std::make_unique<sen::kernel::TestKernel>(bootLoader->getConfig());
}

void SenNode::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_tick_duration_milliseconds", "tick_duration_milliseconds"), &SenNode::set_tick_duration_milliseconds);
    ClassDB::bind_method(D_METHOD("get_tick_duration_milliseconds"), &SenNode::get_tick_duration_milliseconds);

    ADD_PROPERTY(
        PropertyInfo(
            Variant::INT,
            "tick_duration_milliseconds",
            PROPERTY_HINT_RANGE,
            "0,1000000,1,suffix:ms"
        ),
        "set_tick_duration_milliseconds",
        "get_tick_duration_milliseconds"
    );
}

}