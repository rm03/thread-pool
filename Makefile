CXX = g++
CXXFLAGS = -std=c++20 -Wall -Werror -O2 -g
TARGET = main.out 
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lbenchmark

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean: 
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
