// Modified matrix multplication benchmark
// Source: https://github.com/CoffeeBeforeArch/mmul/blob/master/src/baseline/benchmark.cpp

#include "benchmark/benchmark.h"
#include "thread_pool.hpp"
#include "image_generation.hpp"
#include <algorithm>
#include <cstdlib>
#include <random>
#include <thread>

// Serial implementation
void serial_mmul(const float *A, const float *B, float *C, std::size_t N) {
    // For each row...
    for (std::size_t row = 0; row < N; row++)
        // For each col...
        for (std::size_t col = 0; col < N; col++)
            // For each element in the row/col pair...
            for (std::size_t idx = 0; idx < N; idx++)
                // Accumulate the partial results
                C[row * N + col] += A[row * N + idx] * B[idx * N + col];
}

// Parallel implementation
void parallel_mmul(const float *A, const float *B, float *C, std::size_t N, std::size_t start_row, std::size_t end_row) {
    // For each row assigned to this thread...
    for (std::size_t row = start_row; row < end_row; row++)
        // For each column...
        for (std::size_t col = 0; col < N; col++)
            // For each element in the row-col pair...
            for (std::size_t idx = 0; idx < N; idx++)
                // Accumulate the partial results
                C[row * N + col] += A[row * N + idx] * B[idx * N + col];
}

// Serial MMul benchmark
static void serial_mmul_bench(benchmark::State &s) {
    // Number Dimensions of our matrix
    std::size_t N = s.range(0);

    // Create our random number generators
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<float> dist(-10, 10);

    // Create input matrices
    float *A = new float[N * N];
    float *B = new float[N * N];
    float *C = new float[N * N];

    // Initialize them with random values (and C to 0)
    std::generate(A, A + N * N, [&] {
        return dist(rng);
    });
    std::generate(B, B + N * N, [&] {
        return dist(rng);
    });
    std::generate(C, C + N * N, [&] {
        return 0.0f;
    });

    // Main benchmark loop
    for (auto _ : s) {
        serial_mmul(A, B, C, N);
    }

    // Free memory
    delete[] A;
    delete[] B;
    delete[] C;
}
BENCHMARK(serial_mmul_bench)->Arg(384)->Arg(768)->Arg(1152)->Unit(benchmark::kMillisecond);

// Parallel MMul benchmark
static void parallel_mmul_bench(benchmark::State &s) {
    // Number Dimensions of our matrix
    std::size_t N = s.range(0);

    // Create our random number generators
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_real_distribution<float> dist(-10, 10);

    // Create input matrices
    float *A = new float[N * N];
    float *B = new float[N * N];
    float *C = new float[N * N];

    // Initialize them with random values (and C to 0)
    std::generate(A, A + N * N, [&] {
        return dist(rng);
    });
    std::generate(B, B + N * N, [&] {
        return dist(rng);
    });
    std::generate(C, C + N * N, [&] {
        return 0.0f;
    });

    // Set up for launching threads
    size_t num_threads = std::thread::hardware_concurrency();

    // Calculate values to pass to threads
    // Assumed to be divisable by num_threads (evenly)
    std::size_t n_rows = N / num_threads;

    // Main benchmark loop
    for (auto _ : s) {
        // Create thread pool
        thread_pool pool;
        std::size_t end_row = 0;
        for (std::size_t i = 0; i < num_threads - 1; i++) {
            auto start_row = i * n_rows;
            end_row = start_row + n_rows;
            pool.enqueue_task([&] {
                parallel_mmul(A, B, C, N, start_row, end_row);
            });
        }
    }

    // Free memory
    delete[] A;
    delete[] B;
    delete[] C;

    // free(A);
    // free(B);
    // free(C);
}
BENCHMARK(parallel_mmul_bench)->Arg(384)->Arg(768)->Arg(1152)->Unit(benchmark::kMillisecond)->UseRealTime();

BENCHMARK_MAIN();
