CC = gcc

DEBUG_FLAGS = -g -DDEBUG
CFLAGS_DEFAULT = -Wall -Wextra -pedantic -std=c11
CFLAGS = $(CFLAGS_DEFAULT) $(DEBUG_FLAGS)
OPT_FLAGS = $(CFLAGS_DEFAULT) -O2 -flto -march=native


OPT_LD_FLAGS = -flto
LDFLAGS = 

TARGET = gradeviewer
BUILD_DIR = build

rebuild = 0
test = 0
ausan = 0
arg1 =
arg2 =
arg3 =


ifneq ($(rebuild),0)
	CLEAN_DEPENDENCY = clean
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

LIB_SRCS := $(sort $(wildcard src/json/internal/*.c src/json/internal/lexer/*.c src/json/utils/*.c src/json/utils/*/*.c))
LIB_OBJS := $(LIB_SRCS:%.c=$(BUILD_DIR)/lib/%.o)
LIB_TARGET := $(BUILD_DIR)/libslowjson.a

FUZZ_ITERATIONS ?= 100000
FUZZ_SEED ?= 1
FUZZ_THREADS ?= 1
FUZZ_TARGET := $(BUILD_DIR)/fuzz/slowjson-fuzz
FUZZ_OBJS := $(LIB_SRCS:%.c=$(BUILD_DIR)/fuzz/%.o) $(BUILD_DIR)/fuzz/tools/fuzz.o
FUZZ_FLAGS := -pthread -std=c11 -Wall -Wextra -pedantic -g -DDEBUG -O1 -fno-omit-frame-pointer -fsanitize=address,undefined -fno-sanitize-recover=all

# Builds normally
all: $(CLEAN_DEPENDENCY) $(BUILD_DIR)/$(TARGET)

lib: $(CLEAN_DEPENDENCY) $(LIB_TARGET)

fuzz: $(FUZZ_TARGET)
	$(FUZZ_TARGET) $(FUZZ_ITERATIONS) $(FUZZ_SEED) $(FUZZ_THREADS)

$(FUZZ_TARGET): $(FUZZ_OBJS)
	$(CC) $(FUZZ_FLAGS) $(FUZZ_OBJS) -o $@

$(BUILD_DIR)/fuzz/%.o: %.c
	$(call MKDIR,$(dir $@))
	$(CC) $(FUZZ_FLAGS) -Isrc -MMD -MP -c $< -o $@

-include $(FUZZ_OBJS:.o=.d)

$(LIB_TARGET): $(LIB_OBJS)
	$(call MKDIR,$(dir $@))
	$(AR) rcs $@ $(LIB_OBJS)

$(BUILD_DIR)/lib/%.o: %.c
	$(call MKDIR,$(dir $@))
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(LIB_OBJS:.o=.d)

run: all
ifeq ($(OS),Windows_NT)
	$(call FIX_PATH,$(BUILD_DIR)/$(TARGET))
else
	./$(BUILD_DIR)/$(TARGET) $(arg1) $(arg2) $(arg3)
endif

profile: CFLAGS := $(CFLAGS) -g -DDEBUG
profile: all
	valgrind --tool=callgrind --dump-instr=yes ./$(BUILD_DIR)/$(TARGET) $(arg1) $(arg2) $(arg3)

valgrind: CFLAGS := $(CFLAGS) -g -DDEBUG
valgrind: all
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

.PHONY: all lib fuzz clean build run valgrind
