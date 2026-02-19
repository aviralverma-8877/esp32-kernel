# Building on Debian 13 / Ubuntu

Quick guide to build the ESP32 kernel on Debian 13 or Ubuntu systems.

## Quick Start

### Automatic Build (Recommended)

The easiest way to build on Debian/Ubuntu:

```bash
# Clone the repository
git clone https://github.com/aviralverma-8877/esp32-kernel.git
cd esp32-kernel

# Run the build script (installs everything and builds)
./build-debian.sh
```

This script will:
1. Install system dependencies (git, make, gcc, etc.)
2. Download and install ESP32 toolchain
3. Install esptool for flashing
4. Configure your environment
5. Build the kernel

### Script Options

```bash
# Install dependencies and toolchain only (no build)
./build-debian.sh --install-only

# Build only (assumes toolchain is installed)
./build-debian.sh --build-only

# Clean build artifacts
./build-debian.sh --clean

# Show help
./build-debian.sh --help
```

## Manual Installation

If you prefer to install manually:

### 1. Install System Dependencies

```bash
sudo apt-get update
sudo apt-get install -y \
    git wget flex bison gperf \
    python3 python3-pip python3-venv \
    cmake ninja-build ccache \
    libffi-dev libssl-dev dfu-util \
    libusb-1.0-0 make gcc g++ xz-utils
```

### 2. Install ESP32 Toolchain

```bash
# Create directory for toolchain
mkdir -p ~/.espressif

# Download toolchain
cd ~/.espressif
wget https://github.com/espressif/crosstool-NG/releases/download/esp-13.2.0_20230928/xtensa-esp32-elf-13.2.0_20230928-x86_64-linux-gnu.tar.xz

# Extract
tar -xf xtensa-esp32-elf-13.2.0_20230928-x86_64-linux-gnu.tar.xz

# Add to PATH
echo 'export PATH="$HOME/.espressif/xtensa-esp32-elf/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

### 3. Install esptool

```bash
pip3 install --user esptool
```

### 4. Build the Kernel

```bash
cd esp32-kernel
make
```

## Verification

After installation, verify the toolchain:

```bash
# Check compiler
xtensa-esp32-elf-gcc --version

# Check esptool
esptool.py --version

# Check make
make --version
```

Expected output:
```
xtensa-esp32-elf-gcc (crosstool-NG esp-13.2.0_20230928) 13.2.0
esptool.py v4.x
GNU Make 4.x
```

## Build Output

Successful build produces:
- `build/esp32-kernel.elf` - ELF executable
- `build/esp32-kernel.bin` - Binary for flashing
- `build/esp32-kernel.map` - Memory map

## Flashing

Connect your ESP32 and flash:

```bash
# Find your serial port
ls /dev/ttyUSB*  # or /dev/ttyACM*

# Flash (replace /dev/ttyUSB0 with your port)
make flash ESPTOOL_PORT=/dev/ttyUSB0

# If permission denied, add user to dialout group
sudo usermod -a -G dialout $USER
# Then log out and back in
```

## Monitoring

View serial output:

```bash
# Using make
make monitor ESPTOOL_PORT=/dev/ttyUSB0

# Or using screen
screen /dev/ttyUSB0 115200

# Or using miniterm
python3 -m serial.tools.miniterm /dev/ttyUSB0 115200
```

## Troubleshooting

### Permission Denied on Serial Port

```bash
# Add user to dialout group
sudo usermod -a -G dialout $USER

# Apply changes (logout and login, or use newgrp)
newgrp dialout
```

### Toolchain Not Found

```bash
# Make sure PATH is set
export PATH="$HOME/.espressif/xtensa-esp32-elf/bin:$PATH"

# Verify
which xtensa-esp32-elf-gcc
```

### Build Errors

```bash
# Clean and rebuild
make clean
make

# Check dependencies
./build-debian.sh --install-only
```

### esptool Not Found

```bash
# Install via pip
pip3 install --user esptool

# Or via apt
sudo apt-get install esptool
```

## ARM64 / aarch64 Systems

For ARM-based systems (like Raspberry Pi), use the ARM64 toolchain:

```bash
# Download ARM64 toolchain instead
wget https://github.com/espressif/crosstool-NG/releases/download/esp-13.2.0_20230928/xtensa-esp32-elf-13.2.0_20230928-aarch64-linux-gnu.tar.xz

# Extract and use as above
```

## Using Docker (Alternative)

If you prefer containerized builds:

```bash
# Pull ESP-IDF container
docker pull espressif/idf:latest

# Build in container
docker run --rm -v $PWD:/project -w /project espressif/idf:latest \
    bash -c "export PATH=/opt/xtensa-esp32-elf/bin:\$PATH && make"
```

## Additional Resources

- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [ESP32 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf)
- [Project README](README.md)

## Support

For issues specific to this kernel, open an issue on GitHub:
https://github.com/aviralverma-8877/esp32-kernel/issues
