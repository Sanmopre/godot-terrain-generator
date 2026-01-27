#pragma once

#include "utils.h"
#include "noise_generator.h"

// Godot
#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/material.hpp"

// std
#include <memory>
#include <unordered_map>
#include <deque>

namespace godot 
{

enum class TerrainLevelOfDetail : u8
{
	LEVEL_0 = 0,
	LEVEL_1 = 1,
	LEVEL_2 = 2,
	LEVEL_3 = 3
};

struct ChunkEntry {
    MeshInstance3D *node = nullptr;
    TerrainLevelOfDetail lod = TerrainLevelOfDetail::LEVEL_0;
};

struct ChunkCoord {
    i32 x;
    i32 z;

    bool operator==(const ChunkCoord& o) const noexcept {
        return x == o.x && z == o.z;
    }
};

struct BuildRequest {
    ChunkCoord coord;
    TerrainLevelOfDetail lod;
};


struct ChunkData
{
	i32 x;
	i32 z;
	TerrainLevelOfDetail lod = TerrainLevelOfDetail::LEVEL_0;
};

struct ChunkCoordHash {
    size_t operator()(const ChunkCoord& c) const noexcept {
        size_t h1 = std::hash<i32>{}(c.x);
        size_t h2 = std::hash<i32>{}(c.z);
        size_t h = h1;
        h ^= h2 + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
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

	void set_terrain_material(const Ref<Material> &material);
	Ref<Material> get_terrain_material() const;

	void set_view_radius(i32 radius) noexcept;
	i32 get_view_radius() const noexcept;

	void set_unload_radius(i32 radius) noexcept;
	i32 get_unload_radius() const noexcept;

	void set_chunks_per_frame(i32 count) noexcept;
	i32 get_chunks_per_frame() const noexcept;

	void set_lod_level_0_distance(i32 distance) noexcept;
	i32 get_lod_level_0_distance() const noexcept;

	void set_lod_level_1_distance(i32 distance) noexcept;
	i32 get_lod_level_1_distance() const noexcept;

	void set_lod_level_2_distance(i32 distance) noexcept;
	i32 get_lod_level_2_distance() const noexcept;

	void set_water_level(f64 level) noexcept;
	f64 get_water_level() const noexcept;

	i32 get_noise_seed() const;
	void set_noise_seed(i32 v);

	i32 get_noise_type() const;
	void set_noise_type(i32 v);

	f64 get_noise_frequency() const;
	void set_noise_frequency(f64 v);

	i32 get_fractal_type() const;
	void set_fractal_type(i32 v);

	i32 get_fractal_octaves() const;
	void set_fractal_octaves(i32 v);

	f64 get_fractal_lacunarity() const;
	void set_fractal_lacunarity(f64 v);

	f64 get_fractal_gain() const;
	void set_fractal_gain(f64 v);

	bool get_domain_warp_enabled() const;
	void set_domain_warp_enabled(bool v);

	f64 get_domain_warp_amplitude() const;
	void set_domain_warp_amplitude(f64 v);

private:
	[[nodiscard]] MeshInstance3D * generateChunkMesh(const ChunkData& chunkData) const noexcept;
	[[nodiscard]] ChunkCoord chunkFromWorld(const Vector3& worldPosition) const noexcept;
	void onCenterChunkChanged(const ChunkCoord& center);
	[[nodiscard]] TerrainLevelOfDetail lodForDistance(int dist_chunks) const noexcept;
	void enqueueNeededChunks(const ChunkCoord &center);
	void resolvePlayerNode();

private:
	// Noise generator
	std::unique_ptr<NoiseGenerator> noiseGenerator_;
	NoiseSettings noiseSettings_;

private:
	Ref<Material> terrain_material_;
    NodePath player_path_;
    Node3D *player_ = nullptr; // cached (not owned)

private:
	// Tile information
	f64 tileWidth_;
	f64 tileHeight_;
	u16 chunkSize_;
	f64 waterLevel_;

private:
	i32 viewRadius_ = 8;
	i32 unloadRadius_ = 12;
	i32 chunksPerFrame_ = 5;

	i32 lodLevel0Distance_ = 2;
	i32 lodLevel1Distance_ = 4;
	i32 lodLevel2Distance_ = 7;

private:
	// Chunks
	std::unordered_map<ChunkCoord, ChunkEntry, ChunkCoordHash> chunks_;
	std::deque<BuildRequest> chunkBuildQueue_;
	ChunkCoord currentChunkCenter_;
	bool has_center_ = false;
};

}