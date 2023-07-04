BUILD_DIR ?= ./build
BINARY_DIR ?= ./

SERVER_SOURCES ?= ./src/ext/ ./src/server ./src/shared
CLIENT_SOURCES ?= ./src/ext/ ./src/client ./src/shared
INC_DIRS ?= ./src

CLIENT_NAME ?= pixelbox
SERVER_NAME ?= pixelbox-server
COMPILER    ?= gcc
LDLIBS       = -lGL -lm -lsqlite3 -lraylib

MKDIR_P  ?= mkdir -p
RM       ?= rm

DEBUG = 1

ifeq ($(DEBUG),1)
CCFLAGS  = -O0 -Wall -Wextra -g -DPBOX_DEBUG=1 -Wno-unused
UNIFLAGS = -fsanitize=address -fsanitize=undefined
else
CCFLAGS  = -O2 -Wall -DPBOX_DEBUG=0
UNIFLAGS = -flto
endif

# no edit!
SERVER_TARGET = $(BINARY_DIR)/$(SERVER_NAME)
CLIENT_TARGET = $(BINARY_DIR)/$(CLIENT_NAME)
TARGET = TARGET
.PHONY: $(TARGET) all clean server client

$(TARGET) : $(SERVER_TARGET) $(CLIENT_TARGET)

SSRCS := $(shell find $(SERVER_SOURCES) -name '*.c' -or -name '*.cpp')
SOBJS := $(SSRCS:%=$(BUILD_DIR)/%.o)
SDEPS := $(SOBJS:.o=.d)
CSRCS := $(shell find $(CLIENT_SOURCES) -name '*.c' -or -name '*.cpp')
COBJS := $(CSRCS:%=$(BUILD_DIR)/%.o)
CDEPS := $(COBJS:.o=.d)
INC_FILES := $(shell find $(INC_DIRS) -type d) 
INC_FLAGS := $(addprefix -I,$(INC_FILES))

CCFLAGS += $(INC_FLAGS) -MMD -MP
UNIFLAGS += 

# targets

server : $(SERVER_TARGET)
client : $(CLIENT_TARGET)

all : $(TARGET)
	@echo "[MAKE] Sucess!"

$(SERVER_TARGET): $(SOBJS)
	@echo "[MAKE] Building server target for $(PLATFORM)..."
	@$(MKDIR_P) $(dir $@)
	$(COMPILER) $(SOBJS) $(UNIFLAGS) -o $@ $(LDLIBS) 

$(CLIENT_TARGET): $(COBJS)
	@echo "[MAKE] Building client target for $(PLATFORM)..."
	@$(MKDIR_P) $(dir $@)
	$(COMPILER) $(COBJS) $(UNIFLAGS) -o $@ $(LDLIBS) 

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

