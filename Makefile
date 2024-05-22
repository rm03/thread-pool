CXX = g++
CXXFLAGS = -std=c++20 -Wall -Werror -O2 -g
LDFLAGS = -lpthread

BENCHMARK = benchmark.out
IMAGE_GEN = image_gen.out

BENCHMARK_SRC = benchmark.cpp
IMAGE_GEN_SRC = examples/image_generation.cpp

BENCHMARK_OBJ = $(BENCHMARK_SRC:.cpp=.o)
IMAGE_GEN_OBJ = $(IMAGE_GEN_SRC:.cpp=.o)

HEADERS = thread_pool.hpp thread_safe_queue.hpp

.PHONY: all clean benchmark image_gen

all: $(BENCHMARK) $(IMAGE_GEN)

$(BENCHMARK): $(BENCHMARK_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -lbenchmark 
$(IMAGE_GEN): $(IMAGE_GEN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(BENCHMARK) $(IMAGE_GEN) $(BENCHMARK_OBJ) $(IMAGE_GEN_OBJ)

benchmark: $(BENCHMARK)

image_gen: $(IMAGE_GEN)
