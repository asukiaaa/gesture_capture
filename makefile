# Sets of flags
REALSENSE_FLAGS := -Iinclude -Llib -lrealsense -lm

GLFW3_FLAGS := `pkg-config --cflags --libs glfw3 glu gl`

OPENCV_FLAGS := -ggdb `pkg-config --cflags opencv` `pkg-config --libs opencv`

CXXFLAGS := -std=c++11

# Compute a list of all example program binaries
SOURCES := $(wildcard src/*.cpp)
SOURCES := $(addprefix bin/, $(notdir $(basename $(SOURCES))))

# Aliases for convenience
all: prepare $(SOURCES)

clean:
	rm -rf bin

prepare: bin

bin:
	mkdir -p bin

bin/%: src/%.cpp
	$(CXX) $< -o $@	 $(REALSENSE_FLAGS) $(GLFW3_FLAGS) $(OPENCV_FLAGS) $(CXXFLAGS)
