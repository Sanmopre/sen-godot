#pragma once

#include "root_manager.h"

// std_fom
#include "rpr/rpr-base_v2.0.xml.h"

// sen
#include "sen/util/dr/dead_reckoner.h"

struct Pivot
{
    godot::Node3D* yaw = nullptr;
    godot::Node3D* pitch = nullptr;
    godot::Node3D* roll = nullptr;
};

class BaseEntityManager : public RootManager
{
    GDCLASS(BaseEntityManager, RootManager)
protected:
    static void _bind_methods();

public:
    using InterfaceType = rpr::BaseEntityInterface;

public: // RootManager implementation
    void setInterface(sen::Object* interface, ComponentConfiguration* config) override;

public:
    void componentUpdate(sen::kernel::RunApi* api) override;

public: // godot::Node implementation
    void _ready() override;
    void _process(double p_delta) override;
    void _exit_tree() override;

public:
    void align_belly_to_origin();
    [[nodiscard]] Pivot* getEntityPivots() noexcept;
    [[nodiscard]] Node* getModelNode() const noexcept;
    void on_item_selected(int32_t index);
private:
    InterfaceType* interface_ = nullptr;
    Node* model_ = nullptr;
    Pivot pivot_;
    std::vector<sen::ConnectionGuard> guards_;
    std::unique_ptr<sen::util::DeadReckoner<InterfaceType>> deadReckoner_;
    std::string entityType;
    godot::Node *cameraNode_ = nullptr;

private:
    // properties
    godot::Vector3 ecefLocation_;
    godot::Vector3 rotation_;

    // UI list handle
    int32_t itemId_ = 0;
};

