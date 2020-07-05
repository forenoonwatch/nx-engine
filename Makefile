PROJECT_NAME := NXEngine

LIB_DIR := C:/cpplibs

BUILD_DIR := bin
SRC_DIR := src

TARGET := lib$(PROJECT_NAME).a

rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d)) 
MKDIR_P := mkdir -p

SRCS := $(call rwildcard, $(SRC_DIR)/, *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)


UNAME := $(shell uname -s)

ifeq ($(UNAME), Linux)
	BULLET := $(shell pkg-config bullet --static --cflags)
	FREETYPE := $(shell pkg-config freetype2 --static --cflags)

	CXXFLAGS := -I$(CURDIR)/include/engine -I$(CURDIR)/include $(FREETYPE) $(BULLET) -msse2 $(CXXFLAGS)
else
	BULLET := -I$(LIB_DIR)/include/bullet
	FREETYPE := -I$(LIB_DIR)/include/freetype2
	CXXFLAGS := -I$(CURDIR)/include/engine -I$(CURDIR)/include -I$(LIB_DIR)/include $(FREETYPE) $(BULLET) -msse2 $(CXXFLAGS)
endif

CPPFLAGS := -std=c++17 -g

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	@$(AR) rcsu $@ $(OBJS)

$(BUILD_DIR)/%.cpp.o: %.cpp
	@echo "Building $(notdir $@)..."
	@$(MKDIR_P) $(dir $@)
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: all
