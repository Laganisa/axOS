#include "../include/defs.h"
#include "../include/types.h"
#include "../include/io.h"
#include "../include/exception.h"
#include "../include/pm.h"
#include "../include/mm.h"

#include "../include/irq.h"

extern void vector_table(void);

// extern volatile uint8_t resched_flag;

pcb_t *UNIQUE_KERNEL_CURRENT_PROC = 0;

pcb_t **get_current_proc_addr()
{
    return &UNIQUE_KERNEL_CURRENT_PROC;
}

uint64_t irq_handler_main(pcb_t *proc, uint64_t current_sp)
{

    // irq_handler_main 내부
    /*
    puts("Actual Symbol Address in C: ");
    put_hex((uint64_t)&UNIQUE_KERNEL_CURRENT_PROC); // 변수 자체가 위치한 주소 (6F18인지 확인)
    puts("\n");
    puts("\nValue in Symbol: ");
    put_hex((uint64_t)UNIQUE_KERNEL_CURRENT_PROC); // 변수가 들고 있는 PCB 주소 (76F8인지 확인)
    puts("\n");
    // proc = current_proc;
    // --- 1구역: 진입 즉시 인자값 검문 ---
    puts("\n[CHECK 1] Entry Param Check\n");
    puts("Param 'proc' Addr: ");
    put_hex((uint64_t)proc);
    puts("\n");
    if (proc != 0)
    {
        puts("Param 'proc->id': ");
        put_hex((uint64_t)proc->id);
        puts("\n");
    }
    else
    {
        puts("!!! WARNING: Param 'proc' is NULL !!!\n");
    }
    */
    // --- 2구역: 전역 변수 상태 확인 ---
    /*
    puts("[CHECK 2] Global Variable Check\n");
    puts("Global 'current_proc' Addr: ");
    put_hex((uint64_t)current_proc);
    puts("\n");


    if (current_proc != 0)
    {
        puts("Global 'id': ");
        put_hex((uint64_t)current_proc->id);
        puts("\n");
    }
    */

    uint32_t iar = *(volatile uint32_t *)(GIC_CPU_BASE + 0x0C);
    uint32_t irq_nr = iar & 0x3FF;

    if (irq_nr == 30)
    {
        asm volatile("msr cntp_tval_el0, %0" : : "r"(0x1000000));

        /*
        // --- 3구역: 큐에 넣기 직전 최종 확인 ---
        puts("[CHECK 3] Before pm_awake\n");
        if (proc == 0 || proc->id == 0)
        {
            puts("!!! STOP: Trying to awake ID 0 !!!\n");
            // 범인 검거 시점에서 멈추고 싶다면 여기서 무한루프 돌려도 돼
            // while(1);
        }
        */

        pm_awake(&pm_object, 0, proc);

        /*
        puts("[CHECK 4] After awake, Lownum: ");
        put_hex((uint64_t)pm_object.lownum);
        puts("\n");
        */

        UNIQUE_KERNEL_CURRENT_PROC = pm_run(&pm_object);

        // --- 4구역: 다음 프로세스 선택 결과 ---
        /*
        puts("[CHECK 5] Next Proc Selected\n");
        puts("Next ID: ");
        if (current_proc != 0)
            put_hex((uint64_t)current_proc->id);
        else
            puts("NULL");
        puts("\n------------------------\n");
        */
    }

    *(volatile uint32_t *)(GIC_CPU_BASE + 0x10) = iar;

    // Safety check: prevent NULL pointer dereference
    if (UNIQUE_KERNEL_CURRENT_PROC == NULL)
    {
        return current_sp;
    }

    return UNIQUE_KERNEL_CURRENT_PROC->sp;
}

// 이거 왜 있음?
void handle_timer_tick()
{
    // 11시간 사투의 결과물 "A"가 여기서도 찍히는지 확인해보자!
    puts("!\n");

    // 여기서 나중에 스케줄러를 호출해서 current_pcb_addr를 task_B로 바꾸면 끝!
}

void init_vectors()
{
    // VBAR_EL1 레지스터에 테이블의 시작 주소를 박아넣음
    asm volatile("msr vbar_el1, %0" : : "r"(vector_table));

    // ISB 추가 (명령어 동기화)
    asm volatile("isb");

    uint64_t check;
    asm volatile("mrs %0, vbar_el1" : "=r"(check));

    /*
    puts("[IRQ] Vector table registered at: ");
    put_hex((uint64_t)vector_table);
    puts(" (check: ");
    put_hex(check);
    puts(")\n");
    */
}

void init_timer()
{
    uint32_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));

    // ✅ CNTP(Physical Timer) 사용 - CNTV보다 간단하고 EL2 offset 걱정 없음
    // CNTP는 CNTHCTL_EL2[0]=1로만 되면 작동함
    asm volatile("msr cntp_tval_el0, %0" : : "r"(0x1000000)); // 0.1초
    asm volatile("msr cntp_ctl_el0, %0" : : "r"(1));          // Enable=1, IMASK=0

    puts("[IRQ] Timer initialized (CNTP, freq=");
    put_hex(freq);
    puts(")\n");
}

void init_gic()
{
    puts("[GIC] ===== Starting GIC Init =====\n");

    // 0. Distributor OFF (설정 중에 끔)
    *(volatile uint32_t *)(GIC_DIST_BASE + 0x000) = 0;
    puts("[GIC] Distributor OFF\n");

    // 1️⃣ 모든 PPI 인터럽트를 Group1(Non-Secure)로 설정
    //    GICD_IGROUPR0: bits 16-31 (PPI 16-31)
    /* *(volatile uint32_t *)(GIC_DIST_BASE + 0x080) = 0xFFFF0000; // PPI를 모두 Group1로
    puts("[GIC] IGROUPR0 set for all PPI\n");
    */
    *(volatile uint32_t *)(GIC_DIST_BASE + 0x080) = 0x00000000;

    // 2️⃣ 모든 PPI 인터럽트의 우선순위 설정
    //    각 인터럽트당 1 바이트 (4비트 유효)
    for (int i = 16; i < 32; i++)
    {
        *(volatile uint8_t *)((uintptr_t)GIC_DIST_BASE + 0x400 + i) = 0x80;
    }
    puts("[GIC] Priority set for all PPI\n");

    // 3️⃣ 모든 PPI 인터럽트 enable (ISENABLER0)
    //    비트 16-31 = PPI 16-31
    *(volatile uint32_t *)(GIC_DIST_BASE + 0x100) = 0xFFFF0000; // PPI를 모두 활성화
    uint32_t check = *(volatile uint32_t *)(GIC_DIST_BASE + 0x100);
    puts("[GIC] ISENABLER0 set to: ");
    put_hex(check);
    puts("\n");

    // 1. disable 먼저 (안전)
    *(volatile uint32_t *)(GIC_DIST_BASE + 0x000) = 0;

    // 2. priority 설정 (중요)
    *(volatile uint32_t *)(GIC_DIST_BASE + 0x400 + 30) = 0xA0;

    // 3. interrupt enable
    *(volatile uint32_t *)(GIC_DIST_BASE + 0x100) |= (1 << 30);

    // 4. distributor enable
    *(volatile uint32_t *)(GIC_DIST_BASE + 0x000) = 1;

    // 5. CPU interface enable (한 번만!)
    *(volatile uint32_t *)(GIC_CPU_BASE + 0x000) = 1;
    asm volatile("dsb sy");
    asm volatile("isb");

    // 6. PMR (마지막)
    *(volatile uint32_t *)(GIC_CPU_BASE + 0x004) = 0xFF;
    asm volatile("dsb sy");

    // 7. IRQ unmask
    asm volatile("msr daifclr, #2");
}

void init_irq()
{
    puts("\n========== IRQ Initialization ==========\n");

    // 1. 벡터 테이블 등록 (CPU에게 어디로 튈지 알려줌)
    init_vectors();

    // VBAR 확인
    uint64_t vbar;
    asm volatile("mrs %0, vbar_el1" : "=r"(vbar));
    puts("[IRQ] VBAR_EL1: ");
    put_hex(vbar);
    puts("\n");

    // 2. GIC 초기화 (하드웨어 인터럽트 관리자 설정)
    init_gic();

    // 3. 타이머 설정 (주기적으로 인터럽트를 발생시킴)
    init_timer();

    // 4. [중요] CPU의 인터럽트 마스킹 해제
    // 아무리 위에서 설정을 잘해도 CPU가 귀를 막고 있으면 안 들려.
    // DAIF 레지스터의 I비트를 0으로 내려서 인터럽트를 허용해줘야 해.
    asm volatile("msr daifclr, #2");
    puts("[IRQ] DAIF IRQ unmasked\n");

    puts("========== IRQ Init Complete ==========\n\n");

    // 🔥 테스트: 소프트웨어에서 강제로 인터럽트 발생 (타이머 대신)
    puts("[TEST] Forcing software interrupt...\n");
    asm volatile("msr daifset, #2"); // 잠시 IRQ 마스크

    // SGI (Software Generated Interrupt) 발생
    *(volatile uint32_t *)(GIC_DIST_BASE + 0xF00) = 0x8000 | 0; // GICD_SGIR

    asm volatile("msr daifclr, #2"); // IRQ 언마스크
    puts("[TEST] After forced interrupt\n");
}