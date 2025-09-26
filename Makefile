TARGET = circuita
BUILDDIR = build

all: build run

test: build
	@./$(BUILDDIR)/test_resistor
	@./$(BUILDDIR)/test_util

run: 
	@./$(BUILDDIR)/$(TARGET)

build: nob
	@./nob

clean:
	@rm -rf $(BUILDDIR)

.PHONY: build run clean all test
