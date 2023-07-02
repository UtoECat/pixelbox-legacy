
BUILD_DIR ?= ./build
BINARY_DIR ?= ./
SRC_DIRS ?= ./src 
INC_DIRS ?= ./src

TARGET_NAME ?= pixelbox
COMPILER ?= gcc
LDLIBS= -lGL -lm -lsqlite3 -lraylib

MKDIR_P  ?= mkdir -p
RM       ?= rm

DEBUG = 1

ifeq ($(DEBUG),1)
CCFLAGS  = -O0 -Wall -Wextra -g -DPBOX_DEBUG=1 
UNIFLAGS = -fsanitize=address -fsanitize=undefined
else
CCFLAGS  = -O2 -Wall -DPBOX_DEBUG=0
UNIFLAGS = -flto
endif

# no edit!
TARGET = $(BINARY_DIR)/$(TARGET_NAME)
SRCS := $(shell find $(SRC_DIRS) -name '*.c' -or -name '*.cpp')
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
$(BUILD_DIR)/%.c.o: %.c
	@echo "[MAKE] Building $<"
	@$(MKDIR_P) $(dir $@)
	$(COMPILER) $(CCFLAGS) $(UNIFLAGS) -c $< -o $@

# cpp source
$(BUILD_DIR)/%.cpp.o: %.cpp
	@echo "[MAKE] Building $<"
	@$(MKDIR_P) $(dir $@)
	$(COMPILER) $(CCFLAGS) $(UNIFLAGS) -c $< -o $@

clean:
	@echo "[MAKE] Cleaning up..."
	@$(RM) -rf $(BUILD_DIR)

-include $(DEPS)

