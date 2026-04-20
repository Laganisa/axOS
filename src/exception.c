#include "../include/types.h"
#include "../include/io.h"
#include "../include/irq.h"

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
    // ESR_EL1에서 예외 원인 확인
    uint64_t esr;
    asm volatile("mrs %0, esr_el1" : "=r"(esr));

    uint32_t ec = (esr >> 26) & 0x3F;
    uint32_t imm = esr & 0xFFFF;

    if (ec == 0x15)
    {
        if (imm == 0)
        {
            *(volatile uint32_t *)(0x09000000) = 'S';
            puts("[Kernel] SVC Exception caught!\n");
        }
        else if (imm == 1)
        {
            *(volatile uint32_t *)(0x09000000) = 'T';
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
