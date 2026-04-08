#pragma once

// ==============================
// TIPOS DE RUIDO
// ==============================
enum class NoiseType
{
    OpenSimplex2,
    OpenSimplex2S,
    Cellular,
    Perlin,
    ValueCubic,
    Value
};

// ==============================
// TIPOS DE FRACTAL
// ==============================
enum class FractalType
{
    None,
    FBm,
    Ridged,
    PingPong,
    DomainWarpProgressive,
    DomainWarpIndependent
};