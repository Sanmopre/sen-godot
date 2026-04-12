#pragma once

#include "sen_godot_component.h"

// godot
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/item_list.hpp>

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
    void _process(double p_delta) override;

public:
    // Setters - Getters
    void set_georeference_path(const godot::NodePath &path);
    [[nodiscard]] NodePath get_georeference_path() const;
    void set_item_list_path(const NodePath &path) {
        item_list_path_ = path;
    }

    [[nodiscard]] NodePath get_item_list_path() const {
        return item_list_path_;
    }
public:
    [[nodiscard]] Node* getGeoreferenceNode() const noexcept;

private:
    std::shared_ptr<SenGodotComponent> component_;
    std::unique_ptr<sen::kernel::TestKernel> kernel_;

private: // References
    NodePath georeferencePath_;
    Node* georeferenceNode_;

private:
    std::unique_ptr<UI_Components> uiComponents_;
    NodePath item_list_path_;


};

}