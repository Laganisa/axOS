# ARM64 QEMU Bare-metal 타이머/GIC 해결책

## 🔥 18시간 디버깅의 결론: 3가지 핵심 문제

### 문제 1: HCR_EL2 설정 부족

**원인**: 기존 코드가 `HCR_EL2 = (1 << 27)` (TGE만)로 설정

```asm
❌ WRONG:
mov x0, #(1 << 27)      // TGE=1만 설정
msr HCR_EL2, x0

✅ CORRECT:
mov x0, #(1 << 31)      // RW=1: EL1을 64-bit AArch64로 실행
msr HCR_EL2, x0
```text
**영향**: EL1에서 많은 시스템 레지스터 접근이 trap됨 → CNTV_CTL_EL0 쓰기 무시됨

---

### 문제 2: 타이머 선택 오류
**원인**: CNTV(Virtual Timer)를 사용했지만 CNTVOFF_EL2 미설정

**선택지**:
| 타이머 | IRQ | 요구사항 | 추천 |
|--------|-----|---------|------|
| **CNTP** (Physical) | 30 | EL2: CNTHCTL_EL2[0]=1만 필요 | ✅ |
| **CNTV** (Virtual) | 27 | EL2: CNTVOFF_EL2 offset 설정 필수 | ❌ |

**해결책**: CNTP로 변경
```c
// 기존
asm volatile("msr cntv_tval_el0, %0" : : "r"(freq / 10));

// 변경
asm volatile("msr cntp_tval_el0, %0" : : "r"(freq / 10));
```

---

### 문제 3: GIC PPI 비활성화

**원인**: GICD_IGROUPR0의 PPI 비트를 Group1(Non-Secure)로 설정하지 않음

**상황**:

- QEMU: `-machine secure=off` → 커널은 Non-Secure 상태
- GICv2: Distributor가 그룹 별로 관리
  - Bit=0 → Group0 (Secure, 접근 차단)
  - Bit=1 → Group1 (Non-Secure, 허용)

**해결책**:

```c
// GICD_IGROUPR0[30]: bit=1 설정 (Group1/Non-Secure)
*(volatile uint32_t *)(GIC_DIST_BASE + 0x080) |= (1 << 30);

// Distributor ON: 둘 다 켜기
*(volatile uint32_t *)(GIC_DIST_BASE + 0x000) = 0b11;

// CPU Interface ON: 둘 다 켜기
*(volatile uint32_t *)(GIC_CPU_BASE + 0x000) = 0b11;
```

---

## 📝 적용된 모든 변경사항

### 1️⃣ boot/boot.S (EL2 진입점)

```asm
el2_entry:
    // CNTHCTL_EL2: Physical/Virtual Timer 접근 허용
    mov x0, #0x303                  // bits[0,1,8,9]=1
    msr CNTHCTL_EL2, x0

    // CNTVOFF_EL2: Virtual Timer offset = 0 (CNTP와 동일화)
    mov x0, #0
    msr CNTVOFF_EL2, x0

    // HCR_EL2: RW=1 (EL1이 64-bit AArch64 실행)
    mov x0, #(1 << 31)              // RW=1만 필요, 나머지는 0
    msr HCR_EL2, x0
    
    // EL1 진입
    mov x0, #0x3C5
    msr SPSR_EL2, x0
    adr x0, el1_entry
    msr ELR_EL2, x0
    eret
```

### 2️⃣ src/irq.c (GIC + 타이머 초기화)

**init_timer()**:

```c
void init_timer(void)
{
    uint32_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));

    // CNTP 레지스터 사용
    asm volatile("msr cntp_tval_el0, %0" : : "r"(freq / 10));
    asm volatile("msr cntp_ctl_el0, %0" : : "r"(1));
}
```

**init_gic()**:

```c
void init_gic(void)
{
    // Distributor OFF
    *(volatile uint32_t *)(GIC_DIST_BASE + 0x000) = 0;

    // ✅ CRITICAL: GICD_IGROUPR0[30] = 1 (Non-Secure)
    *(volatile uint32_t *)(GIC_DIST_BASE + 0x080) |= (1 << 30);

    // Priority
    *(volatile uint8_t *)(GIC_DIST_BASE + 0x400 + 30) = 0x80;

    // ISENABLER0[30] = 1
    *(volatile uint32_t *)(GIC_DIST_BASE + 0x100) |= (1 << 30);

    // Distributor ON (Group0+Group1)
    *(volatile uint32_t *)(GIC_DIST_BASE + 0x000) = 0b11;

    // CPU Interface
    *(volatile uint32_t *)(GIC_CPU_BASE + 0x004) = 0xFF;
    *(volatile uint32_t *)(GIC_CPU_BASE + 0x000) = 0b11;
}
```

**irq_handler_main()**:

```c
void irq_handler_main(void)
{
    uint32_t iar = *(volatile uint32_t *)(GIC_CPU_BASE + 0x0C);
    uint32_t irq_nr = iar & 0x3FF;

    if (irq_nr == 30)  // ✅ CNTP IRQ = 30 (기존 27에서 변경)
    {
        puts("[Kernel] Tick!\n");
        uint32_t freq;
        asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
        asm volatile("msr cntp_tval_el0, %0" : : "r"(freq / 10));
    }

    *(volatile uint32_t *)(GIC_CPU_BASE + 0x10) = iar;
}
```

### 3️⃣ tools/run-qemu.sh (QEMU 인수)

```bash
qemu-system-aarch64 \
    -M virt \
    -machine gic-version=2,virtualization=on \
    -cpu cortex-a72 \
    -m 512M \
    -kernel build/kernel8.elf \
    -nographic
```

**중요 플래그**:

- `-machine gic-version=2` : GICv2 사용 (GICv3 대신)
- `-machine virtualization=on` : **EL2 활성화 (필수!)**

### 4️⃣ src/main.c (초기화 호출)

```c
void main(void)
{
    uart_init();
    mm_init(&mm_stack, END_KERNEL_ADDR);
    
    // ✅ 인터럽트/타이머 초기화
    init_irq();
    
    puts("myOS kernel\n");
    // ...
}
```

---

## 🧪 테스트 방법

### 1. 빌드

```bash
cd c:\Users\jxlag\OneDrive\Desktop\myOS
make
```

### 2. 실행 (QEMU 콘솔에서 타이머 인터럽트 확인)

```bash
./tools/run-qemu.sh
```

**예상 출력**:

```text
myOS kernel
'help' : list commands
'end'  : exit
Welcome! Have a great time.
[Kernel] Tick!
[Kernel] Tick!
[Kernel] Tick!
...
```

### 3. GDB 디버깅 (타이머 핸들러 확인)

**터미널 1 (QEMU)**:

```bash
qemu-system-aarch64 -M virt -machine gic-version=2,virtualization=on \
    -cpu cortex-a72 -m 512M -kernel build/kernel8.elf \
    -gdb tcp::1234 -serial stdio -S -nographic
```

**터미널 2 (GDB)**:

```bash
aarch64-linux-gnu-gdb build/kernel8.elf
(gdb) target remote :1234
(gdb) b irq_handler_main
(gdb) c
# 타이머 인터럽트 시 멈춤
```

---

## 📊 레지스터 검증 체크리스트

```text
| 레지스터 | 값 | 확인 방법 |
|---------|-----|---------|
| **SPSR_EL1** | 0x3C5 | `mrs x0, spsr_el1` → EL1h 확인 |
| **HCR_EL2** | 0x80000000 | RW=1 (bit 31) |
| **CNTHCTL_EL2** | 0x0303 | bits[0,1,8,9]=1 |
| **CNTP_CTL_EL0** | 0x1 | Enable=1, IMASK=0 |
| **DAIF** | 0x0 | IRQ/FIQ/SError 언마스크 |
| **GICD_CTLR** | 0x3 | Group0+Group1 enable |
| **GICC_CTLR** | 0x3 | Group0+Group1 enable |
| **GICD_ISENABLER0[30]** | 1 | CNTP 인터럽트 활성화 |
```

---

## ⚡ 핵심 인사이트

### 왜 PPI 비트가 안 켜졌는가?

```text
GICD_IGROUPR0 읽기 → 0x0000FFFF (하위 16비트만)
쓰기 시도 → 무시됨

원인: Non-Secure 상태에서 Secure Group0 비트에 접근 불가
해결: GICD_IGROUPR0을 Group1로 설정하여 Non-Secure 권한 획득
```

### 왜 CNTV는 작동 안 했는가?

```text
Virtual Timer = Physical Timer + 가상 offset
CNTVOFF_EL2 미설정 → offset 값 불명확
→ 타이머 카운터 계산 실패 → CNTV_CTL_EL0 쓰기 무시

CNTP 사용 → offset 불필요 → 간단하고 안정적
```

### 최종 권장사항

1. **타이머**: CNTP(Physical) 사용 - 간단하고 추천
2. **GIC**: 명시적으로 Group1(Non-Secure) 설정
3. **EL2**: `virtualization=on`으로 활성화
4. **HCR_EL2**: RW=1만 설정 (나머지는 0)

---

## 참고: 레지스터 주소

**GIC Distributor Base**: `0x08000000`

- GICD_CTLR: +0x000
- GICD_IGROUPR0: +0x080
- GICD_ISENABLER0: +0x100
- GICD_IPRIORITY0: +0x400

**GIC CPU Interface Base**: `0x08010000`

- GICC_CTLR: +0x000
- GICC_PMR: +0x004
- GICC_IAR: +0x00C
- GICC_EOIR: +0x010

---

**이제 타이머 인터럽트가 정상 작동할 것입니다! 🎉
