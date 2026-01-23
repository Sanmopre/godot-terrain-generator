#include "terrain_generator.h"

// Godot
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/array_mesh.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/array.hpp"

namespace godot 
{

namespace
{

constexpr f64 defaultTileWith = 1.0;
constexpr u16 defaultChunkSize = 32;
constexpr f64 defaultTileHeight = 10.0;

} 

void TerrainGenerator::_bind_methods() {
    // Bind methods using the SAME names you'll reference in ADD_PROPERTY
    ClassDB::bind_method(D_METHOD("get_tile_width"), &TerrainGenerator::get_tile_width);
    ClassDB::bind_method(D_METHOD("set_tile_width", "width"), &TerrainGenerator::set_tile_width);

    ClassDB::bind_method(D_METHOD("get_chunk_size"), &TerrainGenerator::get_chunk_size);
    ClassDB::bind_method(D_METHOD("set_chunk_size", "size"), &TerrainGenerator::set_chunk_size);

    ClassDB::bind_method(D_METHOD("get_tile_height"), &TerrainGenerator::get_tile_height);
    ClassDB::bind_method(D_METHOD("set_tile_height", "height"), &TerrainGenerator::set_tile_height);


    ADD_GROUP("Terrain", "");

    ADD_PROPERTY(
        PropertyInfo(Variant::FLOAT, "tile_width", PROPERTY_HINT_RANGE, "0.0,1000.0,0.01,or_greater"),
        "set_tile_width",
        "get_tile_width"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "chunk_size", PROPERTY_HINT_RANGE, "0,512,1"),
        "set_chunk_size",
        "get_chunk_size"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::FLOAT, "tile_height", PROPERTY_HINT_RANGE, "0.0,1000.0,0.01,or_greater"),
        "set_tile_height",
        "get_tile_height"
    );
}

TerrainGenerator::TerrainGenerator()
: noiseGenerator_(std::make_unique<NoiseGenerator>())
, tileWidth_(defaultTileWith)
, chunkSize_(defaultChunkSize)
, tileHeight_(defaultTileHeight)
{
}

f64 TerrainGenerator::get_tile_width() const noexcept {
    return tileWidth_;
}

void TerrainGenerator::set_tile_width(f64 width) noexcept {
    if (width < 0.0) width = 0.0;
    tileWidth_ = width;
}

i32 TerrainGenerator::get_chunk_size() const noexcept {
    return static_cast<int32_t>(chunkSize_);
}

void TerrainGenerator::set_chunk_size(i32 size) noexcept {
    if (size < 0) size = 0;
    if (size > 65535) size = 65535;
    chunkSize_ = static_cast<u16>(size);
}

f64 TerrainGenerator::get_tile_height() const noexcept {
    return tileHeight_;
}

void TerrainGenerator::set_tile_height(f64 height) noexcept {
    tileHeight_ = height;
}

void TerrainGenerator::set_player_node(const NodePath &path) 
{
    player_path_ = path;
    player_ = nullptr;

    if (player_path_.is_empty()) {
        return;
    }

    Node *n = get_node_or_null(player_path_);
    if (!n) {
        return;
    }

    player_ = Object::cast_to<Node3D>(n);
}

NodePath TerrainGenerator::get_player_node() const {
    return player_path_;
}


void TerrainGenerator::_ready() {
    ChunkData key{0, 0, TerrainLevelOfDetail::LEVEL_0};
    MeshInstance3D *mi = generateChunkMesh(key);
    chunks_.try_emplace(key, mi);
    add_child(mi, false);

    key = ChunkData{1, 0, TerrainLevelOfDetail::LEVEL_2};
    MeshInstance3D *mi2 = generateChunkMesh(key);
    chunks_.try_emplace(key, mi2);
    add_child(mi2, false);

    key = ChunkData{0, 1, TerrainLevelOfDetail::LEVEL_3};
    MeshInstance3D *mi3 = generateChunkMesh(key);
    chunks_.try_emplace(key, mi3);
    add_child(mi3, false);
}

void TerrainGenerator::_process(double delta)
{
}

MeshInstance3D *TerrainGenerator::generateChunkMesh(const ChunkData& chunkData) const noexcept {
    MeshInstance3D *meshInstance = memnew(MeshInstance3D);

    Ref<ArrayMesh> mesh;
    mesh.instantiate();

    const int lod_i = static_cast<int>(chunkData.lod);
    const int step = 1 << lod_i; // 1,2,4,8,...
    const int squares_per_side = static_cast<int>(chunkSize_) / step;
    const int verts_per_side = squares_per_side + 1;

    const float tile = static_cast<float>(tileWidth_);
    const float quad_size = tile * static_cast<float>(step);

    const double chunk_world_x0 = static_cast<double>(chunkData.x) * static_cast<double>(chunkSize_) * tileWidth_;
    const double chunk_world_z0 = static_cast<double>(chunkData.z) * static_cast<double>(chunkSize_) * tileWidth_;

    const int vertex_count = verts_per_side * verts_per_side;
    const int index_count = squares_per_side * squares_per_side * 6;

    PackedVector3Array vertices;
    vertices.resize(vertex_count);

    PackedVector3Array normals;
    normals.resize(vertex_count);
    for (int i = 0; i < vertex_count; i++) {
        normals.set(i, Vector3(0, 0, 0));
    }

    PackedVector2Array uvs;
    uvs.resize(vertex_count);

    PackedInt32Array indices;
    indices.resize(index_count);

    auto vid = [verts_per_side](int vx, int vz) -> int {
        return vz * verts_per_side + vx;
    };

    for (int vz = 0; vz < verts_per_side; vz++) {
        for (int vx = 0; vx < verts_per_side; vx++) {
            const int i = vid(vx, vz);

            const double world_x = chunk_world_x0 + static_cast<double>(vx) * static_cast<double>(quad_size);
            const double world_z = chunk_world_z0 + static_cast<double>(vz) * static_cast<double>(quad_size);

            const double h = noiseGenerator_->getNoiseValue(world_x, world_z) * tileHeight_;

            const float px = static_cast<float>(vx) * quad_size;
            const float py = static_cast<float>(h);
            const float pz = static_cast<float>(vz) * quad_size;

            vertices.set(i, Vector3(px, py, pz));

            const float u = (verts_per_side > 1) ? (float)vx / (float)(verts_per_side - 1) : 0.0f;
            const float v = (verts_per_side > 1) ? (float)vz / (float)(verts_per_side - 1) : 0.0f;
            uvs.set(i, Vector2(u, v));
        }
    }

    int idx = 0;
    for (int z = 0; z < squares_per_side; z++) {
        for (int x = 0; x < squares_per_side; x++) {
            const int v00 = vid(x, z);
            const int v10 = vid(x + 1, z);
            const int v01 = vid(x, z + 1);
            const int v11 = vid(x + 1, z + 1);

            indices.set(idx++, v00);
            indices.set(idx++, v11);
            indices.set(idx++, v01);
            indices.set(idx++, v00);
            indices.set(idx++, v10);
            indices.set(idx++, v11);
        }
    }

    for (int t = 0; t < index_count; t += 3) {
        const int ia = indices[t + 0];
        const int ib = indices[t + 1];
        const int ic = indices[t + 2];

        const Vector3 a = vertices[ia];
        const Vector3 b = vertices[ib];
        const Vector3 c = vertices[ic];

        const Vector3 n = (b - a).cross(c - a);

        normals.set(ia, normals[ia] + n);
        normals.set(ib, normals[ib] + n);
        normals.set(ic, normals[ic] + n);
    }

    for (int i = 0; i < vertex_count; i++) {
        Vector3 n = normals[i];
        if (n.length_squared() > 0.000001f) {
            n = n.normalized();
        } else {
            n = Vector3(0, 1, 0);
        }
        normals.set(i, n);
    }

    Array arrays;
    arrays.resize(Mesh::ARRAY_MAX);
    arrays[Mesh::ARRAY_VERTEX] = vertices;
    arrays[Mesh::ARRAY_NORMAL] = normals;
    arrays[Mesh::ARRAY_TEX_UV] = uvs;
    arrays[Mesh::ARRAY_INDEX]  = indices;

    mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
    meshInstance->set_mesh(mesh);

    meshInstance->set_position(Vector3(
        static_cast<float>(chunk_world_x0),
        0.0f,
        static_cast<float>(chunk_world_z0)
    ));

    return meshInstance;
}

}