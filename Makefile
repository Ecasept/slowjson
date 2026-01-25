CC = gcc

DEBUG_FLAGS = -g -DDEBUG
OPT_FLAGS = -O2 # -flto -march=native
CFLAGS = -Wall -Wextra -pedantic -std=c11 $(DEBUG_FLAGS) $(OPT_FLAGS)

OPT_LD_FLAGS = # -flto
LDFLAGS = -lm $(OPT_LD_FLAGS)

TARGET = gradeviewer
BUILD_DIR = build

rebuild = 0
test = 0
ausan = 0
arg1 =
arg2 =
arg3 =


ifneq ($(rebuild),0)
	REBUILD_DEPENDENCY = clean all
endif

ifneq ($(test),0)
	CFLAGS += -DRUN_TESTS
	TARGET := gradeviewer_tests
endif
ifneq ($(ausan),0)
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
all: $(REBUILD_DEPENDENCY) $(BUILD_DIR)/$(TARGET)

run: all
ifeq ($(OS),Windows_NT)
	$(call FIX_PATH,$(BUILD_DIR)/$(TARGET))
else
	./$(BUILD_DIR)/$(TARGET) $(arg1) $(arg2) $(arg3)
endif

profile: CFLAGS := $(CFLAGS) -g -DDEBUG
profile: $(REBUILD_DEPENDENCY)
	valgrind --tool=callgrind --dump-instr=yes ./$(BUILD_DIR)/$(TARGET) $(arg1) $(arg2) $(arg3)

valgrind: CFLAGS := $(CFLAGS) -g -DDEBUG
valgrind: $(REBUILD_DEPENDENCY)
	valgrind --leak-check=full --show-leak-kinds=all --main-stacksize=1000000 ./$(BUILD_DIR)/$(TARGET) $(arg1) $(arg2) $(arg3)

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
