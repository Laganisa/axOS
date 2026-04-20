// void main에서 void kernel로 전환해야 할듯
// main을 발사대로만

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
    // puts("[TASK CHECK]");
    // put_hex(UNIQUE_KERNEL_CURRENT_PROC->id);

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
    // puts("[TASK CHECK]");
    // put_hex(UNIQUE_KERNEL_CURRENT_PROC->id);

    asm volatile("msr daifclr, #2");

    while (1)
    {
        puts("B");
        for (volatile int i = 0; i < 500000; i++)
            ;
    }
}

/*
    커널 로직
    입력 받는 동안에는 인터럽트를 끈다
*/
void kernel(void)
{

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

// 커널 함수
void main(void)
{
    // 하드웨어 초기화
    uart_init(); // 수정 금지

    // 관리자 초기화
    mm_init(&mm_stack, END_KERNEL_ADDR);

    // 인터럽트/타이머 초기화
    init_irq();

    puts("myOS kernel\n");                 // 부팅 메시지
    puts("'help' : list commands\n");      // 사용 가능한 명령어 확인
    puts("'end'  : exit\n");               // 시스템 나가기
    puts("Welcome! Have a great time.\n"); // 환영 메시지

    pcb_t *proc1 = creat_proc(&pm_object, &task_A, 0);
    current_proc = proc1;
    pcb_t *proc2 = creat_proc(&pm_object, &task_B, 0);

    pm_awake(&pm_object, 0, proc2);
    // ? pm_awake(&pm_object, 0, proc1);

    _proc((uint64_t *)proc1->sp);
}