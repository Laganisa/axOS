#!/bin/bash
# 커널 디버깅 자동화 스크립트

KERNEL_PATH="$(dirname "$0")/../build/kernel8.elf"
WORK_DIR="$(dirname "$0")/.."

# 경로 확인
if [ ! -f "$KERNEL_PATH" ]; then
    echo "❌ Error: kernel8.elf not found. Run 'make' first."
    exit 1
fi

echo "🚀 Starting QEMU with GDB support..."
echo "📍 GDB will listen on localhost:1234"
echo "⏸️  CPU is stopped. Use 'continue' or 'c' in GDB to run."
echo ""
echo "In another terminal, run:"
echo "  wsl -e aarch64-linux-gnu-gdb $KERNEL_PATH"
echo ""

cd "$WORK_DIR"

qemu-system-aarch64 \
    -M virt \
    -cpu cortex-a72 \
    -m 512M \
    -kernel "$KERNEL_PATH" \
    -gdb tcp::1234 \
    -S \
    -nographic
