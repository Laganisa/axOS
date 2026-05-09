#include "../include/defs.h"
#include "../include/types.h"
#include "../include/sect.h"

#ifndef __FM_H__
#define __FM_H__

// 파일 객체 블록
// 16B 크기
typedef struct fcb_t
{
    // 1. 파일 이름 (8 bytes)
    int8_t alias[MAX_FILE_NAME];

    uint64_t lens : 5;   // 파일 길이 128바이트가 기준
    uint64_t depth : 2;  // 파일 깊이
    uint64_t is_dir : 1; // 디렉토리 여부

    uint64_t me_auth : 3;  // 나의 권한
    uint64_t you_auth : 3; // 너의 권한

    uint64_t ppdir_addr : 6;
    uint64_t pdir_addr : 6;
    uint64_t me_addr : 6;
    uint64_t last_addr : 6; // 자신이 디렉토리일 경우 마지막으로 준 주소

    uint64_t is_alloc : 1; // 할당 여부

    uint64_t checksum : 8; // 체크섬
    uint64_t is_lock : 1;  // 누가 읽고 있는지 확인

    uint64_t YYYY : 7;
    uint64_t MM : 4;
    uint64_t DD : 5;

} fcb_t;

// 파일 관리자 구조체
// 256KB 정도
typedef struct FMv1_record
{
    uint64_t *base; // 바닥 주소

    uint64_t cur_ptr : 16;  // 보고 있는 주소 읽을때 씀
    uint64_t all_num : 16;  // 모든 파일의 수를 계산
    uint64_t last_addr : 6; // 마지막으로 준 주소
    uint64_t padding : 26;

    fcb_t FMv1_mem_L[MAX_FCB_SIZE_L];
    fcb_t FMv1_mem_M[MAX_FCB_SIZE_M];
    fcb_t FMv1_mem_S[MAX_FCB_SIZE_S];
    uint8_t mapping_L[MAX_FCB_SIZE_L]; // 매핑테이블에 들어갈 값들이 들어있는 테이블
    uint8_t mapping_M[MAX_FCB_SIZE_M];
    uint8_t mapping_S[MAX_FCB_SIZE_S];
    int8_t First_L[MAX_FCB_SIZE_L]; // 첫번째 말을 찾음
    int8_t First_M[MAX_FCB_SIZE_M];
    int8_t First_S[MAX_FCB_SIZE_S];

} FMv1_record;

#define fm_record ((FMv1_record *)FM_ADDR_START)

void fm_init(uint64_t *addr);

#endif
