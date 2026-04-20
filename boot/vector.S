.align 11 // 테이블 시작 주소 2048바이트 정렬
.global vector_table
.global lower_el_aarch64_irq

.extern current_task_id

vector_table:
    // 현재 EL에서 SP_EL0 사용(거의 사용하지 않음)
    .align 7
    b curr_el_sp0_sync
    .align 7
    b curr_el_sp0_irq
    .align 7
    b curr_el_sp0_fiq
    .align 7
    b curr_el_sp0_serror

    //현재 EL에서 SP_ELx 사용 (커널 내부 예외)
    .align 7
    b curr_el_spx_sync
    .align 7
    b curr_el_spx_irq   // 커널 실행 중 인터럽트 터지면 이 함수 실행
    .align 7
    b curr_el_spx_fiq
    .align 7
    b curr_el_spx_serror

    //하위 EL에서 AArch64 사용 (태스크가 돌다 터질 때)
    .align 7
    b lower_el_aarch64_sync  // 시스템 콜 (svc)
    .align 7
    b lower_el_aarch64_irq   // 태스크 실행 중 타이머 터지면 이 함수 실행
    .align 7
    b lower_el_aarch64_fiq
    .align 7
    b lower_el_aarch64_serror

    // 하위 EL에서 AArch32 사용 (거의 안 씀)
    .align 7
    b lower_el_aarch32_sync
    .align 7
    b lower_el_aarch32_irq
    .align 7
    b lower_el_aarch32_fiq
    .align 7
    b lower_el_aarch32_serror

// 밑에 부터 주석 바꾸기

lower_el_aarch64_irq:
    // UART에 직접 'L' 출력 (벡터 진입 확인)
    mov x30, #0x09000000
    mov w29, #'L'
    str w29, [x30]
    
    /* 1. 함수 호출을 통해 '진짜' UNIQUE_KERNEL_CURRENT_PROC의 주소 얻기 */
    bl get_current_proc_addr
    /* 호출 결과: x0 = &UNIQUE_KERNEL_CURRENT_PROC (진짜 변수 위치) */
    
    /* 2. x2에 저장 (복원 단계에서도 사용할 예정) */
    mov x2, x0                /* x2 = &UNIQUE_KERNEL_CURRENT_PROC */
    
    /* 3. 현재 PCB 주소 로드 */
    ldr x3, [x2]              /* x3 = 현재 PCB 주소 */
    
    /* 4. PCB가 NULL이면 아직 첫 프로세스가 없으므로 건너뛰기 */
    cbz x3, skip_handler_lower_el
    
    /* 5. 현재 PCB의 레지스터 저장 위치(sp) 로드 */
    ldr x1, [x3, #16]         /* x1 = pcb->sp (레지스터 저장할 주소) */
    
    /* 6. 사용자 모드 레지스터 저장 (Context Save) */
    stp x0, x1, [x1, #0]
    stp x2, x3, [x1, #16]
    stp x4, x5, [x1, #32]
    stp x6, x7, [x1, #48]
    stp x8, x9, [x1, #64]
    stp x10, x11, [x1, #80]
    stp x12, x13, [x1, #96]
    stp x14, x15, [x1, #112]
    stp x16, x17, [x1, #128]
    stp x18, x19, [x1, #144]
    stp x20, x21, [x1, #160]
    stp x22, x23, [x1, #176]
    stp x24, x25, [x1, #192]
    stp x26, x27, [x1, #208]
    stp x28, x29, [x1, #224]
    str x30, [x1, #240]
    
    /* 7. PC(ELR)와 상태(SPSR) 저장 */
    mrs x0, elr_el1
    mrs x1, spsr_el1
    stp x0, x1, [x1, #256]    /* x1은 아직 pcb->sp를 가리킴 */
    
    /* 8. irq_handler_main 호출 준비: x0=pcb, x1=저장된 위치 */
    mov x0, x3                /* x0 = pcb */
    ldr x1, [x3, #16]         /* x1 = pcb->sp (다시 로드) */
    bl irq_handler_main       /* 반환값: x0 = 새로운 sp */

skip_handler_lower_el:
    /* 9. 스케줄러가 선택한 PCB 가져오기 */
    ldr x3, [x2]              /* x3 = 새로운 PCB 주소 */
    
    /* 10. 새로운 PCB의 레지스터 저장 위치 로드 */
    ldr x1, [x3, #16]         /* x1 = 새로운 pcb->sp */
    
    /* 11. 새로운 PCB의 레지스터 복원 (Context Restore) */
    ldp x0, x1, [x1, #256]    /* ELR, SPSR 복원 */
    msr elr_el1, x0
    msr spsr_el1, x1
    
    ldr x1, [x3, #16]         /* 다시 x1에 저장 위치 로드 */
    ldr x30, [x1, #240]
    ldp x28, x29, [x1, #224]
    ldp x26, x27, [x1, #208]
    ldp x24, x25, [x1, #192]
    ldp x22, x23, [x1, #176]
    ldp x20, x21, [x1, #160]
    ldp x18, x19, [x1, #144]
    ldp x16, x17, [x1, #128]
    ldp x14, x15, [x1, #112]
    ldp x12, x13, [x1, #96]
    ldp x10, x11, [x1, #80]
    ldp x8, x9, [x1, #64]
    ldp x6, x7, [x1, #48]
    ldp x4, x5, [x1, #32]
    ldp x2, x3, [x1, #16]
    ldp x0, x1, [x1, #0]
    
    eret

curr_el_spx_irq:
    /* 1. 현재 태스크의 모든 레지스터 저장 (Context Save) */
    // x0~x30, ELR, SPSR까지 다 저장해야 하므로 272바이트가 필요해 (16바이트 정렬)
    sub sp, sp, #272
    stp x0, x1, [sp, #0]
    stp x2, x3, [sp, #16]
    stp x4, x5, [sp, #32]
    stp x6, x7, [sp, #48]
    stp x8, x9, [sp, #64]
    stp x10, x11, [sp, #80]
    stp x12, x13, [sp, #96]
    stp x14, x15, [sp, #112]
    stp x16, x17, [sp, #128]
    stp x18, x19, [sp, #144]
    stp x20, x21, [sp, #160]
    stp x22, x23, [sp, #176]
    stp x24, x25, [sp, #192]
    stp x26, x27, [sp, #208]
    stp x28, x29, [sp, #224]
    str x30, [sp, #240]

    // 인터럽트 복귀를 위한 ELR과 SPSR 저장 (이걸 안 하면 다른 태스크로 못 가)
    mrs x0, elr_el1
    mrs x1, spsr_el1
    stp x0, x1, [sp, #256]

    /* 2. C 핸들러 호출 준비 (인자 세팅) */
    /* 함수를 통해 진짜 UNIQUE_KERNEL_CURRENT_PROC의 주소 얻기 */
    bl get_current_proc_addr
    /* x0 = &UNIQUE_KERNEL_CURRENT_PROC */
    
    /* x2에 저장 (나중에 새로운 PCB 가져올 때 사용) */
    mov x2, x0
    
    /* 현재 PCB 로드 */
    ldr x0, [x2]              /* x0 = 현재 PCB 주소 */
    
    /* 현재의 SP (레지스터 저장된 위치) */
    mov x1, sp
    
    bl irq_handler_main       /* 리턴값(새로운 sp)은 x0에 담겨옴 */

    /* 3. 새로운 태스크의 컨텍스트로 전환 */
    mov sp, x0               /* CPU의 스택 포인터를 새 태스크의 스택으로 갈아끼움! */

    /* 4. 새 태스크의 레지스터 복원 (Context Restore) */
    ldp x0, x1, [sp, #256]   // ELR, SPSR 복원
    msr elr_el1, x0
    msr spsr_el1, x1

    ldr x30, [sp, #240]      // x30 복원
    ldp x28, x29, [sp, #224]
    ldp x26, x27, [sp, #208]
    ldp x24, x25, [sp, #192]
    ldp x22, x23, [sp, #176]
    ldp x20, x21, [sp, #160]
    ldp x18, x19, [sp, #144]
    ldp x16, x17, [sp, #128]
    ldp x14, x15, [sp, #112]
    ldp x12, x13, [sp, #96]
    ldp x10, x11, [sp, #80]
    ldp x8, x9, [sp, #64]
    ldp x6, x7, [sp, #48]
    ldp x4, x5, [sp, #32]
    ldp x2, x3, [sp, #16]
    ldp x0, x1, [sp, #0]

    add sp, sp, #272

    /* 5. 인터럽트 종료 및 복귀 */
    eret