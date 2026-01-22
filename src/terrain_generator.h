#pragma once

// Godot
#include "godot_cpp/classes/node.hpp"

namespace godot 
{

class TerrainGenerator : public Node
{
	GDCLASS(TerrainGenerator, Node)

protected:
	static void _bind_methods();

public:
	TerrainGenerator() = default;
	~TerrainGenerator() = default;

public:
	// Implements Node functions
	void _ready() override;
	void _process(double delta) override;
};

}