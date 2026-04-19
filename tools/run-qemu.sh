#!/bin/bash
# QEMU에서 커널 실행 (EL2 지원, GICv2 포함, 타이머 활성화)

if [ ! -f "build/kernel8.elf" ]; then
    echo "Error: kernel8.elf not found. Please run 'make' first."
    exit 1
fi

echo "Running kernel in QEMU..."
echo "To exit: Press Ctrl+A then X"
echo ""

# ✅ 수정사항:
# -machine gic-version=2,virtualization=on
#   → gic-version=2: GICv2 사용 (GICv3 대신, 더 직관적)
#   → virtualization=on: EL2 지원 활성화 (중요!)
# -enable-kvm : KVM 활성화 (타이머 정확도 개선, 선택사항)
qemu-system-aarch64 \
    -M virt \
    -machine gic-version=2,virtualization=on \
    -cpu cortex-a72 \
    -m 512M \
    -kernel build/kernel8.elf \
    -nographic


