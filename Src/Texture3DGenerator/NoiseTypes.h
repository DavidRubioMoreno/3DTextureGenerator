#pragma once

#include "FastNoiseLite.h"
#include "anl.h"

// TIPOS DE GENERACION
enum class SeamlessMode
{
    None,
    XYZ
};

// TIPOS DE TILES
enum class TileMode
{
    None,        
    PeriodicXYZ, // seamless real (trigonométrico)
    Blend        // interpolación de bordes (más simple)
};

// TIPOS DE RUIDO
enum class NoiseType
{
    OpenSimplex2,
    OpenSimplex2S,
    Cellular,
    Perlin,
    ValueCubic,
    Value
};


// TIPOS DE FRACTAL
enum class FractalType
{
    None,
    FBm,
    Ridged,
    PingPong,
    DomainWarpProgressive,
    DomainWarpIndependent
};

FastNoiseLite::NoiseType ToFastNoise(NoiseType type)
{
    switch (type)
    {
    case NoiseType::OpenSimplex2:  return FastNoiseLite::NoiseType_OpenSimplex2;
    case NoiseType::OpenSimplex2S: return FastNoiseLite::NoiseType_OpenSimplex2S;
    case NoiseType::Cellular:      return FastNoiseLite::NoiseType_Cellular;
    case NoiseType::Perlin:        return FastNoiseLite::NoiseType_Perlin;
    case NoiseType::ValueCubic:    return FastNoiseLite::NoiseType_ValueCubic;
    case NoiseType::Value:         return FastNoiseLite::NoiseType_Value;
    default:                       return FastNoiseLite::NoiseType_OpenSimplex2;
    }
}

FastNoiseLite::FractalType ToFastFractal(FractalType type)
{
    switch (type)
    {
    case FractalType::None:                  return FastNoiseLite::FractalType_None;
    case FractalType::FBm:                   return FastNoiseLite::FractalType_FBm;
    case FractalType::Ridged:                return FastNoiseLite::FractalType_Ridged;
    case FractalType::PingPong:              return FastNoiseLite::FractalType_PingPong;
    case FractalType::DomainWarpProgressive: return FastNoiseLite::FractalType_DomainWarpProgressive;
    case FractalType::DomainWarpIndependent: return FastNoiseLite::FractalType_DomainWarpIndependent;
    default:                                return FastNoiseLite::FractalType_None;
    }
}

anl::EBasisTypes ToANLBasis(NoiseType type)
{
    switch (type)
    {
    case NoiseType::OpenSimplex2:
    case NoiseType::OpenSimplex2S:
        return anl::SIMPLEX;

    case NoiseType::Perlin:
        return anl::GRADIENT;

    case NoiseType::Value:
        return anl::VALUE;

    case NoiseType::ValueCubic:
        return anl::GRADVAL;

    case NoiseType::Cellular:
        return anl::WHITE; // ANL no tiene celular real en EBasisTypes

    default:
        return anl::GRADIENT;
    }
}

anl::EFractalTypes ToANLFractal(FractalType type)
{
    switch (type)
    {
    case FractalType::None:
        return anl::FBM; // fallback seguro

    case FractalType::FBm:
        return anl::FBM;

    case FractalType::Ridged:
        return anl::RIDGEDMULTI;

    case FractalType::PingPong:
        return anl::MULTI; // aproximación

    case FractalType::DomainWarpProgressive:
        return anl::HYBRIDMULTI;

    case FractalType::DomainWarpIndependent:
        return anl::DECARPENTIERSWISS;

    default:
        return anl::FBM;
    }
}

anl::EInterpTypes ToANLInterp(int interpolationMode)
{
    switch (interpolationMode)
    {
    case 0: return anl::NONE;
    case 1: return anl::LINEAR;
    case 2: return anl::CUBIC;
    case 3: return anl::QUINTIC;
    default: return anl::QUINTIC;
    }
}

inline anl::CImplicitFractal CreateANLFractalFull(
    FractalType fractal,
    NoiseType basis,
    anl::EInterpTypes interp,
    int octaves,
    double frequency,
    double lacunarity,
    double gain,
    unsigned int seed
)
{
    anl::CImplicitFractal noise(
        (unsigned int)ToANLFractal(fractal),
        (unsigned int)ToANLBasis(basis),
        (unsigned int)interp
    );

    noise.setNumOctaves(octaves);
    noise.setFrequency(frequency);
    noise.setLacunarity(lacunarity);
    noise.setGain(gain);
    noise.setSeed(seed);

    return noise;
}