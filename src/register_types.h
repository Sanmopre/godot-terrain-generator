#pragma once

// Godot
#include "godot_cpp/core/class_db.hpp"

namespace godot
{

void initialize_terrain_generator_module(ModuleInitializationLevel p_level);
void uninitialize_terrain_generator_module(ModuleInitializationLevel p_level);

}
