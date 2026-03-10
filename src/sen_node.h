#pragma once

#include "sen_godot_component.h"

// godot
#include <godot_cpp/classes/node.hpp>

// sen
#include "sen/kernel/test_kernel.h"

namespace godot
{

class SenNode : public Node
{
    GDCLASS(SenNode, Node)

protected:
    static void _bind_methods();

public:
    SenNode() = default;
    ~SenNode() override = default ;

    void _ready() override;
    void _physics_process(double p_delta) override;

public:
    template <typename T>
    void addNode(T *node)
    {
        static_assert(std::is_base_of_v<Node, T>, "T must derive from godot::Node");
        this->add_child(node);
    }

private:
    std::shared_ptr<SenGodotComponent> component_;
    std::unique_ptr<sen::kernel::TestKernel> kernel_;
};

}