#include "register_types.h"
#include "sen_node.h"
#include "managers/aircraft_manager.h"
#include "managers/expendable_manager.h"
#include "managers/munition_manager.h"

// godot
#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

void initializeSenGodotModule(godot::ModuleInitializationLevel p_level) {
    if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    GDREGISTER_CLASS(godot::SenNode);
    GDREGISTER_CLASS(RootManager);
    GDREGISTER_CLASS(BaseEntityManager);
    GDREGISTER_CLASS(PhysicalEntityManager);
    GDREGISTER_CLASS(MunitionManager);
    GDREGISTER_CLASS(ExpendableManager);
    GDREGISTER_CLASS(PlatformManager);
    GDREGISTER_CLASS(AircraftManager);
}

void uninitializeSenGodotModule(godot::ModuleInitializationLevel p_level) {
    if (p_level != godot::MODULE_INITIALIZATION_LEVEL_SCENE)
    {
        return;
    }
}

extern "C"
{
    GDExtensionBool GDE_EXPORT senGodotInit(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
    {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initializeSenGodotModule);
        init_obj.register_terminator(uninitializeSenGodotModule);
        init_obj.set_minimum_library_initialization_level(godot::MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}