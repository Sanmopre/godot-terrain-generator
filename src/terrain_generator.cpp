#include "terrain_generator.h"

// Godot
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/array_mesh.hpp"
#include "godot_cpp/variant/packed_vector3_array.hpp"
#include "godot_cpp/variant/packed_int32_array.hpp"
#include "godot_cpp/variant/array.hpp"

// std
#include <algorithm>
#include <cmath>

namespace godot 
{

namespace
{

constexpr f64 defaultTileWith = 1.0;
constexpr u16 defaultChunkSize = 32;
constexpr f64 defaultTileHeight = 10.0;

[[nodiscard]] int chebyshevDist(int dx, int dz) {
    return std::max(std::abs(dx), std::abs(dz));
}

} 

void TerrainGenerator::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_noise_seed"), &TerrainGenerator::get_noise_seed);
    ClassDB::bind_method(D_METHOD("set_noise_seed", "v"), &TerrainGenerator::set_noise_seed);

    ClassDB::bind_method(D_METHOD("get_noise_type"), &TerrainGenerator::get_noise_type);
    ClassDB::bind_method(D_METHOD("set_noise_type", "v"), &TerrainGenerator::set_noise_type);

    ClassDB::bind_method(D_METHOD("get_noise_frequency"), &TerrainGenerator::get_noise_frequency);
    ClassDB::bind_method(D_METHOD("set_noise_frequency", "v"), &TerrainGenerator::set_noise_frequency);

    ClassDB::bind_method(D_METHOD("get_fractal_type"), &TerrainGenerator::get_fractal_type);
    ClassDB::bind_method(D_METHOD("set_fractal_type", "v"), &TerrainGenerator::set_fractal_type);

    ClassDB::bind_method(D_METHOD("get_fractal_octaves"), &TerrainGenerator::get_fractal_octaves);
    ClassDB::bind_method(D_METHOD("set_fractal_octaves", "v"), &TerrainGenerator::set_fractal_octaves);

    ClassDB::bind_method(D_METHOD("get_fractal_lacunarity"), &TerrainGenerator::get_fractal_lacunarity);
    ClassDB::bind_method(D_METHOD("set_fractal_lacunarity", "v"), &TerrainGenerator::set_fractal_lacunarity);

    ClassDB::bind_method(D_METHOD("get_fractal_gain"), &TerrainGenerator::get_fractal_gain);
    ClassDB::bind_method(D_METHOD("set_fractal_gain", "v"), &TerrainGenerator::set_fractal_gain);

    ClassDB::bind_method(D_METHOD("get_domain_warp_enabled"), &TerrainGenerator::get_domain_warp_enabled);
    ClassDB::bind_method(D_METHOD("set_domain_warp_enabled", "v"), &TerrainGenerator::set_domain_warp_enabled);

    ClassDB::bind_method(D_METHOD("get_domain_warp_amplitude"), &TerrainGenerator::get_domain_warp_amplitude);
    ClassDB::bind_method(D_METHOD("set_domain_warp_amplitude", "v"), &TerrainGenerator::set_domain_warp_amplitude);

    ClassDB::bind_method(D_METHOD("get_tile_width"), &TerrainGenerator::get_tile_width);
    ClassDB::bind_method(D_METHOD("set_tile_width", "width"), &TerrainGenerator::set_tile_width);

    ClassDB::bind_method(D_METHOD("get_chunk_size"), &TerrainGenerator::get_chunk_size);
    ClassDB::bind_method(D_METHOD("set_chunk_size", "size"), &TerrainGenerator::set_chunk_size);

    ClassDB::bind_method(D_METHOD("get_tile_height"), &TerrainGenerator::get_tile_height);
    ClassDB::bind_method(D_METHOD("set_tile_height", "height"), &TerrainGenerator::set_tile_height);

    ClassDB::bind_method(D_METHOD("set_player_node", "path"), &TerrainGenerator::set_player_node);
    ClassDB::bind_method(D_METHOD("get_player_node"), &TerrainGenerator::get_player_node);

    ClassDB::bind_method(D_METHOD("set_terrain_material", "material"), &TerrainGenerator::set_terrain_material);
    ClassDB::bind_method(D_METHOD("get_terrain_material"), &TerrainGenerator::get_terrain_material);

    ClassDB::bind_method(D_METHOD("set_view_radius", "radius"), &TerrainGenerator::set_view_radius);
    ClassDB::bind_method(D_METHOD("get_view_radius"), &TerrainGenerator::get_view_radius);

    ClassDB::bind_method(D_METHOD("set_unload_radius", "radius"), &TerrainGenerator::set_unload_radius);
    ClassDB::bind_method(D_METHOD("get_unload_radius"), &TerrainGenerator::get_unload_radius);

    ClassDB::bind_method(D_METHOD("set_chunks_per_frame", "count"), &TerrainGenerator::set_chunks_per_frame);
    ClassDB::bind_method(D_METHOD("get_chunks_per_frame"), &TerrainGenerator::get_chunks_per_frame);

    ClassDB::bind_method(D_METHOD("set_lod_level_0_distance", "distance"), &TerrainGenerator::set_lod_level_0_distance);
    ClassDB::bind_method(D_METHOD("get_lod_level_0_distance"), &TerrainGenerator::get_lod_level_0_distance);

    ClassDB::bind_method(D_METHOD("set_lod_level_1_distance", "distance"), &TerrainGenerator::set_lod_level_1_distance);
    ClassDB::bind_method(D_METHOD("get_lod_level_1_distance"), &TerrainGenerator::get_lod_level_1_distance);

    ClassDB::bind_method(D_METHOD("set_lod_level_2_distance", "distance"), &TerrainGenerator::set_lod_level_2_distance);
    ClassDB::bind_method(D_METHOD("get_lod_level_2_distance"), &TerrainGenerator::get_lod_level_2_distance);

    ClassDB::bind_method(D_METHOD("set_water_level", "level"), &TerrainGenerator::set_water_level);
    ClassDB::bind_method(D_METHOD("get_water_level"), &TerrainGenerator::get_water_level);


    ADD_GROUP("Noise", "");

    ADD_PROPERTY(PropertyInfo(Variant::INT, "noise_seed"), "set_noise_seed", "get_noise_seed");

    ADD_PROPERTY(PropertyInfo(
        Variant::INT, "noise_type", PROPERTY_HINT_ENUM,
        "OpenSimplex2,Perlin,Cellular"
    ), "set_noise_type", "get_noise_type");

    ADD_PROPERTY(PropertyInfo(
        Variant::FLOAT, "noise_frequency", PROPERTY_HINT_RANGE,
        "0.0001,1.0,0.0001"
    ), "set_noise_frequency", "get_noise_frequency");

    ADD_PROPERTY(PropertyInfo(
        Variant::INT, "fractal_type", PROPERTY_HINT_ENUM,
        "FBm,Ridged,PingPong"
    ), "set_fractal_type", "get_fractal_type");

    ADD_PROPERTY(PropertyInfo(
        Variant::INT, "fractal_octaves", PROPERTY_HINT_RANGE,
        "1,10,1"
    ), "set_fractal_octaves", "get_fractal_octaves");

    ADD_PROPERTY(PropertyInfo(
        Variant::FLOAT, "fractal_lacunarity", PROPERTY_HINT_RANGE,
        "1.0,4.0,0.01"
    ), "set_fractal_lacunarity", "get_fractal_lacunarity");

    ADD_PROPERTY(PropertyInfo(
        Variant::FLOAT, "fractal_gain", PROPERTY_HINT_RANGE,
        "0.0,1.0,0.01"
    ), "set_fractal_gain", "get_fractal_gain");

    ADD_PROPERTY(PropertyInfo(
        Variant::BOOL, "domain_warp_enabled"
    ), "set_domain_warp_enabled", "get_domain_warp_enabled");

    ADD_PROPERTY(PropertyInfo(
        Variant::FLOAT, "domain_warp_amplitude", PROPERTY_HINT_RANGE,
        "0.0,200.0,0.1"
    ), "set_domain_warp_amplitude", "get_domain_warp_amplitude");

    ADD_GROUP("Generation", "");

    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "view_radius", PROPERTY_HINT_RANGE, "0,100,1"),
        "set_view_radius",
        "get_view_radius"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "unload_radius", PROPERTY_HINT_RANGE, "0,100,1"),
        "set_unload_radius",
        "get_unload_radius"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "chunks_per_frame", PROPERTY_HINT_RANGE, "0,100,1"),
        "set_chunks_per_frame",
        "get_chunks_per_frame"
    );

    ADD_SUBGROUP("LOD Distances", "");

    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "lod_level_0_distance", PROPERTY_HINT_RANGE, "0,100,1"),
        "set_lod_level_0_distance",
        "get_lod_level_0_distance"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "lod_level_1_distance", PROPERTY_HINT_RANGE, "0,100,1"),
        "set_lod_level_1_distance",
        "get_lod_level_1_distance"
    );

    ADD_PROPERTY(
        PropertyInfo(Variant::INT, "lod_level_2_distance", PROPERTY_HINT_RANGE, "0,100,1"),
        "set_lod_level_2_distance",
        "get_lod_level_2_distance"
    );

    ADD_GROUP("Terrain", "");

    ADD_PROPERTY(
    PropertyInfo(Variant::NODE_PATH, "player_node", PROPERTY_HINT_NODE_PATH_TO_EDITED_NODE, "Node3D"),
    "set_player_node",
    "get_player_node"
    );

    ADD_PROPERTY(
    PropertyInfo(
        Variant::OBJECT,
        "terrain_material",
        PROPERTY_HINT_RESOURCE_TYPE,
        "Material"
    ),
    "set_terrain_material",
    "get_terrain_material"
    );

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

    ADD_PROPERTY(
        PropertyInfo(Variant::FLOAT, "water_level", PROPERTY_HINT_RANGE, "0.0,1.0,0.01"),
        "set_water_level",
        "get_water_level"
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
    resolvePlayerNode();
}

NodePath TerrainGenerator::get_player_node() const {
    return player_path_;
}

void TerrainGenerator::set_terrain_material(const Ref<Material> &material) {
    terrain_material_ = material;
}

Ref<Material> TerrainGenerator::get_terrain_material() const {
    return terrain_material_;
}

void TerrainGenerator::set_view_radius(i32 radius) noexcept {
    if (radius < 0) radius = 0;
    viewRadius_ = radius;
}

i32 TerrainGenerator::get_view_radius() const noexcept {
    return viewRadius_;
}

void TerrainGenerator::set_unload_radius(i32 radius) noexcept {
    if (radius < 0) radius = 0;
    unloadRadius_ = radius;
}

i32 TerrainGenerator::get_unload_radius() const noexcept {
    return unloadRadius_;
}

void TerrainGenerator::set_chunks_per_frame(i32 count) noexcept {
    if (count < 0) count = 0;
    chunksPerFrame_ = count;
}

i32 TerrainGenerator::get_chunks_per_frame() const noexcept {
    return chunksPerFrame_;
}

void TerrainGenerator::set_lod_level_0_distance(i32 distance) noexcept {
    if (distance < 0) distance = 0;
    lodLevel0Distance_ = distance;
}

i32 TerrainGenerator::get_lod_level_0_distance() const noexcept {
    return lodLevel0Distance_;
}

void TerrainGenerator::set_lod_level_1_distance(i32 distance) noexcept {
    if (distance < 0) distance = 0;
    lodLevel1Distance_ = distance;
}

i32 TerrainGenerator::get_lod_level_1_distance() const noexcept {
    return lodLevel1Distance_;
}

void TerrainGenerator::set_lod_level_2_distance(i32 distance) noexcept {
    if (distance < 0) distance = 0;
    lodLevel2Distance_ = distance;
}

i32 TerrainGenerator::get_lod_level_2_distance() const noexcept {
    return lodLevel2Distance_;
}

void TerrainGenerator::set_water_level(f64 level) noexcept {
    waterLevel_ = level;
}

f64 TerrainGenerator::get_water_level() const noexcept {
    return waterLevel_;
}

i32 TerrainGenerator::get_noise_seed() const {
    return noiseSettings_.seed;
}

void TerrainGenerator::set_noise_seed(i32 v) {
    noiseSettings_.seed = v;
}

i32 TerrainGenerator::get_noise_type() const {
       switch (noiseSettings_.noise_type) {
        case FastNoiseLite::NoiseType_Perlin:
            return NOISE_PERLIN;
        case FastNoiseLite::NoiseType_Cellular:
            return NOISE_CELLULAR;
        default:
            return NOISE_OPENSIMPLEX2;
    }
}

void TerrainGenerator::set_noise_type(i32 v) {
    switch (v) {
        case NOISE_PERLIN:
            noiseSettings_.noise_type = FastNoiseLite::NoiseType_Perlin;
            break;
        case NOISE_CELLULAR:
            noiseSettings_.noise_type = FastNoiseLite::NoiseType_Cellular;
            break;
        default:
            noiseSettings_.noise_type = FastNoiseLite::NoiseType_OpenSimplex2;
            break;
    }
}

void TerrainGenerator::set_noise_frequency(f64 v) {
    noiseSettings_.frequency = v;
}

f64 TerrainGenerator::get_noise_frequency() const {
    return noiseSettings_.frequency;
}

i32 TerrainGenerator::get_fractal_type() const {
    return static_cast<i32>(noiseSettings_.fractal_type);
}

void TerrainGenerator::set_fractal_type(i32 v) {
    noiseSettings_.fractal_type = static_cast<FastNoiseLite::FractalType>(v);
}

i32 TerrainGenerator::get_fractal_octaves() const {
    return noiseSettings_.octaves;
}

void TerrainGenerator::set_fractal_octaves(i32 v) {
    noiseSettings_.octaves = v;
}

f64 TerrainGenerator::get_fractal_lacunarity() const {
    return noiseSettings_.lacunarity;
}

void TerrainGenerator::set_fractal_lacunarity(f64 v) {
    noiseSettings_.lacunarity = v;
}

f64 TerrainGenerator::get_fractal_gain() const {
    return noiseSettings_.gain;
}

void TerrainGenerator::set_fractal_gain(f64 v) {
    noiseSettings_.gain = v;
}

bool TerrainGenerator::get_domain_warp_enabled() const {
    return noiseSettings_.domain_warp_enabled;
}

void TerrainGenerator::set_domain_warp_enabled(bool v) {
    noiseSettings_.domain_warp_enabled = v;
}

f64 TerrainGenerator::get_domain_warp_amplitude() const {
    return noiseSettings_.domain_warp_amp;
}

void TerrainGenerator::set_domain_warp_amplitude(f64 v) {
    noiseSettings_.domain_warp_amp = v;
}




void TerrainGenerator::_ready() 
{
    noiseGenerator_->applySettings(noiseSettings_);

    resolvePlayerNode();
    if (!player_) 
    {
        UtilityFunctions::push_warning("TerrainGenerator: player_node is not set or not a Node3D.");
        return;
    }
    
    // Set initial center and enqueue chunks
    currentChunkCenter_ = chunkFromWorld(player_->get_global_position());
    has_center_ = true;
    onCenterChunkChanged(currentChunkCenter_);
}

void TerrainGenerator::_process(double delta)
{
    if (!player_) return;

    const ChunkCoord center = chunkFromWorld(player_->get_global_position());

    if (!has_center_ || !(center == currentChunkCenter_)) 
    {
        has_center_ = true;
        currentChunkCenter_ = center;
        onCenterChunkChanged(currentChunkCenter_);
    }

    int budget = chunksPerFrame_;
    while (budget-- > 0 && !chunkBuildQueue_.empty()) {
        const BuildRequest req = chunkBuildQueue_.front();
        chunkBuildQueue_.pop_front();

        auto it = chunks_.find(req.coord);
        if (it != chunks_.end() && it->second.lod == req.lod)
            continue;

        ChunkData cd{ req.coord.x, req.coord.z, req.lod };
        MeshInstance3D* mi = generateChunkMesh(cd);
        add_child(mi, false);

        if (it == chunks_.end()) {
            chunks_.emplace(req.coord, ChunkEntry{mi, req.lod});
        } else {
            it->second.node->queue_free();
            it->second.node = mi;
            it->second.lod = req.lod;
        }
    }

}

MeshInstance3D *TerrainGenerator::generateChunkMesh(const ChunkData& chunkData) const noexcept {
    MeshInstance3D *meshInstance = memnew(MeshInstance3D);

    Ref<ArrayMesh> mesh;
    mesh.instantiate();

    int lod_i = static_cast<int>(chunkData.lod);
    lod_i = std::clamp(lod_i, 0, 6);

    // Ensure step <= chunkSize_
    while ((1 << lod_i) > static_cast<int>(chunkSize_) && lod_i > 0) {
        lod_i--;
    }

    const int step = 1 << lod_i;
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

            auto noiseValue = noiseGenerator_->getNoiseValue(world_x, world_z);

            // Set to water level if below
            if(noiseValue <= waterLevel_) {
                noiseValue = waterLevel_;
            }

            const double h = noiseValue * tileHeight_;
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

    if (terrain_material_.is_valid()) {
        meshInstance->set_material_override(terrain_material_);
    }

    meshInstance->set_position(Vector3(
        static_cast<float>(chunk_world_x0),
        0.0f,
        static_cast<float>(chunk_world_z0)
    ));

    return meshInstance;
}

ChunkCoord TerrainGenerator::chunkFromWorld(const Vector3 &worldPosition) const noexcept
{
    const f64 s = (f64)chunkSize_ * tileWidth_;
    return ChunkCoord{
        (i32)std::floor(worldPosition.x / s),
        (i32)std::floor(worldPosition.z / s)
    };
}

void TerrainGenerator::onCenterChunkChanged(const ChunkCoord &center) {
    // 1) enqueue chunks in view radius with appropriate LOD
    for (int dz = -viewRadius_; dz <= viewRadius_; dz++) {
        for (int dx = -viewRadius_; dx <= viewRadius_; dx++) {
            ChunkCoord c{ center.x + dx, center.z + dz };

            const int dist = chebyshevDist(dx, dz);
            TerrainLevelOfDetail desired = lodForDistance(dist);

            auto it = chunks_.find(c);

            // Not loaded -> schedule build
            if (it == chunks_.end()) {
                chunkBuildQueue_.push_back(BuildRequest{c, desired});
                continue;
            }

            // Loaded but wrong LOD -> schedule rebuild (replace mesh)
            if (it->second.lod != desired) {
                chunkBuildQueue_.push_back(BuildRequest{c, desired});
            }
        }
    }

    // 2) unload chunks outside unload radius (unchanged idea)
    const int unload2 = unloadRadius_ * unloadRadius_;
    for (auto it = chunks_.begin(); it != chunks_.end(); ) {
        const int ddx = it->first.x - center.x;
        const int ddz = it->first.z - center.z;
        const int dist2 = ddx * ddx + ddz * ddz;

        if (dist2 > unload2) {
            if (it->second.node) it->second.node->queue_free();
            it = chunks_.erase(it);
        } else {
            ++it;
        }
    }
}

TerrainLevelOfDetail TerrainGenerator::lodForDistance(int dist_chunks) const noexcept
{
    if (dist_chunks <= lodLevel0Distance_) return TerrainLevelOfDetail::LEVEL_0;
    if (dist_chunks <= lodLevel1Distance_) return TerrainLevelOfDetail::LEVEL_1;
    if (dist_chunks <= lodLevel2Distance_) return TerrainLevelOfDetail::LEVEL_2;
    return TerrainLevelOfDetail::LEVEL_3;
}

void TerrainGenerator::resolvePlayerNode()
{
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

}