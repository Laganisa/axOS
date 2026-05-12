#include "../include/types.h"

#ifndef __SYSCALL_H__
#define __SYSCALL_H__

// Syscall numbers
#define SYS_WRITE 1
#define SYS_READ 3
#define SYS_EXIT 93

// Syscall handler
void handle_syscall(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3);

#endif