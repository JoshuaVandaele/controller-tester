CC				= gcc
CSTD            := -std=c23
LIBS			= -levdev
INCLUDE_DIRS	= -I/usr/include -I/usr/local/include -I/usr/include/libevdev-1.0
CFLAGS_RELEASE 	= $(CSTD) -O2 $(LIBS) $(INCLUDE_DIRS)
CFLAGS_DEBUG	= -g -O0 -fsanitize=address,undefined $(CSTD) -Wall -Werror $(LIBS) $(INCLUDE_DIRS)

SRC_DIR		= ./src
BUILD_DIR	= ./build
OBJ_DIR     = $(BUILD_DIR)/obj
BIN_DIR		= $(BUILD_DIR)/bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

TARGET = $(BIN_DIR)/controller-tester

all: release

release: CFLAGS := $(CFLAGS_RELEASE)
release: $(TARGET)

debug: CFLAGS := $(CFLAGS_DEBUG)
debug: $(TARGET)

# Link object files
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all release debug