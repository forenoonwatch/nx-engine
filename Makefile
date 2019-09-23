PROJECT_NAME := NXEngine

LIB_DIR := C:/cpplibs

BUILD_DIR := bin
SRC_DIR := src

TARGET := lib$(PROJECT_NAME).a

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d)) 
MKDIR_P := mkdir -p

SRCS := $(call rwildcard, $(SRC_DIR)/, *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

CPPFLAGS := -std=c++17 -g
CXXFLAGS := -I$(CURDIR)/include/engine -I$(CURDIR)/include -I$(LIB_DIR)/Glew/include -I$(LIB_DIR)/GLFW/include -I$(LIB_DIR)/GLM/include -I$(LIB_DIR)/assimp/include -msse2 $(CXXFLAGS)

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	@$(AR) rcsu $@ $(OBJS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@echo "Building $(notdir $@)..."
	@$(MKDIR_P) $(dir $@)
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: all
