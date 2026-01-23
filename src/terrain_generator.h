#pragma once

#include "utils.h"
#include "noise_generator.h"

// Godot
#include "godot_cpp/classes/node3d.hpp"
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

struct ChunkCoordinatesHash {
    std::size_t operator()(const ChunkData& c) const noexcept {
        std::size_t h1 = std::hash<i32>{}(c.x);
        std::size_t h2 = std::hash<i32>{}(c.z);
        std::size_t h3 = std::hash<u8>{}(static_cast<u8>(c.lod));

        std::size_t h = h1;
        h ^= h2 + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        h ^= h3 + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        return h;
    }
};


class TerrainGenerator : public Node3D
{
	GDCLASS(TerrainGenerator, Node3D)

protected:
	static void _bind_methods();

public:
	TerrainGenerator();
	~TerrainGenerator() = default;

public:
	// Implements Node functions
	void _ready() override;
	void _process(double delta) override;

public:
	// Getters and setters
	f64 get_tile_width() const noexcept;
	void set_tile_width(f64 width) noexcept;

	i32 get_chunk_size() const noexcept;
	void set_chunk_size(i32 size) noexcept;

	f64 get_tile_height() const noexcept;
	void set_tile_height(f64 height) noexcept;

    void set_player_node(const NodePath &path);
    NodePath get_player_node() const;

private:
	[[nodiscard]] MeshInstance3D * generateChunkMesh(const ChunkData& chunkData) const noexcept;

private:
	// Noise generator
	std::unique_ptr<NoiseGenerator> noiseGenerator_;

private:
    NodePath player_path_;
    Node3D *player_ = nullptr; // cached (not owned)

private:
	// Tile information
	f64 tileWidth_;
	f64 tileHeight_;
	u16 chunkSize_;

private:
	// Chunks
	std::unordered_map<ChunkData, MeshInstance3D*, ChunkCoordinatesHash> chunks_;
};

}