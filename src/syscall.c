#include "../include/types.h"
#include "../include/io.h"
#include "../include/syscall.h"
#include "../include/pm.h"

extern pcb_t *current_proc;
extern pcb_t **get_current_proc_addr(void);

// Simple syscall handler
void handle_syscall(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
    switch (syscall_num)
    {
    case SYS_WRITE:
        // arg1: fd (0=stdin, 1=stdout, 2=stderr)
        // arg2: buffer pointer
        // arg3: length
        if (arg1 == 1 && arg2)
        {
            for (uint64_t i = 0; i < arg3; i++)
            {
                putc(((char *)arg2)[i]);
            }
        }
        break;

    case SYS_READ:
        // TODO: implement read
        break;

    case SYS_EXIT:
        // arg1: exit code
        if (pm_object.lownum <= 1)
        {
            pcb_t **current_proc_ptr = (pcb_t **)get_current_proc_addr();
            pcb_t *current = *current_proc_ptr;
            pm_awake(&pm_object, 1, current);
            pcb_t *next = pm_run(&pm_object);
            current_proc = next;
            new_context(next->sp);
        }
        break;

    default:
        puts("[Kernel] Unknown syscall: ");
        put_hex(syscall_num);
        puts("\n");
        break;
    }
}
