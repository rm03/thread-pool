#include "../thread_pool.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"

#include <chrono>
#include <vector>

void generatePerlinNoiseImage(std::vector<unsigned char> &image, int width, int height, unsigned int seed) {
    float scale = 0.006;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float n = stb_perlin_noise3_seed((x + (seed % 100)) * scale, y * scale, 0, 0, 0, 0, seed);
            int index = (y * width + x) * 3;
            if (n < 0) {
                image[index] = 0;
                image[index + 1] = 0;
                image[index + 2] = 0.5 + 0.5 * (n + 1) * 255;
            } else if (n < 0.4) {
                image[index] = 0;
                image[index + 1] = 0.5 + 0.5 * (n + 1) * 255;
                image[index + 2] = 0;
            } else {
                image[index] = 255 * (0.7 + (0.3 * n));
                image[index + 1] = 255 * (0.7 + (0.3 * n));
                image[index + 2] = 255 * (0.7 + (0.3 * n));
            }
        }
    }
}

void generatePerlinNoiseImageThreaded(std::vector<unsigned char> &image, int width, int task_height, unsigned int seed) {
    float scale = 0.006;
    for (int x = 0; x < width; x++) {
        float n = stb_perlin_noise3_seed(x * scale, task_height * scale, 0, 0, 0, 0, seed);
        int index = (task_height * width + x) * 3;
        if (n < 0) {
            image[index] = 0;
            image[index + 1] = 0;
            image[index + 2] = 0.5 + 0.5 * (n + 1) * 255;
        } else if (n < 0.4) {
            image[index] = 0;
            image[index + 1] = 0.5 + 0.5 * (n + 1) * 255;
            image[index + 2] = 0;
        } else {
            image[index] = 255 * (0.7 + (0.3 * n));
            image[index + 1] = 255 * (0.7 + (0.3 * n));
            image[index + 2] = 255 * (0.7 + (0.3 * n));
        }
    }
}

int main() {
    constexpr int WIDTH = 4096;
    constexpr int HEIGHT = 4096;
    std::vector<unsigned char> image(3 * WIDTH * HEIGHT);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    auto seed = std::rand();

    // multithreaded
    auto start_threaded = std::chrono::high_resolution_clock::now();
    {
        thread_pool pool;
        for (int i = 0; i < HEIGHT; i++) {
            pool.enqueue_task([&, i]() {
                generatePerlinNoiseImageThreaded(image, WIDTH, i, seed);
            });
        }
    }
    auto end_threaded = std::chrono::high_resolution_clock::now();
    stbi_write_png("image.png", WIDTH, HEIGHT, 3, image.data(), WIDTH * 3);

    // single-threaded
    auto start_nonthreaded = std::chrono::high_resolution_clock::now();
    generatePerlinNoiseImage(image, WIDTH, HEIGHT, std::rand());
    auto end_nonthreaded = std::chrono::high_resolution_clock::now();
    stbi_write_png("image2.png", WIDTH, HEIGHT, 3, image.data(), WIDTH * 3);

    auto duration_threaded = std::chrono::duration_cast<std::chrono::milliseconds>(end_threaded - start_threaded);
    auto duration_nonthreaded = std::chrono::duration_cast<std::chrono::milliseconds>(end_nonthreaded - start_nonthreaded);

    std::cout << "Threaded execution time: " << duration_threaded.count() << " ms\n";
    std::cout << "Non-threaded execution time: " << duration_nonthreaded.count() << " ms\n";
}
