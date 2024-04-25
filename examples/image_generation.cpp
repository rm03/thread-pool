#include "../thread_pool.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

#include <vector>

void generatePerlinNoiseImage(const char *filename, int width, int height, unsigned int seed) {
    std::vector<unsigned char> image(width * height * 3);

    float scale = 0.1;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float n = stb_perlin_noise3(x * scale, y * scale, seed, 0, 0, 0);
            unsigned char value = static_cast<unsigned char>((n + 1.0) * 0.5 * 255);
            int index = (y * width + x) * 3;
            image[index] = value;                   // Red channel
            image[index + 1] = 255 - value;         // Green channel
            image[index + 2] = (value + 128) % 256; // Blue channel
        }
    }

    stbi_write_png(filename, width, height, 3, image.data(), width * 3);
}

void generatePerlinNoiseImage(std::vector<unsigned char> &image, int width, int task_height, unsigned int seed) {
    float scale = 0.1;
    for (int x = 0; x < width; x++) {
        float n = stb_perlin_noise3(x * scale, task_height * scale, seed, 0, 0, 0);
        unsigned char value = static_cast<unsigned char>((n + 1.0) * 0.5 * 255);
        int index = (task_height * width + x) * 3;
        image[index] = value;                   // Red channel
        image[index + 1] = 255 - value;         // Green channel
        image[index + 2] = (value + 128) % 256; // Blue channel
    }
}

int main() {
    
    constexpr int WIDTH = 256;
    constexpr int HEIGHT = 256;
    std::vector<unsigned char> image(WIDTH * HEIGHT * 3);
    thread_pool pool;
    for (int i = 0; i < HEIGHT; i++) {
        pool.enqueue_task([&]() {
            generatePerlinNoiseImage(image, WIDTH, i, 0);
        });
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    stbi_write_png("test.png", WIDTH, HEIGHT, 3, image.data(), WIDTH * 3);
}
