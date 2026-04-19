.section ".text.proc"
.global _proc

_proc:
    // 코드
    // 1. PC (reg_val[32]) -> ELR_EL1 (Exception Link Register)
    // CPU가 eret을 실행하는 순간 이 주소로 점프해.
    ldr x1, [x0, #(32 * 8)]
    msr elr_el1, x1

    // 2. SP (reg_val[31]) -> SP_EL0
    // 프로세스가 사용할 전용 스택 포인터를 설정해줘.
    ldr x1, [x0, #(31 * 8)]
    msr sp_el0, x1

    // 3. SPSR_EL1 (Processor State) 설정
    // 프로세스가 어떤 모드에서 실행될지 결정해. 
    // 0x5는 EL1h(커널모드) 유지야. 나중에 유저모드로 가려면 값을 바꿔야 해!
    mov x1, #0x005
    msr spsr_el1, x1

    // 4. 일반 목적 레지스터 복원 (x1 ~ x30)
    // ldp 명령어로 16바이트씩(8바이트 2개) 짝지어서 슝슝 읽어와.
    ldp x1, x2, [x0, #(1 * 8)]
    ldp x3, x4, [x0, #(3 * 8)]
    ldp x5, x6, [x0, #(5 * 8)]
    ldp x7, x8, [x0, #(7 * 8)]
    ldp x9, x10, [x0, #(9 * 8)]
    ldp x11, x12, [x0, #(11 * 8)]
    ldp x13, x14, [x0, #(13 * 8)]
    ldp x15, x16, [x0, #(15 * 8)]
    ldp x17, x18, [x0, #(17 * 8)]
    ldp x19, x20, [x0, #(19 * 8)]
    ldp x21, x22, [x0, #(21 * 8)]
    ldp x23, x24, [x0, #(23 * 8)]
    ldp x25, x26, [x0, #(25 * 8)]
    ldp x27, x28, [x0, #(27 * 8)]
    ldp x29, x30, [x0, #(29 * 8)]

    // 5. 마지막으로 x0 복원
    // x0를 가장 나중에 하는 이유는, x0가 지금 이 데이터들의 주소를 들고 있기 때문이야!
    ldr x0, [x0]
    
    msr daifclr, #2
    // 6. 대망의 점프!
    // 이 명령어를 실행하면 CPU가 ELR_EL1에 적힌 주소(task1)로 순간이동해!
    eret