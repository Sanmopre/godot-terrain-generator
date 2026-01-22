#include "noise_generator.h"

NoiseGenerator::NoiseGenerator()
{
    noiseGenerator_ = std::make_unique<FastNoiseLite>();
}

f64 NoiseGenerator::getNoiseValue(f64 x, f64 y) const noexcept
{
    return noiseGenerator_->GetNoise<f64>(x, y);
}
