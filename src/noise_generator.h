#pragma once

#include "utils.h"
 
// FastNoiseLite
#include "FastNoiseLite.h"

// std
#include <memory>

class NoiseGenerator
{

public:
    NoiseGenerator();
    ~NoiseGenerator() = default;

public:
    [[nodiscard]] f64 getNoiseValue(f64 x, f64 y) const noexcept;

private:
    std::unique_ptr<FastNoiseLite> noiseGenerator_;
};
