#include "../include/defs.h"
#include "../include/types.h"

#ifndef __FM_H__
#define __FM_H__

// 파일 객체 블록
// 16B 크기
typedef struct fcb_t
{
    // 1. 파일 이름 (8 bytes)
    int8_t alias[8];

    uint64_t lens : 6;   // 파일 길이 64바이트가 기준
    uint64_t depth : 1;  // 파일 깊이
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

    uint16_t cur_ptr; // 보고 있는 주소
    uint16_t all_num; // 모든 파일의 수를 계산

    fcb_t FMv1_mem[MAX_FCB_SIZE];
    uint8_t HASH[MAX_FCB_SIZE];

} FMv1_record;

uint8_t hash();

extern FMv1_record fm_record;

#endif