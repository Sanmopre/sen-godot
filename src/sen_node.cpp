#include "sen_node.h"

// sen
#include "sen/kernel/test_kernel.h"
#include "sen/kernel/bootloader.h"

// godot
#include "godot_cpp/variant/utility_functions.hpp"
#include <godot_cpp/classes/engine.hpp>

namespace godot
{

void SenNode::_physics_process(double p_delta)
{
    kernel_->step();
}

void SenNode::_ready()
{
    const auto millisecondsPerTick = static_cast<i64>(1000.0 / Engine::get_singleton()->get_physics_ticks_per_second());
    component_ = std::make_shared<SenGodotComponent>(this, std::chrono::milliseconds(millisecondsPerTick));
    const auto bootLoader = sen::kernel::Bootloader::fromYamlString("", false);

    sen::kernel::ComponentContext component;
    component.instance = component_.get();
    component.info.name = "sen_godot";
    component.info.description = "Sen-Godot extension";
    component.info.buildInfo = {};
    component.config.cpuAffinity = 0xFF;
    component.config.group = 2;
    component.config.priority = sen::kernel::Priority::nominalMin;
    component.config.stackSize = 0;
    component.config.inQueue.evictionPolicy = sen::kernel::QueueEvictionPolicy::dropOldest;
    component.config.inQueue.maxSize = 0;
    component.config.outQueue.evictionPolicy = sen::kernel::QueueEvictionPolicy::dropOldest;
    component.config.outQueue.maxSize = 0;
    component.config.sleepPolicy = sen::kernel::SystemSleep{};

    sen::kernel::ComponentConfig config;
    config.cpuAffinity = 0xFF;
    config.group = 2;
    config.priority = sen::kernel::Priority::nominalMin;
    config.stackSize = 0;
    config.inQueue.evictionPolicy = sen::kernel::QueueEvictionPolicy::dropOldest;
    config.inQueue.maxSize = 0;
    config.outQueue.evictionPolicy = sen::kernel::QueueEvictionPolicy::dropOldest;
    config.outQueue.maxSize = 0;
    config.sleepPolicy = sen::kernel::SystemSleep{};

    const sen::kernel::KernelConfig::ComponentToLoad componentConfig
    {
        std::move(component),
        config,
        {} // empty params
    };

    bootLoader->getConfig().addToLoad(componentConfig);
    kernel_ = std::make_unique<sen::kernel::TestKernel>(bootLoader->getConfig());
}

void SenNode::_bind_methods()
{
}

}
