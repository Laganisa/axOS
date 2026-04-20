#include "../include/types.h"
#include "../include/defs.h"
#include "../include/io.h"

#include "../include/mm.h"
#include "../include/pm.h"

extern void _proc(uint64_t *reg_val); // proc와 연결

PMv1_object pm_object;

// 프로세스 생성
// ! 나중에 내 입맛 대로 만들기
pcb_t *creat_proc(PMv1_object *obj, void *task, uint8_t parid)
{
    uint64_t target_chunk;
    uint64_t leading_zeros;

    uint64_t search_com = ~(uint64_t)obj->proc_comocc;
    asm volatile("clz %0, %1" : "=r"(leading_zeros) : "r"(search_com << 60));

    obj->occ_num = (uint8_t)leading_zeros;

    // if (obj->occ_num >= 4) 이면 나가게
    // 선택된 64비트 청크 내에서 빈자리(0) 찾기
    target_chunk = ~obj->proc_occ[obj->occ_num]; // 0을 1로 반전
    asm volatile("clz %0, %1" : "=r"(leading_zeros) : "r"(target_chunk));

    // PID 계산 (청크 번호 * 64 + 비트 위치)
    uint8_t bit_pos = 63 - (uint8_t)leading_zeros;
    uint8_t pid = (obj->occ_num << 6) | bit_pos;

    // ! 사용한 비트 1로 채우기 (나중에 occ_num 청크가 다 차면 comocc도 1로)
    obj->proc_occ[obj->occ_num] |= (1ULL << bit_pos);
    if (obj->proc_occ[obj->occ_num] == ~0ULL)
    {
        obj->proc_comocc |= (1 << (3 - obj->occ_num));
    }

    obj->PMv1_mem[pid].id = pid;      // 프로세스의 id를 할당된 pid로 변경
    obj->PMv1_mem[pid].p_id = parid;  // 부모 id를 수정함
    obj->PMv1_mem[pid].proc_info = 0; // 정보를 0으로 수정
    // 128KB를 할당 리턴 된 메모리 스택 주소를 받음
    obj->PMv1_mem[pid].mm_addr = (uint16_t)(uintptr_t)mm_run(&mm_stack, &mm_substack, 0, INITIAL_PROC_SIZE, 0);

    // 할당 후 주소를 줌
    // 자신의 주소를 알아내고
    uint64_t real_addr = (uint64_t)(uintptr_t)mm_run(&mm_stack, &mm_substack, 2, obj->PMv1_mem[pid].mm_addr, 0);
    uint64_t *reg_val = (uint64_t *)(real_addr + (INITIAL_PROC_SIZE << 10) - 256);
    obj->PMv1_mem[pid].reg = (INITIAL_PROC_SIZE << 10) - 256; // 레지스터 위치

    obj->PMv1_mem[pid].pc = (uint64_t)task;
    obj->PMv1_mem[pid].sp = (uint64_t)reg_val;

    for (int i = 0; i < 31; i++)
        reg_val[i] = 0;              // x0~x30 초기화
    reg_val[31] = (uint64_t)reg_val; // SP 초기값
    reg_val[32] = (uint64_t)task;    // PC (ELR_EL1)
    reg_val[33] = 0x3C5;             // SPSR_EL1

    return &obj->PMv1_mem[pid];
}

/* 주소를 주면 변환해서 내주는 코드
    주소 -> 실제 주소
    type = 0 : low q로직
    type = 1 : high q로직
    cmd = 0 : 넣기
    cmd = 1 : 빼기
*/
uint8_t pm_qaddr(PMv1_object *queue, uint8_t type, uint8_t cmd, uint8_t val)
{
    if (type == 0)
    {
        if (cmd == 0)
        {
            queue->PMv1_lowqueue[queue->lowhead] = val;
            queue->lowhead = (queue->lowhead + 1) & 255;
            queue->lownum++;
            return 0;
        }

        if (queue->lownum == 0)
            return 0;

        uint8_t ret = queue->PMv1_lowqueue[queue->lowtail];
        queue->lowtail = (queue->lowtail + 1) & 255;
        queue->lownum--;
        return ret;
    }
    else
    {
        if (cmd == 0)
        {
            queue->PMv1_highqueue[queue->highhead] = val;
            queue->highhead = (queue->highhead + 1) & 255;
            queue->highnum++;
            return 0;
        }

        if (queue->highnum == 0)
            return 0;

        uint8_t ret = queue->PMv1_highqueue[queue->hightail];
        queue->hightail = (queue->hightail + 1) & 255;
        queue->highnum--;
        return ret;
    }
}

#define PMV1_MAX_PROC 255
// 프로세스 실행 함수
// 큐에 들어가 있는 대로 진행함
pcb_t *pm_run(PMv1_object *obj)
{
    uint8_t data;

    if (obj->highnum != 0)
    {
        data = pm_qaddr(obj, 1, 1, 0);

        puts("Next Proc ID: ");
        put_hex(data);
        puts("\n");

        // 안전 체크
        if (data >= PMV1_MAX_PROC)
            return &obj->PMv1_mem[0];

        if (data == 0)
            return &obj->PMv1_mem[0];

        if (data == PROC_SIGNAL)
        {
            // signal 처리
            return &obj->PMv1_mem[0];
        }

        return &obj->PMv1_mem[data];
    }

    else if (obj->lownum != 0)
    {
        data = pm_qaddr(obj, 0, 1, 0);

        puts("Next Proc ID: ");
        put_hex(data);
        puts("\n");

        if (data >= PMV1_MAX_PROC)
            return &obj->PMv1_mem[0];

        if (data == 0)
            return &obj->PMv1_mem[0];

        return &obj->PMv1_mem[data];
    }
    return &obj->PMv1_mem[0];
}
/*
    프로세스 활성, 비활성 함수
    uint8_t cmd = 0 , uint8_t task의 주소 포인터 활성 상태로 전환
    uint8_t cmd = 1 , uintt_t task의 주소 포인터 비활성 상태로 전환
*/
void pm_awake(PMv1_object *obj, uint8_t cmd, pcb_t *proc)
{
    // pm_awake 함수 안에서
    /*
    puts("[AWAKE] Target ID: ");
    put_hex((uint64_t)proc->id);
    puts("\n");
    puts("[AWAKE] Current lownum: ");
    put_hex((uint64_t)obj->lownum);
    puts("\n");
    */

    if (cmd == 0)
    {
        pm_qaddr(&pm_object, 0, 0, proc->id);
    }
}

// 프로세스 종료 함수

// 프로세스 대기 함수

// 추가 함수들