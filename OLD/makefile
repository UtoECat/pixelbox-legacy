
BUILD_DIR ?= ./build
BINARY_DIR ?= ./
SRC_DIRS ?= ./src 
INC_DIRS ?= ./src ./include

PLATFORM ?= LINUX

ifeq ($(PLATFORM),WINDOWS)
COMPILER ?= x86_64-w64-mingw32-g++
TARGET_NAME ?= pixelbox.exe
LDLIBS= -static -lsqlite3 -fstack-protector -lstdc++ -lwinpthread -lopengl32 -lgdi32 -luser32 ./libglfw3dll.a -lglew32
BUILD_DIR := ./buildwin
else
TARGET_NAME ?= pixelbox
COMPILER ?= $(CXX)
LDLIBS= -lGL -lm -lsqlite3 -lglfw
endif

MKDIR_P  ?= mkdir -p
RM       ?= rm

DEBUG = 1

ifeq ($(DEBUG),1)
CCFLAGS = -O0 -Wall -Wextra -g
else
CCFLAGS = -O2 -Wall
UNIFLAGS = -flto
endif

# no edit!
TARGET = $(BINARY_DIR)/$(TARGET_NAME)
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_FILES := $(shell find $(INC_DIRS) -type d) 
INC_FLAGS := $(addprefix -I,$(INC_FILES))

CCFLAGS += $(INC_FLAGS) -MMD -MP
UNIFLAGS += 

# targets

.PHONY: all clean $(TARGET)

all : $(TARGET)
	@echo "[MAKE] Sucess!"

$(TARGET): $(OBJS)
	@echo "[MAKE] Building target for $(PLATFORM)..."
	@$(MKDIR_P) $(dir $@)
	@$(COMPILER) $(OBJS) $(UNIFLAGS) -o $@ $(LDLIBS) 

# c source
$(BUILD_DIR)/%.cpp.o: %.cpp
	@echo "[MAKE] Building $<"
	@$(MKDIR_P) $(dir $@)
	$(COMPILER) $(CCFLAGS) $(UNIFLAGS) -c $< -o $@

clean:
	@echo "[MAKE] Cleaning up..."
	@$(RM) -rf $(BUILD_DIR)

-include $(DEPS)

