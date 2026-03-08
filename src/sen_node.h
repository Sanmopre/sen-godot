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
    void _process(double delta) override;

    [[nodiscard]] i64 get_tick_duration_milliseconds() const noexcept;
    void set_tick_duration_milliseconds(i64 tick_duration_milliseconds);

private:
    std::shared_ptr<SenGodotComponent> component_;
    std::unique_ptr<sen::kernel::TestKernel> kernel_;
    i64 tickDurationMilliseconds_ = 100;
};

}