#!/bin/bash
#
# ESP32 Kernel Build Script for Debian 13
# This script installs dependencies, sets up the toolchain, and builds the kernel
#

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
ESP_TOOLCHAIN_VERSION="esp-13.2.0_20230928"
ESP_TOOLCHAIN_ARCH="x86_64-linux-gnu"
TOOLCHAIN_DIR="$HOME/.espressif"
TOOLCHAIN_URL="https://github.com/espressif/crosstool-NG/releases/download/${ESP_TOOLCHAIN_VERSION}/xtensa-esp32-elf-13.2.0_20230928-${ESP_TOOLCHAIN_ARCH}.tar.xz"

# Function to print colored messages
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check if running on Debian/Ubuntu
check_os() {
    print_info "Checking operating system..."
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        if [[ "$ID" == "debian" ]] || [[ "$ID" == "ubuntu" ]]; then
            print_success "Running on $PRETTY_NAME"
        else
            print_warning "This script is designed for Debian/Ubuntu. You may encounter issues."
        fi
    else
        print_warning "Could not detect OS. Continuing anyway..."
    fi
}

# Install system dependencies
install_dependencies() {
    print_info "Installing system dependencies..."

    # Check if we have sudo/root privileges
    if ! command_exists sudo; then
        print_error "sudo not found. Please run as root or install sudo."
        exit 1
    fi

    # Update package list
    print_info "Updating package lists..."
    sudo apt-get update

    # Install required packages
    print_info "Installing build tools and dependencies..."
    sudo apt-get install -y \
        git \
        wget \
        flex \
        bison \
        gperf \
        python3 \
        python3-pip \
        python3-venv \
        cmake \
        ninja-build \
        ccache \
        libffi-dev \
        libssl-dev \
        dfu-util \
        libusb-1.0-0 \
        make \
        gcc \
        g++ \
        xz-utils

    print_success "Dependencies installed successfully"
}

# Download and install ESP32 toolchain
install_toolchain() {
    print_info "Setting up ESP32 toolchain..."

    # Create toolchain directory
    mkdir -p "$TOOLCHAIN_DIR"

    TOOLCHAIN_PATH="$TOOLCHAIN_DIR/xtensa-esp32-elf"

    # Check if toolchain already exists
    if [ -d "$TOOLCHAIN_PATH" ] && [ -f "$TOOLCHAIN_PATH/bin/xtensa-esp32-elf-gcc" ]; then
        print_warning "Toolchain already installed at $TOOLCHAIN_PATH"
        print_info "Skipping download. To reinstall, remove: $TOOLCHAIN_PATH"
    else
        print_info "Downloading ESP32 toolchain..."
        TEMP_FILE="/tmp/xtensa-esp32-elf.tar.xz"

        wget -O "$TEMP_FILE" "$TOOLCHAIN_URL" || {
            print_error "Failed to download toolchain"
            exit 1
        }

        print_info "Extracting toolchain..."
        mkdir -p "$TOOLCHAIN_PATH"
        tar -xf "$TEMP_FILE" -C "$TOOLCHAIN_DIR"

        # Cleanup
        rm -f "$TEMP_FILE"

        print_success "Toolchain installed to $TOOLCHAIN_PATH"
    fi

    # Add toolchain to PATH for this session
    export PATH="$TOOLCHAIN_PATH/bin:$PATH"

    # Verify installation
    if command_exists xtensa-esp32-elf-gcc; then
        TOOLCHAIN_VERSION=$(xtensa-esp32-elf-gcc --version | head -n1)
        print_success "Toolchain verified: $TOOLCHAIN_VERSION"
    else
        print_error "Toolchain installation failed"
        exit 1
    fi
}

# Install esptool for flashing
install_esptool() {
    print_info "Installing esptool..."

    if command_exists esptool.py; then
        print_warning "esptool.py already installed"
    else
        pip3 install --user esptool || {
            print_warning "Failed to install esptool via pip3, trying apt..."
            sudo apt-get install -y esptool || {
                print_error "Could not install esptool"
                exit 1
            }
        }
        print_success "esptool installed"
    fi
}

# Add toolchain to shell profile
setup_environment() {
    print_info "Setting up environment variables..."

    SHELL_RC=""
    if [ -n "$BASH_VERSION" ]; then
        SHELL_RC="$HOME/.bashrc"
    elif [ -n "$ZSH_VERSION" ]; then
        SHELL_RC="$HOME/.zshrc"
    else
        SHELL_RC="$HOME/.profile"
    fi

    EXPORT_LINE="export PATH=\"$TOOLCHAIN_DIR/xtensa-esp32-elf/bin:\$PATH\""

    if ! grep -q "xtensa-esp32-elf" "$SHELL_RC" 2>/dev/null; then
        print_info "Adding toolchain to $SHELL_RC"
        echo "" >> "$SHELL_RC"
        echo "# ESP32 Toolchain" >> "$SHELL_RC"
        echo "$EXPORT_LINE" >> "$SHELL_RC"
        print_success "Environment configured. Run 'source $SHELL_RC' or restart your terminal."
    else
        print_info "Toolchain already in $SHELL_RC"
    fi
}

# Build the kernel
build_kernel() {
    print_info "Building ESP32 kernel..."

    # Ensure we're in the project directory
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    cd "$SCRIPT_DIR"

    # Export toolchain path
    export PATH="$TOOLCHAIN_DIR/xtensa-esp32-elf/bin:$PATH"

    # Clean previous build
    if [ -d "build" ]; then
        print_info "Cleaning previous build..."
        make clean
    fi

    # Build
    print_info "Compiling kernel..."
    make || {
        print_error "Build failed!"
        exit 1
    }

    # Check if binary was created
    if [ -f "build/esp32-kernel.bin" ]; then
        BINARY_SIZE=$(stat -f%z "build/esp32-kernel.bin" 2>/dev/null || stat -c%s "build/esp32-kernel.bin" 2>/dev/null)
        print_success "Build successful!"
        print_success "Binary: build/esp32-kernel.bin (${BINARY_SIZE} bytes)"
        echo ""
        print_info "To flash to ESP32:"
        echo -e "  ${GREEN}make flash ESPTOOL_PORT=/dev/ttyUSB0${NC}"
        echo ""
        print_info "To monitor serial output:"
        echo -e "  ${GREEN}make monitor${NC}"
    else
        print_error "Binary not found after build"
        exit 1
    fi
}

# Display usage information
show_usage() {
    echo "ESP32 Kernel Build Script for Debian 13"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --install-only    Install dependencies and toolchain only"
    echo "  --build-only      Build kernel only (requires toolchain)"
    echo "  --clean           Clean build artifacts"
    echo "  --help            Show this help message"
    echo ""
    echo "Default behavior (no options): Install everything and build"
}

# Main script logic
main() {
    echo ""
    echo "======================================"
    echo "  ESP32 Kernel Build Script"
    echo "  Debian 13 / Ubuntu"
    echo "======================================"
    echo ""

    # Parse command line arguments
    case "${1:-}" in
        --help)
            show_usage
            exit 0
            ;;
        --install-only)
            check_os
            install_dependencies
            install_toolchain
            install_esptool
            setup_environment
            print_success "Installation complete!"
            exit 0
            ;;
        --build-only)
            build_kernel
            exit 0
            ;;
        --clean)
            print_info "Cleaning build artifacts..."
            make clean 2>/dev/null || rm -rf build
            print_success "Clean complete"
            exit 0
            ;;
        "")
            # Default: do everything
            check_os
            install_dependencies
            install_toolchain
            install_esptool
            setup_environment
            build_kernel
            print_success "Setup and build complete!"
            ;;
        *)
            print_error "Unknown option: $1"
            show_usage
            exit 1
            ;;
    esac

    echo ""
    echo "======================================"
    print_success "All done!"
    echo "======================================"
    echo ""
}

# Run main function
main "$@"
