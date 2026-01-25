#include "noise_generator.h"

NoiseGenerator::NoiseGenerator()
{
    noiseGenerator_ = std::make_unique<FastNoiseLite>();
    applySettings(NoiseSettings{});
}

void NoiseGenerator::applySettings(const NoiseSettings& settings) noexcept
{
    noiseGenerator_->SetSeed(settings.seed);
    noiseGenerator_->SetNoiseType(settings.noise_type);
    noiseGenerator_->SetFrequency(settings.frequency);

    // Fractal settings
    noiseGenerator_->SetFractalType(settings.fractal_type);
    noiseGenerator_->SetFractalOctaves(settings.octaves);
    noiseGenerator_->SetFractalLacunarity(settings.lacunarity);
    noiseGenerator_->SetFractalGain(settings.gain);
    noiseGenerator_->SetFractalWeightedStrength(settings.weighted_strength);
    noiseGenerator_->SetFractalPingPongStrength(settings.ping_pong_strength);

    // Domain warp settings
    if (settings.domain_warp_enabled) {
        noiseGenerator_->SetDomainWarpType(settings.domain_warp_type);
        noiseGenerator_->SetDomainWarpAmp(settings.domain_warp_amp);
    }
}

f64 NoiseGenerator::getNoiseValue(f64 x, f64 y) const noexcept
{
    auto noiseValue = noiseGenerator_->GetNoise<f64>(x, y);
    noiseValue = (noiseValue + 1.0) / 2.0;
    return noiseValue;
}
