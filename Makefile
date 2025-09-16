TARGET = circuita
BUILDDIR = build

all: build run

test:
	@./$(BUILDDIR)/test_resistor
#test: build
#	@cd $(BUILDDIR)/test && ctest --output-on-failure

run: 
	@./$(TARGET)

build: nob
	@./nob

clean:
	@rm -rf $(BUILDDIR)

.PHONY: build run clean all test
