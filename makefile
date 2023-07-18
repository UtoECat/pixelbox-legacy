BUILD_DIR ?= ./build
BINARY_DIR ?= ./

SOURCES ?= ./src/ext/ ./src/
INC_DIRS ?= ./src

CLIENT_NAME ?= pixelbox
SERVER_NAME ?= pixelbox-server
COMPILER    ?= gcc
LDLIBS       = -lGL -lm -lraylib -lluajit

MKDIR_P  ?= mkdir -p
RM       ?= rm

DEBUG = 0

ifeq ($(DEBUG),1)
CCFLAGS  = -O0 -Wall -Wextra -g -DPBOX_DEBUG=1 -Wno-unused
UNIFLAGS = -fsanitize=address -fsanitize=undefined
else
CCFLAGS  = -O2 -Wall -DPBOX_DEBUG=0
UNIFLAGS = -s
endif

# no edit!
SERVER_TARGET = $(BINARY_DIR)/$(SERVER_NAME)
CLIENT_TARGET = $(BINARY_DIR)/$(CLIENT_NAME)
TARGET = TARGET
.PHONY: $(TARGET) all clean server client

$(TARGET) : $(SERVER_TARGET) $(CLIENT_TARGET)

SRCS := $(shell find $(SOURCES) -name '*.c' -or -name '*.cpp')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)
INC_FILES := $(shell find $(INC_DIRS) -type d) 
INC_FLAGS := $(addprefix -I,$(INC_FILES))

CCFLAGS += $(INC_FLAGS) -MMD -MP
UNIFLAGS += 

# targets

server : $(SERVER_TARGET)
client : $(CLIENT_TARGET)

all : $(TARGET)
	@echo "[MAKE] Sucess!"

$(SERVER_TARGET): $(OBJS) ./build/server.o
	@echo "[MAKE] Building server target for $(PLATFORM)..."
	@$(MKDIR_P) $(dir $@)
	@$(COMPILER) $(OBJS) $(UNIFLAGS) -o $@ $(LDLIBS) 

$(CLIENT_TARGET): $(OBJS) ./build/client.o
	@echo "[MAKE] Building client target for $(PLATFORM)..."
	@$(MKDIR_P) $(dir $@)
	@$(COMPILER) $(OBJS) $(UNIFLAGS) -o $@ $(LDLIBS) 

LUA_ALL = $(shell find $(./src/shared) -name '*.lua')
LUA_SRV = $(shell find $(./src/server) -name '*.lua') $(LUA_ALL)
LUA_CLI = $(shell find $(./src/client) -name '*.lua') $(LUA_ALL)

./build/server.o: $(LUA_SRV)
	@echo "precompiling lua sources in $@"
	@luajit ./tools/ar.lua $< | gcc -x c -o $@

./build/client.o: $(LUA_CLI)
	@echo "precompiling lua sources in $@"
	@luajit ./tools/ar.lua $< | gcc -x c -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	@echo "[MAKE] Building $<"
	@$(MKDIR_P) $(dir $@)
	$(COMPILER) $(CCFLAGS) $(UNIFLAGS) -c $< -o $@

clean:
	@echo "[MAKE] Cleaning up..."
	@$(RM) -rf $(BUILD_DIR)

-include $(DEPS)

