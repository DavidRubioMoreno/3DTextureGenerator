# 3D Texture Generator (C++)

## Description

**3D Texture Generator** is a C++ application designed to procedurally generate volumetric (3D) textures using coherent noise.

The program generates a **256³ noise volume** and packs it into a **4096×4096 2D atlas**, where each slice of the volume is arranged in a 16×16 tile grid.

This type of texture is compatible with engines like **Unreal Engine 5**, where it can be used as a **Volume Texture** for effects such as:

* Volumetric clouds
* Fog
* Procedural materials
* VFX effects

---

## Features

* Procedural 3D noise generation
* Support for multiple noise types:

  * OpenSimplex2
  * Perlin
  * Cellular
  * Value / ValueCubic
* Fractal support:

  * FBm
  * Ridged
  * PingPong
  * Domain Warp
* Multithreaded generation (leverages modern CPUs)
* PNG image export
* Configurable via `constexpr` parameters

---

## How it works

1. A noise generator (FastNoiseLite) is configured
2. A 3D volume of size `256×256×256` is evaluated
3. Each slice along the Z axis is placed into a 2D atlas:

   * 16×16 tiles
   * Each tile is 256×256
4. The final result is saved as a grayscale image (`volume_atlas.png`)

---

## Multithreading

The generation process is parallelized by splitting the volume into Z-slice ranges.

Each thread:

* Processes its own slice range
* Uses its own noise instance (thread-safe)
* Writes to a separate region of the image buffer

This allows efficient use of multi-core CPUs (e.g., 16 threads).

---

## Libraries Used

### FastNoiseLite

* High-performance procedural noise generation
* Supports multiple algorithms and fractals
* Header-only (easy integration)

### stb_image_write

* Lightweight image writing library
* Supports multiple formats (PNG, BMP, TGA, JPG, HDR)
* Used to export the generated atlas

---

## Configuration

Core parameters are defined using `constexpr` in the code:

```cpp
constexpr int VOLUME_SIZE = 256;
constexpr float NOISE_SCALE = 0.1f;
constexpr int NOISE_SEED = 1337;
```

You can also configure:

* Noise type
* Fractal type
* Number of octaves
* Lacunarity and gain

---

## Output

The program generates:

```bash
volume_atlas.png
```

* Resolution: **4096×4096**
* Format: **Grayscale (single channel)**
* Content: slices of a 3D volume

---

## Usage in Unreal Engine 5

1. Import the texture into UE5
2. Disable **sRGB**
3. Set compression (e.g., *Masks*)
4. Convert to a **Volume Texture** if needed
5. Use it in volumetric materials

---

## Possible Improvements

* DDS export (better GPU integration)
* Tileable 3D noise
* Multi-channel output (RGBA)
* GUI for parameter editing
* Direct export to Volume Texture

---

## Author - David Rubio Moreno

Developed as a tool for procedural 3D texture generation in C++.

---
