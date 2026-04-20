#include "../include/types.h"

// 드라이버 구조체
typedef struct
{
    char *name;              // 드라이버 이름
    int (*init)(void);       // 초기화 함수 포인터
    int (*read)(void *buf);  // 데이터 읽기 함수 포인터
    int (*write)(void *buf); // 데이터 쓰기 함수 포인터
    void (*handler)(void);   // 인터럽트 발생 시 처리 로직
} Driver;