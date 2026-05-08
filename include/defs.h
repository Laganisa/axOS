#include "../include/sect.h"

#ifndef __DEFS_H__
#define __DEFS_H__

#define END_KERNEL_ADDR 0x40100000 // 커널 끝 주소

// 산수 관련

// 계산 관련
#define BIT_ODD8_t 0x55  // 8비트 홀수 비트 검정
#define BIT_EVEN8_t 0xAA // 8비트 짝수 비트 검정

#define BIT_ODD64_t 0x5555555555555555  // 64비트 홀수 비트 검정
#define BIT_EVEN64_t 0xAAAAAAAAAAAAAAAA // 64비트 짝수 비트 검정

// 프로세스 관련
#define MAX_PCB_SIZE 0x100     // 최대 pcb 수 256개 0은 pid 0 운영체제 꺼
#define MAX_PCB_BITSIZE 0x04   // 최대 pcb bit 수 4개 이유는 1비트당 하나의 프로세서이니까
#define INITIAL_PROC_SIZE 0x80 // 초기 프로세스 할당 크기
#define PROC_SIGNAL 0xFF       // low q를 확인할 차례라는 시그널
#define PMV1_MAX_PROC 0xFF     // 프로세스 최대 수
#define MAX_PTP_MSGBOX 0x10    // 프로세스간 메시지 통신 개수 제한, 이유 : 이러면 딱 1KB

// 파일 & 디렉토리 관련

// 파일 관리자의 크기를 뺀 값 나중에 바꿔야함
#define END_PROC_ADDR 0x5010A400 // 프로세서 끝 주소 여기부터 파일 영역

#define MAX_FCB_SIZE 16644 // 파일 크기

#endif