#include "../include/types.h"
#include "../include/defs.h"

#ifndef __PM_H__
#define __PM_H__

// pcb 8바이트
// ? 나중에 프로세스가 생성되고 레지스터 공간 따로 할당
typedef struct pcb_t
{
    uint8_t id;         // 1 byte
    uint8_t p_id;       // 1 byte
    uint16_t proc_info; // 2 bytes
    uint16_t mm_addr;   // 2 bytes
    uint16_t padding1;  // 2 bytes (여기에 패딩 추가!) - 여기까지 8바이트

    uint32_t reg;      // 4 bytes
    uint32_t padding2; // 4 bytes (여기에 패딩 추가!) - 여기까지 16바이트

    uint64_t sp; // 8 bytes (24바이트 지점)
    uint64_t pc; // 8 bytes (32바이트 지점)
} __attribute__((aligned(8))) pcb_t;

typedef struct PMv1_object
{
    // 총 공간이 3KB 정도
    uint8_t proc_comocc : 4;              // 그중에서 어떤 proc_occ 이 사용되지 않았는지
    uint8_t proc_comscj : 4;              // 프로세서 pm_rum에 들어가는 관리하는
    uint8_t lownum;                       // low에 들어있는 프로세스 수
    uint8_t highnum;                      // high에 들어있는 프로세스 수
    uint8_t lowhead;                      // 원형큐 머리
    uint8_t lowtail;                      // 원형큐 꼬리
    uint8_t highhead;                     // 원형큐 머리
    uint8_t hightail;                     // 원형큐 꼬리
    pcb_t PMv1_mem[MAX_PCB_SIZE];         // 최대 프로세스 수 만큼 만들기 2KB 정도 pcb의 배열
    uint8_t PMv1_lowqueue[MAX_PCB_SIZE];  // 프로세스 low q
    uint8_t PMv1_highqueue[MAX_PCB_SIZE]; // 프로세스 high q

    uint64_t proc_occ[MAX_PCB_BITSIZE];    // 어떤 프로세스 pid 를 사용하지 않았는지
    uint64_t occ_num;                      // occ 숫자 넣기 레지스터에 넣기 좋도록 64bit를 씀
    uint64_t proc_scj[MAX_PCB_BITSIZE];    // 스케줄러에 들어갈 task들의 우선순위를 계산하기 위한 배열 순환돌때 여기다가 적는다
    uint64_t proc_priscj[MAX_PCB_BITSIZE]; // 스캐줄러에 들어갈 task들의 우선순위가 적힌 배열
} PMv1_object;

// 함수 선언
pcb_t *creat_proc(PMv1_object *obj, void *task, uint8_t parid);
uint8_t pm_qaddr(PMv1_object *queue, uint8_t type, uint8_t cmd, uint8_t val);
pcb_t *pm_run(PMv1_object *obj);
void pm_awake(PMv1_object *obj, uint8_t cmd, pcb_t *proc);

// 전역 구조체 선언
extern PMv1_object pm_object; // 프로세스 관리자의 스택 전역변수 선언 //? 왜 인지는 모름

#endif