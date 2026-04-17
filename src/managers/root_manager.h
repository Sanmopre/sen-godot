// ================================
//
// sen_godot
// File: root_manager.h
//
// Author: Sanmopre
// Copyright (c) 2026 Santiago Moliner
//
// Licensed under the MIT License.
// See LICENSE file for details.
//
// ================================

#pragma once

// sen
#include "sen/core/obj/object.h"

// godot
#include <sen/kernel/kernel.h>
#include "godot_cpp/classes/node3d.hpp"

// generated code
#include "configuration.stl.h"

struct UI_Components;
class BaseEntityManager;
namespace godot {class SenNode;}

struct ComponentConfiguration
{
    std::shared_ptr<configuration::EngineConfigurationBase> engineConfiguration_;
    sen::impl::WorkQueue* workQueue_;
    godot::SenNode* senNode_;
    UI_Components* uiComponents_;
    std::unordered_map<std::string, BaseEntityManager*>* baseEntityManagers_;
};


class RootManager : public godot::Node3D
{
    GDCLASS(RootManager, godot::Node3D)
protected:
    static void _bind_methods(){}

public:
    virtual void setInterface(sen::Object* interface, ComponentConfiguration* config)
    {
        object_ = interface;
        config_ = config;
    }

    virtual void componentUpdate(sen::kernel::RunApi* api)
    {
        std::ignore = api;
    }

protected:
    [[nodiscard]] ComponentConfiguration* getConfig() const
    {
        return config_;
    }

private:
    sen::Object* object_ = nullptr;
    ComponentConfiguration* config_;
};