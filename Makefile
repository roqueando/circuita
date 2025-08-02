TARGET = lamina
BUILDDIR = build/debug

all: build run

test: build
	@cd $(BUILDDIR)/test && ctest --output-on-failure

run: $(BUILDDIR)/Makefile
	@cd $(BUILDDIR) && ./$(TARGET)

build: $(BUILDDIR)/Makefile
	@cmake --build build/debug

$(BUILDDIR)/Makefile:
	@cmake -DCMAKE_BUILD_TYPE=Debug -B build/debug

clean:
	@$(MAKE) -C $(BUILDDIR) clean
	@rm -rf $(BUILDDIR)

.PHONY: build run clean all test
