CXX = g++
CXXFLAGS = -std=c++20 -Wall -Werror -O2
TARGET = main.out 
SRCS = main.cpp thread_pool.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean: 
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
