#ifndef __SECT_H__
#define __SECT_H__

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

// 커널의 메모리 정하기
#define KERNEL_TEXT_START 0x40000000

#define KERNEL_TEXT_SIZE 0x19000 // 100KB정도

#define KERNEL_TEXT_END (KERNEL_TEXT_START + KERNEL_BIN_SIZE)

#define KERNEL_DATA_START KERNEL_BIN_END

#define KERNEl_DATA_END

#define KERNE_BSS_START KERNEl_DATA_END
#define KERNE_BSS_END

#define KERNE_HEAP_START KERNE_BSS_END
#define KERNE_STACTK_START

#define USER_PROC_START KERNE_STACTK_START
#define USER_PROC_END

#define USER_FILE_START USER_PROC_END
#define USRT_FILE_END

#endif