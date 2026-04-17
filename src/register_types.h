// ================================
//
// sen_godot
// File: register_types.h
//
// Author: Sanmopre
// Copyright (c) 2026 Santiago Moliner
//
// Licensed under the MIT License.
// See LICENSE file for details.
//
// ================================

#pragma once

// godot
#include "godot_cpp/core/class_db.hpp"

void initializeSenGodotModule(godot::ModuleInitializationLevel p_level);
void uninitializeSenGodotModule(godot::ModuleInitializationLevel p_level);