#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <thread>
#include <cmath>
#include <chrono>
#include "FastNoiseLite.h"

#include "NoiseTypes.h"

#define PI 3.14159265f

// CONFIGURACIÓN 
constexpr int VOLUME_SIZE = 256;
constexpr int ATLAS_TILES = 16;
constexpr int TILE_SIZE = VOLUME_SIZE;
constexpr int IMAGE_SIZE = ATLAS_TILES * TILE_SIZE;

constexpr float NOISE_SCALE = 0.1f;
constexpr int NOISE_SEED = 1337;
constexpr int NOISE_FRACTAL_OCTAVES = 8;
constexpr float NOISE_LACUNARITY = 2.0f;
constexpr float NOISE_GAIN = 0.5f;
constexpr NoiseType NOISE_TYPE = NoiseType::OpenSimplex2S;
constexpr FractalType FRACTAL_TYPE = FractalType::FBm;
constexpr TileMode TILE_MODE = TileMode::Blend;
constexpr float TILE_REPEAT = 1.0f; // cuántas veces repite dentro del volumen

constexpr int NUM_THREADS = 8; // puedes usar std::thread::hardware_concurrency()


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

float SampleNoise(FastNoiseLite& noise, float x, float y, float z)
{
    if (TILE_MODE == TileMode::None)
    {
        return noise.GetNoise(x, y, z);
    }

    else if (TILE_MODE == TileMode::PeriodicXYZ)
    {
        float fx = x / VOLUME_SIZE;
        float fy = y / VOLUME_SIZE;
        float fz = z / VOLUME_SIZE;

        float ax = fx * TILE_REPEAT * 2.0f * PI;
        float ay = fy * TILE_REPEAT * 2.0f * PI;
        float az = fz * TILE_REPEAT * 2.0f * PI;

        float nx = cos(ax);
        float ny = sin(ax);

        float nz = cos(ay);
        float nw = sin(ay);

        float nu = cos(az);
        float nv = sin(az);

        return noise.GetNoise(
            nx + nz + nu,
            ny + nw + nv,
            nz + nu
        );
    }

    else if (TILE_MODE == TileMode::Blend)
    {
        float blendX = x / VOLUME_SIZE;
        float blendY = y / VOLUME_SIZE;
        float blendZ = z / VOLUME_SIZE;

        float n000 = noise.GetNoise(x, y, z);
        float n100 = noise.GetNoise(x - VOLUME_SIZE, y, z);
        float n010 = noise.GetNoise(x, y - VOLUME_SIZE, z);
        float n001 = noise.GetNoise(x, y, z - VOLUME_SIZE);

        float nx = n000 * (1 - blendX) + n100 * blendX;
        float ny = n000 * (1 - blendY) + n010 * blendY;
        float nz = n000 * (1 - blendZ) + n001 * blendZ;

        return (nx + ny + nz) / 3.0f;
    }

    return 0.0f;
}


// FUNCIÓN DE TRABAJO POR HILO
void GenerateSlices(
    std::vector<uint8_t>& image,
    int zStart,
    int zEnd
)
{
    // Cada hilo tiene su propia instancia (MUY IMPORTANTE)
    FastNoiseLite noise;
    noise.SetSeed(NOISE_SEED);
    noise.SetNoiseType(ToFastNoise(NOISE_TYPE));
    noise.SetFractalType(ToFastFractal(FRACTAL_TYPE));
    noise.SetFrequency(NOISE_SCALE);
    noise.SetFractalOctaves(NOISE_FRACTAL_OCTAVES);
    noise.SetFractalLacunarity(NOISE_LACUNARITY);
    noise.SetFractalGain(NOISE_GAIN);

    for (int z = zStart; z < zEnd; z++)
    {
        int tileX = z % ATLAS_TILES;
        int tileY = z / ATLAS_TILES;

        for (int y = 0; y < VOLUME_SIZE; y++)
        {
            for (int x = 0; x < VOLUME_SIZE; x++)
            {
                int atlasX = tileX * TILE_SIZE + x;
                int atlasY = tileY * TILE_SIZE + y;
                int index = atlasY * IMAGE_SIZE + atlasX;

                float nx = x * NOISE_SCALE;
                float ny = y * NOISE_SCALE;
                float nz = z * NOISE_SCALE;

                float n = SampleNoise(noise, nx, ny, nz);

                float normalized = (n + 1.0f) * 0.5f;
                uint8_t value = static_cast<uint8_t>(normalized * 255.0f);

                image[index] = value;
            }
        }
    }
}

// MAIN
int main()
{
    std::cout << "Generating 3D noise atlas (multithreaded)..." << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<uint8_t> image(IMAGE_SIZE * IMAGE_SIZE);

    std::vector<std::thread> threads;

    int slicesPerThread = VOLUME_SIZE / NUM_THREADS;

    for (int t = 0; t < NUM_THREADS; t++)
    {
        int zStart = t * slicesPerThread;
        int zEnd = (t == NUM_THREADS - 1)
            ? VOLUME_SIZE
            : zStart + slicesPerThread;

        threads.emplace_back(GenerateSlices, std::ref(image), zStart, zEnd);
    }

    // esperar a todos los hilos
    for (auto& th : threads)
        th.join();

    std::cout << "Generation complete, saving..." << std::endl;

    int success = stbi_write_png(
        "volume_atlas.png",
        IMAGE_SIZE,
        IMAGE_SIZE,
        1,
        image.data(),
        IMAGE_SIZE
    );

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Time: "
        << std::chrono::duration<double>(end - start).count()
        << "s\n";

    if (success)
        std::cout << "Saved volume_atlas.png" << std::endl;
    else
        std::cout << "Error saving image!" << std::endl;

    return 0;
}