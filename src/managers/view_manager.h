#pragma once

#include "root_manager.h"
#include "base_entity_manager.h"

// generated code
#include "ig_interface/view.stl.h"

// godot
#include "godot_cpp/classes/camera3d.hpp"
#include "godot_cpp/classes/sub_viewport.hpp"
#include "godot_cpp/classes/sub_viewport_container.hpp"

class ViewManager : public RootManager
{
    GDCLASS(ViewManager, RootManager)
protected:
    static void _bind_methods(){};

public:
    using InterfaceType = sen_ig_gateway::ViewInterface;

public: // RootManager implementation
    void setInterface(sen::Object* interface, ComponentConfiguration* config) override;
    void setEntityToAttach(BaseEntityManager* entity);
    void componentUpdate(sen::kernel::RunApi* api) override;

public: // godot::Node implementation
    void _ready() override;
    void _process(double p_delta) override;

private:
    void updateViewportLayout();
    void manageViewTypeChange(const sen_ig_gateway::ViewType& vieType);
    void manageFreeCamera(const sen_ig_gateway::FreeCamera& view);
    void manageAttachedCamera(const sen_ig_gateway::AttachedCamera& view);
    void manageOrbitCamera(const sen_ig_gateway::OrbitCamera& view);

private:
    InterfaceType* interface_ = nullptr;
    const configuration::Viewport* currentViewport = nullptr;
    std::vector<sen::ConnectionGuard> guards_;

    godot::SubViewportContainer* subViewContainer_ = nullptr;
    godot::SubViewport* viewport_ = nullptr;
    godot::Camera3D* camera_ = nullptr;

private:
    // properties callbacks
    bool fovChanged_ = false;
    bool viewTypeChanged_ = false;

private:
    std::optional<std::string> attachedEntity_;
    godot::Vector3 viewOffset_;
    f64 yaw_;
    f64 pitch_;
    f64 roll_;
    bool entityFound_ = false;
};

