#include "terrain_generator.h"

// Godot
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/array_mesh.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/array.hpp"

namespace godot 
{

void TerrainGenerator::_bind_methods() 
{
}

TerrainGenerator::TerrainGenerator()
: noiseGenerator_(std::make_unique<NoiseGenerator>())
, tileWidth_(1.0)
, chunkSize_(32)
{
}

void TerrainGenerator::_ready()
{
    chunks_.try_emplace(ChunkData{0, 0, TerrainLevelOfDetail::LEVEL_0}, generateChunkMesh(ChunkData{0, 0, TerrainLevelOfDetail::LEVEL_0}));
    add_child(chunks_.at(ChunkData{0, 0, TerrainLevelOfDetail::LEVEL_0}), false);
}

void TerrainGenerator::_process(double delta)
{
}

MeshInstance3D * TerrainGenerator::generateChunkMesh(ChunkData &&chunkData) const noexcept
{
    auto meshInstance = memnew(MeshInstance3D);

    Ref<ArrayMesh> mesh;
    mesh.instantiate();

    const f32 w = static_cast<f32>(tileWidth_) * static_cast<f32>(chunkSize_);

    PackedVector3Array vertices;
    vertices.resize(4);
    vertices.set(0, Vector3(0, 0, 0));
    vertices.set(1, Vector3(w, 0, 0));
    vertices.set(2, Vector3(w, 0, w));
    vertices.set(3, Vector3(0, 0, w));

    PackedInt32Array indices;
    indices.resize(6);
    indices.set(0, 0);
    indices.set(1, 1);
    indices.set(2, 2);
    indices.set(3, 0);
    indices.set(4, 2);
    indices.set(5, 3);

    Array arrays;
    arrays.resize(Mesh::ARRAY_MAX);
    arrays[Mesh::ARRAY_VERTEX] = vertices;
    arrays[Mesh::ARRAY_INDEX]  = indices;

    mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
    meshInstance->set_mesh(mesh);

    return meshInstance;
}

}