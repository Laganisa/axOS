#!/bin/bash
# aarch64 크로스 컴파일 도구 설정 스크립트

echo "Setting up aarch64 cross-compilation tools..."

# Debian/Ubuntu
if command -v apt-get &> /dev/null; then
    echo "Installing gcc-aarch64-linux-gnu and binutils-aarch64-linux-gnu..."
    sudo apt-get update
    sudo apt-get install -y gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu qemu-system-arm
    
# Fedora/RHEL
elif command -v dnf &> /dev/null; then
    echo "Installing gcc-aarch64-linux-gnu and binutils-aarch64-linux-gnu..."
    sudo dnf install -y gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu qemu-system-aarch64
    
# Arch
elif command -v pacman &> /dev/null; then
    echo "Installing aarch64-linux-gnu-gcc and qemu..."
    sudo pacman -S aarch64-linux-gnu-gcc aarch64-linux-gnu-binutils qemu-system-aarch64

else
    echo "Unsupported package manager. Please install:"
    echo "  - gcc-aarch64-linux-gnu"
    echo "  - binutils-aarch64-linux-gnu"
    echo "  - qemu-system-aarch64"
    exit 1
fi

# 도구 확인
echo ""
echo "Verifying tools..."
aarch64-linux-gnu-gcc --version
aarch64-linux-gnu-as --version
aarch64-linux-gnu-ld --version

echo ""
echo "Setup complete!"
