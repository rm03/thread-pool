#pragma once

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

#include <vector>

void generatePerlinNoiseImage(const char* filename, int width, int height, unsigned int seed) {
    std::vector<unsigned char> image(width * height * 3);

    float scale = 0.1;  // Scale factor for noise detail
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float n = stb_perlin_noise3(x * scale, y * scale, seed, 0, 0, 0);
            unsigned char value = static_cast<unsigned char>((n + 1.0) * 0.5 * 255);
            int index = (y * width + x) * 3;
            image[index] = value;           // Red channel
            image[index + 1] = 255 - value; // Green channel
            image[index + 2] = (value + 128) % 256; // Blue channel
        }
    }

    stbi_write_png(filename, width, height, 3, image.data(), width * 3);
}
