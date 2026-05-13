#include "../include/types.h"
#include "../include/io.h"
#include "../include/irq.h"
#include "../include/exce.h"
#include "../include/syscall.h"

// EL1, SP0 예외 정상일땐 작동 안함
void curr_el_sp0_sync()
{
    while (1)
        ;
}

void curr_el_sp0_irq()
{
    while (1)
        ;
}

void curr_el_sp0_fiq()
{
    while (1)
        ;
}

void curr_el_sp0_serror()
{
    while (1)
        ;
}

// EL1, SPx 예외 정상 작동중
void curr_el_spx_sync()
{
    uint64_t esr;
    asm volatile("mrs %0, esr_el1" : "=r"(esr));

    uint32_t ec = (esr >> 26) & 0x3F; // Exception Class
    uint32_t imm = esr & 0xFFFF;      // Immediate value (svc #N)

    if (ec == 0x15) // SVC instruction
    {
        // x8 레지스터에서 시스템 콜 번호를 읽어옴
        uint64_t syscall_num;
        asm volatile("mov %0, x8" : "=r"(syscall_num));

        if (syscall_num == 93) // EXIT 시스템 콜
        {
            // puts("[Kernel] Task requested EXIT (via x8=93)\n");

            if (pm_object.lownum <= 1)
            {
                // 현재 프로세스를 종료 리스트로 보냄
                pcb_t **current_proc_ptr = (pcb_t **)get_current_proc_addr();
                pcb_t *current = *current_proc_ptr;

                /*
                puts("\n[EXIT] Current PCB Addr: ");
                put_hex((uint64_t)current); // PCB 구조체 자체의 주소
                puts(" | ID before: ");
                put_hex(current->id);
                */

                // pm_awake(&pm_object, 1, *(pcb_t **)get_current_proc_addr());
                pm_awake(&pm_object, 1, current);

                // 세로운 프로세스 찾고 넘기기
                pcb_t *next = pm_run(&pm_object);
                current_proc = next;

                /*
                puts("\n[EXIT] Next PCB Addr: ");
                put_hex((uint64_t)next); // 다음 실행될 놈의 주소
                puts(" | Next ID: ");
                put_hex(next->id);

                puts("\n[EXIT] Check Current ID after awake: ");
                put_hex(current->id);
                */

                new_context(next->sp);
            }
            else
            {
                // 넘길 프로세스가 부족하면
                // pid = 0 로 넘기기
            }
        }
        else if (imm == 1) // 테스트용으로 쓴 svc #1
        {
            puts("[Kernel] Tick!\n");
        }
    }
}

// extern pcb_t *current_proc;

void curr_el_spx_irq()
{

    uint32_t iar = *(volatile uint32_t *)(GIC_CPU_BASE + 0x0C);
    uint32_t irq_nr = iar & 0x3FF;

    if (irq_nr == 30)
    {
        // 타이머만 재설정
        asm volatile("msr cntp_tval_el0, %0" : : "r"(0x1000000));
        // 부르더라도 '스케줄링' 로직이 없는 별도의 핸들러를 써야 함.
    }

    *(volatile uint32_t *)(GIC_CPU_BASE + 0x10) = iar;
    // 그대로 복귀 (iret/ret)
}

void curr_el_spx_fiq()
{
    while (1)
        ;
}

void curr_el_spx_serror()
{
    while (1)
        ;
}

// 하위 EL (AArch64)예외
void lower_el_aarch64_sync()
{
    while (1)
        ;
}

void lower_el_aarch64_fiq()
{
    while (1)
        ;
}

void lower_el_aarch64_serror()
{
    while (1)
        ;
}

// 하위 EL (AArch32) 예외
void lower_el_aarch32_sync()
{
    while (1)
        ;
}

void lower_el_aarch32_irq()
{
    while (1)
        ;
}

void lower_el_aarch32_fiq()
{
    while (1)
        ;
}

void lower_el_aarch32_serror()
{
    while (1)
        ;
}
