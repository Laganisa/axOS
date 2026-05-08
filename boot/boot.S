.section ".text.boot"
.global _start

_start:
    mrs x0, CurrentEL
    cmp x0, #0xC            // EL3
    beq el3_entry
    cmp x0, #0x8            // EL2
    beq el2_entry
    b el1_entry

// EL3 to EL2
el3_entry:
    // 비 보안 세상으로 전환
    mrs x0, SCR_EL3
    orr x0, x0, #(1 << 0)       // NS = 1
    orr x0, x0, #(1 << 8)
    orr x0, x0, #(1 << 10)
    msr SCR_EL3, x0

    // EL2 진입 시 상태
    mov x0, #0x3C9              // EL2h, IRQ/FIQ/SError mask
    msr SPSR_EL3, x0

    adr x0, el2_entry
    msr ELR_EL3, x0

    eret



// EL2 SETUP (중요: QEMU virt, secure=off 용)

el2_entry:

    //  CNTHCTL_EL2 (EL1의 Physical/Virtual Timer access 허용)
    //  bit[0]=EL1PCTEN: EL1 Physical Counter EN
    //  bit[1]=EL1PTEN:  EL1 Physical Timer EN  
    //  bit[8]=EVNTEN:   Event Timer EN
    //  bit[9]=EVNTDIR:  Event Timer Direction

    mov x0, #0x303              // 0b001100000011
    msr CNTHCTL_EL2, x0

    //  CNTVOFF_EL2 (Virtual Timer Offset = 0, 즉 Physical Timer와 동일하게)
    mov x0, #0
    msr CNTVOFF_EL2, x0

    //  HCR_EL2 (Critical: 64-bit EL1, VM off, interrupts routed to EL1)
    //  bit[0]=VM:    Virtual Machine (off, bare-metal)
    //  bit[31]=RW:   EL1 is 64-bit
    //  bit[3]=AMO:   Allow AMON to EL1 (optional)
    //  bit[5]=TSC:   Trap SMC (off for Non-Secure)

    mov x0, #(1 << 31)

    // orr x0, x0, #(1 << 4)
    // orr x0, x0, #(1 << 3)

    msr HCR_EL2, x0

    //  EL1 진입 상태: EL1h mode, IRQ/FIQ/SError masked, DAIF=1111

    mov x0, #0x3C5              // EL1h
    msr SPSR_EL2, x0

    adr x0, el1_entry
    msr ELR_EL2, x0

    eret



// EL1 KERNEL ENTRY
el1_entry:
    // 스택 설정
    ldr x0, =0x40080000
    mov sp, x0

    // 인터럽트 enable (CPU level)
    msr DAIFClr, #0xf

    // VBAR 초기화 (중요)
    adr x0, vector_table
    msr VBAR_EL1, x0

    isb
    
    ldr x0, =bss_start
    ldr x1, =bss_end
    mov x2, #0

clear_bss:
    cmp x0, x1
    b.ge clear_bss_done
    str x2, [x0], #8
    b clear_bss

clear_bss_done:
    bl main

hang:
    b hang