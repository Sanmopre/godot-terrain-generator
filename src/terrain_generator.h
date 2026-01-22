#pragma once

#include "utils.h"
#include "noise_generator.h"

// Godot
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"


// std
#include <memory>
#include <unordered_map>

namespace godot 
{

enum class TerrainLevelOfDetail : u8
{
	LEVEL_0 = 0,
	LEVEL_1 = 1,
	LEVEL_2 = 2,
	LEVEL_3 = 3,
	LEVEL_4 = 4,
	LEVEL_5 = 5,
	LEVEL_6 = 6
};

struct ChunkData
{
	i32 x;
	i32 z;
	TerrainLevelOfDetail lod = TerrainLevelOfDetail::LEVEL_0;

	bool operator==(const ChunkData &other) const
	{
		return x == other.x && z == other.z && lod == other.lod;
	}
};

struct ChunkCoordinatesHash 
{
    std::size_t operator()(const ChunkData& c) const noexcept {
        std::size_t h1 = std::hash<i32>{}(c.x);
        std::size_t h2 = std::hash<i32>{}(c.z);
        return h1 ^ (h2 << 1);
    }
};


class TerrainGenerator : public Node
{
	GDCLASS(TerrainGenerator, Node)

protected:
	static void _bind_methods();

public:
	TerrainGenerator();
	~TerrainGenerator() = default;

public:
	// Implements Node functions
	void _ready() override;
	void _process(double delta) override;

private:
	[[nodiscard]] MeshInstance3D * generateChunkMesh(ChunkData &&chunkData) const noexcept;

private:
	// Noise generator
	std::unique_ptr<NoiseGenerator> noiseGenerator_;

private:
	// Tile information
	f64 tileWidth_;
	u16 chunkSize_;

private:
	// Chunks
	std::unordered_map<ChunkData, MeshInstance3D*, ChunkCoordinatesHash> chunks_;
};

}