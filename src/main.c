/*
    ! 프로세스 관리자, 파일 관리자 만들기
    ! 패딩을 생각해서 구조체에서 순서 바꾸기
*/

// 타입 헤더
#include "../include/types.h"

// 분리 파일
#include "../include/asm.h"       // 어셈블리 함수가 있는 헤더
#include "../include/defs.h"      // 정의 헤더
#include "../include/io.h"        // 입출력 헤더
#include "../include/irq.h"       // 인터럽트 헤더 추가
#include "../include/exception.h" // Exception handlers

#include "../include/mm.h" // 메모리 관리자가 있는 헤더
#include "../include/pm.h" // 프로세스 관리자 헤더

extern void _proc(uint64_t *reg_val);
extern void vector_table(void);
// extern void irq_handler_main(void);

volatile uint8_t resched_flag = 0;
int current_task_id = 0; // 반드시 함수 밖(Global)에 있어야 함

// task 함수들
void task_A()
{
    /*
    // 100ms 대기 (타이머가 pending 상태가 되도록)
    for (volatile long wait = 0; wait < 10000000; wait++)
        ;

    // CNTP_CTL 레지스터 읽기 (bit[2]=ISTATUS: 타이머 인터럽트 대기 여부)
    uint64_t cntp_ctl;
    asm volatile("mrs %0, cntp_ctl_el0" : "=r"(cntp_ctl));

    // GIC ISPENDR0 읽기 (bit[30]=CNTP IRQ 대기 여부)
    uint32_t gicd_ispendr0 = *(volatile uint32_t *)(0x08000200);

    // 타이머 상태 출력
    puts("\n[CNTP_CTL.ISTATUS] ");
    putchar(((cntp_ctl >> 2) & 1) ? '1' : '0');
    puts("\n[GIC.ISPENDR[30]] ");
    putchar(((gicd_ispendr0 >> 30) & 1) ? '1' : '0');
    puts("\n");

    // 인터럽트 활성화 (I-bit 언마스크)
    asm volatile("msr daifclr, #2");
    */
    // Task A: 주기적으로 'A' 출력

    puts("[TASK CHECK]");
    put_hex(UNIQUE_KERNEL_CURRENT_PROC->id);

    asm volatile("msr daifclr, #2");

    while (1)
    {
        puts("A");
        for (volatile int i = 0; i < 100000; i++)
            ;
    }
}

void task_B()
{
    puts("[TASK CHECK]");
    put_hex(UNIQUE_KERNEL_CURRENT_PROC->id);

    asm volatile("msr daifclr, #2");

    while (1)
    {
        puts("B");
        for (volatile int i = 0; i < 500000; i++)
            ;
    }
}

// 커널 함수
void main(void)
{
    // 하드웨어 초기화
    uart_init(); // 수정 금지

    // 관리자 초기화
    mm_init(&mm_stack, END_KERNEL_ADDR);

    // ✅ 인터럽트/타이머 초기화
    init_irq();

    puts("myOS kernel\n");                 // 부팅 메시지
    puts("'help' : list commands\n");      // 사용 가능한 명령어 확인
    puts("'end'  : exit\n");               // 시스템 나가기
    puts("Welcome! Have a great time.\n"); // 환영 메시지

    pcb_t *proc1 = creat_proc(&pm_object, &task_A, 0);
    current_proc = proc1;
    pcb_t *proc2 = creat_proc(&pm_object, &task_B, 0);
    // 주소와 값을 직접 대조해서 메모리 맵을 확인하자
    /*
    puts("[DEBUG] proc created");
    put_hex((uint64_t)proc2);
    put_hex((uint64_t)&proc2->id);
    put_hex((uint64_t)proc2->id);

    puts("proc1 id");
    put_hex(proc1->id);
    puts("proc2 id");
    put_hex(proc2->id);
    */

    // proc1 확인
    /*
    puts("Proc1 ID: ");
    put_hex((uint64_t)proc1->id);
    puts("\n");
    puts("Proc1 Addr: ");
    put_hex((uint64_t)proc1);
    puts("\n");
    puts("Proc1 SP Addr: ");
    put_hex((uint64_t)&proc1->sp);
    puts("\n");

    // proc2 확인
    puts("Proc2 ID: ");
    put_hex((uint64_t)proc2->id);
    puts("\n");
    puts("Proc2 Addr: ");
    put_hex((uint64_t)proc2);
    puts("\n");

    // pm_object(큐)와의 거리 확인
    puts("PM_Obj Addr: ");
    put_hex((uint64_t)&pm_object);
    puts("\n");
    */

    pm_awake(&pm_object, 0, proc2);
    // ? pm_awake(&pm_object, 0, proc1);

    _proc((uint64_t *)proc1->sp);

    // ? _proc((uint64_t *)proc2->sp);
    /*
    puts("\n--- Debug: PCB Context Check ---\n");
    puts("1. PCB Register Start Addr: ");
    put_hex(val); // 실제 레지스터 저장 시작 주소
    puts("\n");

    uint64_t *ptr = (uint64_t *)val;

    puts("2. Target PC (task_A): ");
    put_hex(ptr[32]); // reg_val[32] 위치의 값
    puts("\n");

    puts("3. Target SP: ");
    put_hex(ptr[31]); // reg_val[31] 위치의 값
    puts("\n");

    puts("4. SPSR_EL1 Value: ");
    // 만약 SPSR을 메모리에 안 넣었다면 확인 불가, 넣었다면 인덱스에 맞춰 출력
    // ptr[인덱스] ...

    puts("--- Ready to Jump ---\n");

    // =====================================================
    // 🔥 HARDWARE TIMER DIAGNOSTIC (main에서 직접 실행)
    // =====================================================
    puts("\n[DIAGNOSTIC] CNTP/GIC/IRQ Gate Check\n");

    uint64_t cntp_ctl, cntp_tval;
    asm volatile("mrs %0, cntp_ctl_el0" : "=r"(cntp_ctl));
    asm volatile("mrs %0, cntp_tval_el0" : "=r"(cntp_tval));

    puts("[CNTP] CTL=");
    put_hex(cntp_ctl);
    puts(" bit[0]=");
    putchar((cntp_ctl & 1) ? '1' : '0');
    puts(" bit[2]=");
    putchar((cntp_ctl & 4) ? '1' : '0');
    puts("\n");

    uint32_t gicd_igroupr0 = *(volatile uint32_t *)(0x08000080);
    uint32_t gicd_isenabler0 = *(volatile uint32_t *)(0x08000100);
    uint32_t gicd_ispendr0 = *(volatile uint32_t *)(0x08000200);

    puts("[GIC] IGROUPR0[30]=");
    putchar(((gicd_igroupr0 >> 30) & 1) ? '1' : '0');
    puts(" ISENABLER0[30]=");
    putchar(((gicd_isenabler0 >> 30) & 1) ? '1' : '0');
    puts(" ISPENDR0[30]=");
    putchar(((gicd_ispendr0 >> 30) & 1) ? '1' : '0');
    puts("\n");

    uint64_t daif;
    asm volatile("mrs %0, daif" : "=r"(daif));

    puts("[CPU] DAIF I-bit=");
    putchar(((daif >> 7) & 1) ? '1' : '0');
    puts(" (1=masked, 0=unmasked)\n");

    puts("[DIAGNOSTIC] End.\n\n");

    // ✅ CRITICAL: current_pcb_addr 설정 (task context 주소)
    // ? current_pcb_addr = (void *)val;
    */

    /*
    if (resched_flag)
    {
        resched_flag = 0;
        current_proc = pm_run(&pm_object);
        _proc((uint64_t *)current_proc->sp);
    }
    */
    while (1)
    {
        int8_t cmd[64];
        puts("root@localhost : ");
        gets(cmd, 64);

        if (strcmp(cmd, "end") == 0)
        {
            // 시스템 종료
            break;
        }
        else if (strcmp(cmd, "help") == 0)
        {
            // 어떤 쉘 명령이 있는지
            knowcmd();
        }
        else
        {
            // 쉘 명령 실행
            shell_run(cmd);
        }
    }

    puts("Goodbye, see you next time."); // 종료 메시지
}