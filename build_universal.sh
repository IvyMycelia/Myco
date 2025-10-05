#!/bin/bash

# Myco Universal Build Script
# Supports: Linux, macOS, Windows, FreeBSD, OpenBSD
# Architectures: x86_64, ARM64, ARM

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Detect platform
detect_platform() {
    case "$(uname -s)" in
        Linux*)     echo "linux" ;;
        Darwin*)    echo "macos" ;;
        CYGWIN*|MINGW*|MSYS*) echo "windows" ;;
        FreeBSD*)   echo "freebsd" ;;
        OpenBSD*)   echo "openbsd" ;;
        *)          echo "unknown" ;;
    esac
}

# Detect architecture
detect_arch() {
    case "$(uname -m)" in
        x86_64|amd64)   echo "x86_64" ;;
        arm64|aarch64)  echo "arm64" ;;
        arm*)          echo "arm" ;;
        *)              echo "unknown" ;;
    esac
}

# Get optimization flags for platform
get_optimization_flags() {
    local platform=$1
    local arch=$2
    
    case $platform in
        linux)
            echo "-O3 -march=native -mtune=native -flto -fuse-linker-plugin"
            ;;
        macos)
            if [[ $arch == "arm64" ]]; then
                echo "-O3 -mcpu=apple-m1 -flto"
            else
                echo "-O3 -march=native -flto"
            fi
            ;;
        windows)
            echo "-O3 -march=native -flto"
            ;;
        freebsd|openbsd)
            echo "-O3 -march=native -flto"
            ;;
        *)
            echo "-O3 -flto"
            ;;
    esac
}

# Get compiler for platform
get_compiler() {
    local platform=$1
    
    case $platform in
        linux|freebsd|openbsd)
            if command -v gcc >/dev/null 2>&1; then
                echo "gcc"
            elif command -v clang >/dev/null 2>&1; then
                echo "clang"
            else
                echo "gcc"
            fi
            ;;
        macos)
            if command -v clang >/dev/null 2>&1; then
                echo "clang"
            else
                echo "gcc"
            fi
            ;;
        windows)
            if command -v gcc >/dev/null 2>&1; then
                echo "gcc"
            elif command -v clang >/dev/null 2>&1; then
                echo "clang"
            else
                echo "gcc"
            fi
            ;;
        *)
            echo "gcc"
            ;;
    esac
}

# Main build function
build_myco() {
    local platform=$(detect_platform)
    local arch=$(detect_arch)
    local compiler=$(get_compiler $platform)
    local opt_flags=$(get_optimization_flags $platform $arch)
    
    echo -e "${BLUE}Building Myco for ${platform} (${arch}) using ${compiler}${NC}"
    echo -e "${YELLOW}Optimization flags: ${opt_flags}${NC}"
    
    # Set compiler flags
    export CC="$compiler"
    export CFLAGS="$opt_flags -Wall -Wextra -std=c99"
    export LDFLAGS="-flto"
    
    # Platform-specific flags
    case $platform in
        macos)
            export CFLAGS="$CFLAGS -mmacosx-version-min=10.15"
            ;;
        windows)
            export CFLAGS="$CFLAGS -static-libgcc"
            ;;
    esac
    
    # Clean previous build
    echo -e "${YELLOW}Cleaning previous build...${NC}"
    make clean 2>/dev/null || true
    
    # Build with optimizations
    echo -e "${YELLOW}Building with optimizations...${NC}"
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
    
    # Test the build
    echo -e "${YELLOW}Testing build...${NC}"
    if ./bin/myco --version >/dev/null 2>&1; then
        echo -e "${GREEN}✓ Build successful!${NC}"
    else
        echo -e "${RED}✗ Build failed!${NC}"
        exit 1
    fi
    
    # Run test suite
    echo -e "${YELLOW}Running test suite...${NC}"
    if ./pass >/dev/null 2>&1; then
        echo -e "${GREEN}✓ All tests passed!${NC}"
    else
        echo -e "${RED}✗ Some tests failed!${NC}"
        exit 1
    fi
    
    # Show binary info
    echo -e "${BLUE}Binary Information:${NC}"
    ls -lh bin/myco
    file bin/myco
    
    echo -e "${GREEN}Myco built successfully for ${platform} (${arch})!${NC}"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            make clean
            exit 0
            ;;
        --test)
            ./pass
            exit 0
            ;;
        --help)
            echo "Usage: $0 [--clean] [--test] [--help]"
            echo "  --clean: Clean build artifacts"
            echo "  --test:  Run test suite"
            echo "  --help:  Show this help"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
    shift
done

# Run build
build_myco
