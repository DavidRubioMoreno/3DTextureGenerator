#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>
#include <cmath>
#include "FastNoiseLite.h"

// ==============================
// CONFIGURACIÓN 
// ==============================
constexpr int VOLUME_SIZE = 256;          // 256^3
constexpr int ATLAS_TILES = 16;           // 16x16 tiles
constexpr int TILE_SIZE = VOLUME_SIZE;    // 256
constexpr int IMAGE_SIZE = ATLAS_TILES * TILE_SIZE; // 4096

constexpr float NOISE_SCALE = 0.02f;
constexpr int NOISE_SEED = 1337;
constexpr int NOISE_FRACTAL_OCTAVES = 5;
constexpr float NOISE_LACUNARITY = 2.0f;
constexpr float NOISE_GAIN = 0.5f;

// ==============================
// MAIN
// ==============================
int main()
{
    std::cout << "Generating 3D noise atlas..." << std::endl;

    // Buffer de imagen (grayscale)
    std::vector<uint8_t> image(IMAGE_SIZE * IMAGE_SIZE);

    // ==============================
    // CONFIGURAR FASTNOISELITE
    // ==============================
    FastNoiseLite noise;
    noise.SetSeed(NOISE_SEED);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(NOISE_SCALE);

    // Opcional (mejor calidad)
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    noise.SetFractalOctaves(NOISE_FRACTAL_OCTAVES);
    noise.SetFractalLacunarity(NOISE_LACUNARITY);
    noise.SetFractalGain(NOISE_GAIN);

    // ==============================
    // GENERAR VOLUMEN - ATLAS
    // ==============================
    for (int z = 0; z < VOLUME_SIZE; z++)
    {
        // calcular tile en el atlas
        int tileX = z % ATLAS_TILES;
        int tileY = z / ATLAS_TILES;

        for (int y = 0; y < VOLUME_SIZE; y++)
        {
            for (int x = 0; x < VOLUME_SIZE; x++)
            {
                // coordenadas globales en atlas
                int atlasX = tileX * TILE_SIZE + x;
                int atlasY = tileY * TILE_SIZE + y;

                int index = atlasY * IMAGE_SIZE + atlasX;

                // ==============================
                // SAMPLE DE RUIDO 3D
                // ==============================
                float nx = x * NOISE_SCALE;
                float ny = y * NOISE_SCALE;
                float nz = z * NOISE_SCALE;

                float n = noise.GetNoise(nx, ny, nz); // [-1,1]

                // convertir a [0,255]
                float normalized = (n + 1.0f) * 0.5f;
                uint8_t value = static_cast<uint8_t>(normalized * 255.0f);

                image[index] = value;
            }
        }

        if (z % 32 == 0)
            std::cout << "Slice " << z << " / " << VOLUME_SIZE << std::endl;
    }

    // ==============================
    // GUARDAR IMAGEN
    // ==============================
    int success = stbi_write_png(
        "volume_atlas.png",
        IMAGE_SIZE,
        IMAGE_SIZE,
        1, // 1 canal (grayscale)
        image.data(),
        IMAGE_SIZE
    );

    if (success)
        std::cout << "Saved volume_atlas.png" << std::endl;
    else
        std::cout << "Error saving image!" << std::endl;

    return 0;
}