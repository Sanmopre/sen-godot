#pragma once

// sen
#include "sen/core/obj/object.h"

// godot
#include <sen/kernel/kernel.h>

#include "godot_cpp/classes/node3d.hpp"

class RootManager : public godot::Node3D
{
    GDCLASS(RootManager, godot::Node3D)
protected:
    static void _bind_methods(){}

public:
    virtual void setInterface(sen::Object* interface, sen::impl::WorkQueue* queue)
    {
        object_ = interface;
    }

    virtual void componentUpdate(sen::kernel::RunApi* api)
    {
        std::ignore = api;
    }

private:
    sen::Object* object_ = nullptr;
};