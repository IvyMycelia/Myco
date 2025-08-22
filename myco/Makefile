CC = gcc
CFLAGS_DEV  = -O2 -std=c99 -Iinclude -g -Wall -Wextra 
CFLAGS_REL  = -O3 -std=c99 -Iinclude -DNDEBUG -fomit-frame-pointer -flto -march=native -mtune=native
CFLAGS_PROD = -O3 -std=c99 -Iinclude -DNDEBUG -fomit-frame-pointer -flto -s -march=native -mtune=native
CLINK = -lm

# Platform-specific production flags
ifeq ($(shell uname),Darwin)
    # macOS: clang doesn't support --gc-sections
    CFLAGS_PROD += -fdata-sections -ffunction-sections
else
    # Linux/Windows: full optimization
    CFLAGS_PROD += -fdata-sections -ffunction-sections -Wl,--gc-sections
endif

SRC = src/main.c src/lexer.c src/parser.c src/eval.c src/codegen.c src/memory_tracker.c src/loop_manager.c
OUT = myco
WINCC = x86_64-w64-mingw32-gcc
WINOUT = myco.exe

# Development build (default) - with debug info and warnings
all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS_DEV) -o $(OUT) $(SRC) $(CLINK)

# Release build - optimized for speed and size
release: $(SRC)
	$(CC) $(CFLAGS_REL) -o $(OUT)_release $(SRC) $(CLINK)

# Production build - maximum optimization, stripped
prod: $(SRC)
	$(CC) $(CFLAGS_PROD) -o $(OUT)_prod $(SRC) $(CLINK)

# Profile-guided optimization (PGO) - maximum performance
pgo: profile_gen profile_use

# Generate profiling data
profile_gen: $(SRC)
	$(CC) $(CFLAGS_DEV) -fprofile-generate -o $(OUT)_profile $(SRC)
	@echo "Profile generation build created. Run some Myco programs to collect data:"
	@echo "./$(OUT)_profile your_program.myco"

# Use profiling data for optimization
profile_use: $(SRC)
	@if [ -f *.gcda ]; then \
		$(CC) $(CFLAGS_REL) -fprofile-use -fprofile-correction -o $(OUT)_pgo $(SRC); \
		echo "PGO build created: $(OUT)_pgo"; \
	else \
		echo "No profiling data found. Run 'make profile_gen' first, then execute some programs."; \
	fi

# Windows builds with optimizations
windows: $(SRC)
	$(WINCC) $(CFLAGS_DEV) -o $(WINOUT) $(SRC) -lws2_32 -lcrypt32

windows_release: $(SRC)
	$(WINCC) -O3 -std=c99 -Iinclude -DNDEBUG -fomit-frame-pointer -flto -s -o $(WINOUT)_release $(SRC) -lws2_32 -lcrypt32

# ARM64-specific optimizations for Apple Silicon
arm64: $(SRC)
	$(CC) $(CFLAGS_REL) -mcpu=apple-m1 -mtune=native -o $(OUT)_arm64 $(SRC)

# Clean all build artifacts and profiling data
clean:
	rm -f $(OUT) $(OUT)_release $(OUT)_prod $(OUT)_pgo $(OUT)_profile $(OUT)_arm64 $(WINOUT) $(WINOUT)_release output.c
	rm -f *.gcda *.gcno

# Size comparison and analysis
size: all release prod
	@echo "=== Binary Size Analysis ==="
	@echo "Development: $(shell du -h $(OUT) | cut -f1)"
	@echo "Release: $(shell du -h $(OUT)_release | cut -f1)"
	@echo "Production: $(shell du -h $(OUT)_prod | cut -f1)"
	@echo ""
	@echo "=== Performance Builds ==="
	@echo "make release    - Optimized for speed"
	@echo "make prod       - Maximum optimization"
	@echo "make pgo        - Profile-guided optimization (best performance)"
	@echo "make arm64      - Apple Silicon optimized"
	@echo "make windows_release - Windows optimized"

# Quick performance test
bench: prod
	@echo "=== Performance Benchmark ==="
	@echo "Testing production build..."
	@time ./$(OUT)_prod test_programs/*.myco 2>/dev/null || echo "No test programs found" 
