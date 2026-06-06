CC ?= cc
CPPFLAGS ?=
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -g
LDFLAGS ?=
LDLIBS ?=

BUILD_DIR := build
TEST_DIR := tests

LIB_OBJECTS := $(BUILD_DIR)/object.o
TEST_BIN := $(BUILD_DIR)/test_refcounting
TEST_SOURCES := $(TEST_DIR)/test_object.c

CRITERION_CFLAGS := $(shell pkg-config --cflags criterion 2>/dev/null)
CRITERION_LIBS := $(shell pkg-config --libs criterion 2>/dev/null || echo -lcriterion)

.PHONY: all test clean

all: $(LIB_OBJECTS)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/object.o: object.c object.h | $(BUILD_DIR)
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(TEST_SOURCES) $(LIB_OBJECTS) object.h | $(BUILD_DIR)
	@$(CC) $(CPPFLAGS) $(CFLAGS) $(CRITERION_CFLAGS) -I. $(TEST_SOURCES) $(LIB_OBJECTS) $(LDFLAGS) $(LDLIBS) $(CRITERION_LIBS) -o $@

test: $(TEST_BIN)
	@./$(TEST_BIN)

clean:
	@rm -rf $(BUILD_DIR)
