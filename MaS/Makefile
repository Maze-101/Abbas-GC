CC ?= cc
CPPFLAGS ?=
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -g
LDFLAGS ?=
LDLIBS ?=

BUILD_DIR := build
TEST_DIR := tests

SOURCES := object.c new.c stack.c vm.c
OBJECTS := $(SOURCES:%.c=$(BUILD_DIR)/%.o)
TEST_BIN := $(BUILD_DIR)/test_mas
TEST_SOURCES := $(TEST_DIR)/test_mas.c

CRITERION_CFLAGS := $(shell pkg-config --cflags criterion 2>/dev/null)
CRITERION_LIBS := $(shell pkg-config --libs criterion 2>/dev/null || echo -lcriterion)

.PHONY: all test clean

all: $(OBJECTS)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: %.c object.h new.h stack.h vm.h | $(BUILD_DIR)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_SOURCES) $(OBJECTS) object.h new.h stack.h vm.h | $(BUILD_DIR)
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(CRITERION_CFLAGS) -I. $(TEST_SOURCES) $(OBJECTS) $(LDFLAGS) $(LDLIBS) $(CRITERION_LIBS) -o $@

test: $(TEST_BIN)
	@./$(TEST_BIN)

clean:
	@rm -rf $(BUILD_DIR)
