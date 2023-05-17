TARGET_NAME ?= pixelbox

BUILD_DIR ?= ./build
BINARY_DIR ?= ./
SRC_DIRS ?= ./src 
INC_DIRS ?= ./src

COMPILER ?= $(CXX)
MKDIR_P  ?= mkdir -p
RM       ?= rm

DEBUG = TRUE

ifdef DEBUG
CCFLAGS = -O0 -Wall -Wextra
UNIFLAGS = -fsanitize=address -fsanitize=undefined
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
LDLIBS    = -lGL -lglfw -lm

# targets

.PHONY: all clean

all : $(TARGET)
	@echo "[MAKE] Sucess!"

$(TARGET): $(OBJS)
	@echo "[MAKE] Building target..."
	@$(MKDIR_P) $(dir $@)
	@$(COMPILER) $(OBJS) $(UNIFLAGS) -o $@ $(LDLIBS) 

# c source
$(BUILD_DIR)/%.cpp.o: %.cpp
	@echo "[MAKE] Building $<"
	@$(MKDIR_P) $(dir $@)
	@$(COMPILER) $(CCFLAGS) $(UNIFLAGS) -c $< -o $@

clean:
	@echo "[MAKE] Cleaning up..."
	@$(RM) -rf $(BUILD_DIR)

-include $(DEPS)

