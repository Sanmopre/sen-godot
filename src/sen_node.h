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
    // Setters - Getters
    void set_georeference_path(const godot::NodePath &path);
    [[nodiscard]] NodePath get_georeference_path() const;
    void set_tileset_paths(const godot::Array &tilesets);
    [[nodiscard]] godot::Array get_tileset_paths() const;

public:
    [[nodiscard]] godot::Vector3 getGeoreferenceEcefValue();
    [[nodiscard]] Node* getGeoreferenceNode() const noexcept;
    [[nodiscard]] godot::Array* getTilesets() noexcept;

private:
    std::shared_ptr<SenGodotComponent> component_;
    std::unique_ptr<sen::kernel::TestKernel> kernel_;

private:
    NodePath georeferencePath_;
    Array tilesetsPaths_;
    Node* georeferenceNode_;
    Array tilesetsNodes_;
};

}