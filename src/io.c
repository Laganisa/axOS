#include "../include/types.h"
#include "../include/defs.h"

// UART 초기화
void uart_init(void)
{
    // QEMU virt는 이미 UART 활성화됨
    // 아무것도 안 해도 됨
}

void putchar(int8_t c)
{
    // UART_FR 5번 비트: TXFF (보낼 데이터 칸이 꽉 찼는지 확인)
    while (*UART0_FR & (1 << 5))
    {
    }

    *UART0_DR = c;
}

// 문자열 출력
void puts(const int8_t *s)
{
    while (*s)
    {
        putchar(*s++);
    }
}

// 문자 입력 (UART에서 수신 대기)
int8_t getchar(void)
{
    // UART_FR 레지스터 4번 비트: RX FIFO empty
    // 데이터가 올 때까지 대기
    while (*UART0_FR & (1 << 4))
    {
        // UART가 데이터를 기다리는 중
    }
    return *UART0_DR;
}

// 문자열 입력 (Enter까지 받음)
void gets(int8_t *s, int32_t max_len)
{
    int32_t i = 0;
    while (i < max_len - 1)
    {
        int8_t c = getchar();
        putchar(c); // 입력한 문자를 다시 출력 (echo)

        if (c == '\r' || c == '\n')
        {
            putchar('\n');
            break;
        }
        else if (c == '\b' && i > 0)
        {
            // Backspace 처리
            s[--i] = '\0';
            puts("\b \b"); // 스크린에서 문자 제거
        }
        else if (c >= 32 && c < 127)
        {
            // 출력 가능한 문자
            s[i++] = c;
        }
    }
    s[i] = '\0';
}

// PCB-related global variable
void *current_pcb_addr = 0;

// Clear screen using ANSI escape codes
void clear()
{
    // \033[2J : 화면 전체 삭제
    // \033[H  : 커서를 홈(0,0)으로 이동
    puts("\033[2J\033[H");
}

// Convert and print unsigned 64-bit integer as decimal
void put_uint(uint64_t n)
{
    int8_t buf[21];
    int32_t i = 19;
    buf[20] = '\0';

    if (n == 0)
        buf[i--] = '0';

    while (n > 0 && i >= 0)
    {
        buf[i--] = (n % 10) + '0';
        n /= 10;
    }

    puts(&buf[i + 1]);
}

// String comparison (returns 0 if equal)
int32_t strcmp(const int8_t *s1, const int8_t *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(uint8_t *)s1 - *(uint8_t *)s2;
}

// Print hex value with 0x prefix
void put_hex(uint64_t d)
{
    char *hex = "0123456789ABCDEF";
    puts("0x");
    for (int i = 60; i >= 0; i -= 4)
    {
        putchar(hex[(d >> i) & 0xF]);
    }
}

// List available commands
void knowcmd(void)
{
    // 어떤 함수가 있는지 알려주는 함수
}

// Execute shell command
void shell_run(int8_t *cmd)
{
    // 예시: 개행 문자 제거
    for (int32_t i = 0; i < 64; i++)
    {
        if (cmd[i] == '\n' || cmd[i] == '\r')
        {
            cmd[i] = '\0';
            break;
        }
    }

    // 명령이 나오는 지 확인용
    puts("comand");

    if (strcmp(cmd, "clear") == 0)
    {
        clear();
    }

    puts("\n"); // 개행
}
