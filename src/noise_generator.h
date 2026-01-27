#pragma once

#include "utils.h"
 
// FastNoiseLite
#include "FastNoiseLite.h"

// std
#include <memory>

enum NoiseType {
    NOISE_OPENSIMPLEX2 = 0,
    NOISE_PERLIN = 1,
    NOISE_CELLULAR = 2
};

enum FractalType {
    FRACTAL_FBM = 0,
    FRACTAL_RIDGED = 1,
    FRACTAL_PINGPONG = 2
};

struct NoiseSettings
{
    // Core
    i32 seed = 1337;
    FastNoiseLite::NoiseType noise_type = FastNoiseLite::NoiseType_OpenSimplex2;
    f32 frequency = 0.001f;

    // Fractal
    FastNoiseLite::FractalType fractal_type = FastNoiseLite::FractalType_FBm;
    i32 octaves = 5;
    f32 lacunarity = 2.0f;
    f32 gain = 0.5f;
    f32 weighted_strength = 0.0f; // 0 = off, >0 adds ridgy weighting
    f32 ping_pong_strength = 2.0f; // only used if fractal_type == PingPong

    // Domain warp (optional)
    bool domain_warp_enabled = false;
    FastNoiseLite::DomainWarpType domain_warp_type = FastNoiseLite::DomainWarpType_OpenSimplex2;
    f32 domain_warp_amp = 30.0f;

    // Output shaping (VERY useful for terrain)
    f32 height_scale = 10.0f;   // multiplies final height
    f32 height_offset = 0.0f;   // adds after scale
};


class NoiseGenerator
{

public:
    NoiseGenerator();
    ~NoiseGenerator() = default;

public:
    [[nodiscard]] f64 getNoiseValue(f64 x, f64 y) const noexcept;
    void applySettings(const NoiseSettings& settings) noexcept;

private:
    std::unique_ptr<FastNoiseLite> noiseGenerator_;
};
