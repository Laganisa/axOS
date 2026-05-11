// void main에서 void kernel로 전환해야 할듯
// main을 발사대로만

// 타입 헤더
#include "../include/types.h"

// 분리 파일
#include "../include/asm.h"  // 어셈블리 함수가 있는 헤더
#include "../include/defs.h" // 정의 헤더
#include "../include/sect.h" // 메모리 매핑 헤더

#include "../include/io.h"   // 입출력 헤더
#include "../include/irq.h"  // 인터럽트 헤더 추가
#include "../include/exce.h" // Exception handlers

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
    // put_hex(current_proc->id);

    asm volatile("msr daifclr, #2");

    while (1)
    {
        puts("A");
        for (volatile int i = 0; i < 100000; i++)
            ;
    }
}

void task_B1()
{
    // puts("[TASK CHECK]");
    // put_hex(current_proc->id);

    asm volatile("msr daifclr, #2");

    while (1)
    {
        puts("B");
        for (volatile int i = 0; i < 1000000; i++)
            ;
    }
}

// 종료 실험
void task_B()
{
    // puts("[TASK CHECK]");
    // put_hex(current_proc->id);

    asm volatile("msr daifclr, #2");
    int i = 10;
    while (i > 0)
    {
        puts("B");
        i--;
    }

    asm volatile(
        "mov x8, #93\n"
        "svc #0\n" ::: "x8" // x8 레지스터를 사용한다고 컴파일러에게 알림
    );

    while (1)
    {
        puts("Error: Task B should be dead!\n");
    }
}

/*
    pid 0 : 루트 프로세스
    pid 1 의 부모
*/
void ROOT(void)
{
    // 루트 프로세스
}

/*
    pid 1 :init 프로세스
    데몬들의 부모이자 모든 프로세스의 부모
*/
void INIT(void)
{
    asm volatile("msr daifclr, #2");

    while (1)
    {
        int8_t cmd[64];
        puts("root@localhost : ");
        remo_get(cmd, 64);

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

/*
    pid 2 : 네트워크 프로세스
    첫 번째 데몬

*/
void NET(void)
{
    // 네트워크 관리자
}

/*
    pid 3 : 디바이스 관리자
    2번째 데몬
*/
void DEV(void)
{
    // 디바이스 관리자
}

// 커널 함수
void main(void)
{
    // 하드웨어 초기화
    uart_init();
    // 관리자 초기화
    mm_init(&mm_stack, MM_ADDR_START);
    // 인터럽트/타이머 초기화
    init_irq();

    put_hex(sizeof(PMv1_object));
    puts("myOS kernel\n");                 // 부팅 메시지
    puts("'help' : list commands\n");      // 사용 가능한 명령어 확인
    puts("'end'  : exit\n");               // 시스템 나가기
    puts("Welcome! Have a great time.\n"); // 환영 메시지

    pcb_t *proc1 = creat_proc(&pm_object, &task_A, 0);
    pcb_t *proc2 = creat_proc(&pm_object, &task_B, 0);
    current_proc = proc1;

    put_hex(proc1->id);
    put_hex(proc2->id);

    pm_awake(&pm_object, 0, proc2);
    // ? pm_awake(&pm_object, 0, proc1);

    _proc((uint64_t *)proc1->sp);
}