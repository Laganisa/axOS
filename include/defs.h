#ifndef __DEFS_H__
#define __DEFS_H__

// QEMU virt 머신의 PL011 UART 주소
#define UART0_BASE 0x09000000
#define UART0_DR ((volatile uint32_t *)(UART0_BASE + 0x00))
#define UART0_FR ((volatile uint32_t *)(UART0_BASE + 0x18))
#define UART0_CR ((volatile uint32_t *)(UART0_BASE + 0x30))

// 인터럽트 해석 QEMU virt machine GIC V2 주소
#define GIC_DIST_BASE 0x08000000 // Distributor
#define GIC_CPU_BASE 0x08010000  // CPU Interface

// 인터럽트 제어 레지스터
#define GICC_IAR ((volatile uint32_t *)(GIC_CPU_BASE + 0x0C))
#define GICC_EOI ((volatile uint32_t *)(GIC_CPU_BASE + 0x10))
#define GICC_CTLR ((volatile uint32_t *)(GIC_CPU_BASE + 0x00))
#define GICD_CTLR ((volatile uint32_t *)(GIC_DIST_BASE + 0x00))
#define GICD_ISENABLER ((volatile uint32_t *)(GIC_DIST_BASE + 0x100))

// 인터럽트 확인 레지스터
#define GIC_INTERFACE_IAR (*(volatile uint32_t *)(GIC_CPU_BASE + 0x0C))

// 인터럽트 종료 알림 레지스터
#define GIC_INTERFACE_EOI (*(volatile uint32_t *)(GIC_CPU_BASE + 0x10))

#define END_KERNEL_ADDR 0x40100000 // 임시 커널 끝 주소

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

#endif