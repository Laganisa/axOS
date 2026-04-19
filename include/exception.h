#include "../include/pm.h"

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

// Exception handler declarations
void curr_el_sp0_sync(void);
void curr_el_sp0_irq(void);
void curr_el_sp0_fiq(void);
void curr_el_sp0_serror(void);

void curr_el_spx_sync(void);
void curr_el_spx_irq(void);
void curr_el_spx_fiq(void);
void curr_el_spx_serror(void);

void lower_el_aarch64_sync(void);
void lower_el_aarch64_fiq(void);
void lower_el_aarch64_serror(void);

void lower_el_aarch32_sync(void);
void lower_el_aarch32_irq(void);
void lower_el_aarch32_fiq(void);
void lower_el_aarch32_serror(void);
// 예: src/process.c 혹은 main.c

extern pcb_t *UNIQUE_KERNEL_CURRENT_PROC;
extern pcb_t **get_current_proc_addr(void);

#define current_proc UNIQUE_KERNEL_CURRENT_PROC

#endif // __EXCEPTION_H__
