#pragma once


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