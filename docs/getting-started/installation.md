# Installation Guide

This guide will help you install Myco on your system and get ready to start programming.

## System Requirements

Myco supports the following operating systems:
- **macOS** 10.15+ (Intel and Apple Silicon)
- **Linux** (Ubuntu 18.04+, CentOS 7+, Debian 9+)
- **Windows** 10+ (with WSL2 recommended)

### Hardware Requirements
- **RAM**: 1GB minimum, 2GB recommended
- **Storage**: 800KB for installation
- **CPU**: x86_64 or ARM64 architecture

## Installation Methods

### Method 1: Pre-built Binaries (Recommended)

Download the latest release from our GitHub releases page:

1. Visit [https://github.com/ivymycelia/Myco/releases](https://github.com/ivymycelia/Myco/releases)
2. Download the appropriate binary for your system:
   - `myco-macos-x64` for Intel Macs
   - `myco-macos-arm64` for Apple Silicon Macs
   - `myco-linux-x64` for Linux
   - `myco-windows-x64.exe` for Windows

3. Make the binary executable and move it to your PATH:

**macOS/Linux:**
```bash
chmod +x myco-macos-x64
sudo mv myco-macos-x64 /usr/local/bin/myco
```

**Windows:**
```cmd
move myco-windows-x64.exe C:\Program Files\Myco\myco.exe
# Add C:\Program Files\Myco to your PATH
```

### Method 2: Build from Source

If you prefer to build Myco from source or want the latest development version:

#### Prerequisites
- **C Compiler**: GCC 7+ or Clang 6+
- **Make**: GNU Make 4.0+
- **Git**: For cloning the repository

#### Build Steps

1. **Clone the repository:**
   ```bash
   git clone https://github.com/ivymycelia/Myco.git
   cd Myco
   ```

2. **Build Myco:**
   ```bash
   make
   ```

3. **Install (optional):**
   ```bash
   sudo make install
   ```

4. **Run tests to verify installation:**
   ```bash
   ./bin/myco pass.myco
   ```

### Method 3: Package Managers

#### Homebrew (macOS)
```bash
brew tap ivymycelia/myco
brew install myco
```

#### Linux Package Managers
**Ubuntu/Debian:**
```bash
# Add our repository (coming soon)
sudo apt update
sudo apt install myco
```

**CentOS/RHEL:**
```bash
# Add our repository (coming soon)
sudo yum install myco
```

## Verification

After installation, verify that Myco is working correctly:

1. **Check version:**
   ```bash
   myco --version
   ```
   Expected output: `Myco v1.0.0`

2. **Run a simple test:**
   ```bash
   echo 'print("Hello, Myco!");' > hello.myco
   myco hello.myco
   ```
   Expected output: `Hello, Myco!`

3. **Run the full test suite:**
   ```bash
   myco pass.myco
   ```
   Expected output: `ALL TESTS PASSED!` (297/297 tests)

## IDE Integration

### Visual Studio Code

1. Install the Myco extension from the VS Code marketplace
2. Open any `.myco` file
3. Enjoy syntax highlighting, IntelliSense, and debugging support

### Other Editors

Myco provides Language Server Protocol (LSP) support for:
- **Vim/Neovim** (with LSP client)
- **Emacs** (with LSP mode)
- **Sublime Text** (with LSP package)
- **Atom** (with LSP package)

## Troubleshooting

### Common Issues

**Issue**: `myco: command not found`
- **Solution**: Ensure Myco is in your PATH. Try using the full path to the binary.

**Issue**: Permission denied when running `myco`
- **Solution**: Make the binary executable: `chmod +x myco`

**Issue**: Build fails with "missing dependencies"
- **Solution**: Install required build tools (GCC/Clang, Make)

**Issue**: Tests fail on Windows
- **Solution**: Use WSL2 or ensure you have the Visual C++ Redistributable installed

### Getting Help

If you encounter issues not covered here:

1. **Check the [FAQ](https://mycolang.org/faq)**
2. **Search [GitHub Issues](https://github.com/ivymycelia/Myco/issues)**
3. **Join our [Discord community](https://discord.gg/myco)**
4. **Create a new issue** with details about your system and the error

## Next Steps

Once Myco is installed, you're ready to start programming! Check out:

- [Quick Start Guide](quick-start.md) - Write your first Myco program
- [Language Tour](language-tour.md) - Explore Myco's features
- [Tutorials](../tutorials/) - Step-by-step learning guides

---

**Welcome to Myco!** 🚀 Start coding and discover the power of modern, efficient programming.
