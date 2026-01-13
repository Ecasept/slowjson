CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -g -DDEBUG -O2
LDFLAGS = -lm
TARGET = gradeviewer
BUILD_DIR = build

test = 0
run = 0
debug = 0
arg1 =

ifneq ($(test),0)
	CFLAGS += -DRUN_TESTS
	TARGET := gradeviewer_tests
endif
ifneq ($(debug),0)
	CFLAGS += -DDEBUG -g -fsanitize=address,undefined
	LDFLAGS += -fsanitize=address,undefined
endif

# Platform detection
ifeq ($(OS),Windows_NT)
    TARGET := gradeviewer.exe
    RM = del /Q /F
    RMDIR = rmdir /S /Q
    MKDIR = if not exist "$(subst /,\,$1)" mkdir "$(subst /,\,$1)"
    FIX_PATH = $(subst /,\,$1)
    SRCS := $(sort $(shell dir /s /b src\*.c 2>nul))
    NULL_DEVICE = nul
else
    RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p $1
    FIX_PATH = $1
    SRCS := $(sort $(shell find src -type f -name '*.c' 2>/dev/null))
    NULL_DEVICE = /dev/null
endif

OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)

# Builds normally
all: $(BUILD_DIR)/$(TARGET)

# Cleans and builds everything from scratch
rebuild: clean all

run: all
ifeq ($(OS),Windows_NT)
	$(call FIX_PATH,$(BUILD_DIR)/$(TARGET))
else
	./$(BUILD_DIR)/$(TARGET) $(arg1)
endif

valgrind: CFLAGS := $(CFLAGS) -g -DDEBUG
valgrind: rebuild
	valgrind --tool=callgrind --dump-instr=yes ./$(BUILD_DIR)/$(TARGET) $(arg1)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(call MKDIR,$(dir $@))
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: %.c
	$(call MKDIR,$(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	$(RMDIR) $(call FIX_PATH,$(BUILD_DIR)) 2>$(NULL_DEVICE) || echo Clean complete
else
	$(RMDIR) $(BUILD_DIR)
endif

.PHONY: all clean build run valgrind
