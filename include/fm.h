#include "../include/defs.h"
#include "../include/types.h"
#include "../include/sect.h"

#ifndef __FM_H__
#define __FM_H__

/*
// 파일 객체 블록
// 16B 크기
typedef struct fcb_t
{
    // 1. 파일 이름 (8 bytes)
    int8_t alias[MAX_FILE_NAME];

    uint8_t fid;         // 파일 id
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

    fcb_t FMv1_mem[MAX_FCB_SIZE_L + 1][MAX_FCB_SIZE_M + 1][MAX_FCB_SIZE_S + 1];
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
*/

// 파일 객체 블록 V2
// 더 큰 파일 크기를 지원하기 위해 lens 필드 확장
typedef struct fcb_t
{
    // 1. 파일 이름 (8 bytes)
    // /0 또한 포함
    int8_t alias[MAX_FILE_NAME + 1];

    uint16_t lens : 11; // 파일 길이 (1KB 단위, 최대 1MB)
    uint16_t fid : 5;   // 파일 id

    uint64_t padding : 12; // 패딩
    uint64_t depth : 3;    // 파일 깊이 (0~2)
    uint64_t is_dir : 1;   // 디렉토리 여부

    uint64_t me_auth : 3;  // 나의 권한
    uint64_t you_auth : 3; // 너의 권한

    uint64_t ppdir_addr : 4; // 디렉토리당 최대 16개 파일 (4비트 = 0-15)
    uint64_t pdir_addr : 4;  // 부모 디렉토리 내 인덱스 (4비트 = 0-15)
    uint64_t me_addr : 4;    // 현재 파일의 인덱스 (4비트 = 0-15)
    uint64_t last_addr : 4;  // 마지막으로 준 주소 (4비트 = 0-15)

    uint64_t is_alloc : 1; // 할당 여부

    uint64_t checksum : 8; // 체크섬
    uint64_t is_lock : 1;  // 누가 읽고 있는지 확인

    uint64_t YYYY : 7;
    uint64_t MM : 4;
    uint64_t DD : 5;

} fcb_t;

// 파일 관리자 구조체 V2
typedef struct FMv2_record
{
    uint64_t *base; // 바닥 주소

    uint64_t cur_ptr : 16;  // 보고 있는 주소 읽을때 씀
    uint64_t all_num : 16;  // 모든 파일의 수를 계산
    uint64_t last_addr : 5; // 마지막으로 준 파일의 수 0~16
    uint64_t padding : 27;  // 패딩

    // 메타데이터 배열
    fcb_t FMv2_mem[MAX_FCB_SIZE_L + 1][MAX_FCB_SIZE_M + 1][MAX_FCB_SIZE_S + 1];
    /*
    fcb_t FMv2_mem_L[MAX_FCB_SIZE_L];
    fcb_t FMv2_mem_M[MAX_FCB_SIZE_M];
    fcb_t FMv2_mem_S[MAX_FCB_SIZE_S];
    */
    // 매핑테이블
    uint8_t mapping[MAX_FCB_SIZE_L + 1][MAX_FCB_SIZE_M + 1][MAX_FCB_SIZE_S + 1];
    /*
    uint8_t mapping_L[MAX_FCB_SIZE_L]; // 매핑테이블
    uint8_t mapping_M[MAX_FCB_SIZE_M];
    uint8_t mapping_S[MAX_FCB_SIZE_S];
    */

    // 첫번째 말을 찾음
    int8_t First[MAX_FCB_SIZE_L + 1][MAX_FCB_SIZE_M + 1][MAX_FCB_SIZE_S + 1];
    /*
    int8_t First_L[MAX_FCB_SIZE_L]; // 첫번째 문자
    int8_t First_M[MAX_FCB_SIZE_M];
    int8_t First_S[MAX_FCB_SIZE_S];
    */
} FMv2_record;

#define fm_record ((FMv2_record *)FM_ADDR_START)

void fm_init(uint64_t *addr);
bool fm_check(FMv2_record *reco, uint8_t cmd, int8_t path[27]);
fcb_t *fm_create(FMv2_record *reco, int8_t path[27], int8_t name[8], uint64_t size, bool ok_dir);

/*
fcb_t *fm_delete(FMv2_record *reco, uint8_t path[26]);
fcb_t *fm_find_path(FMv2_record *reco, uint8_t path[26]);
void fm_list_dir(FMv2_record *reco, uint8_t path[26]);
int32_t fm_read(FMv2_record *reco, uint8_t path[26], uint64_t offset, uint8_t *buf, uint32_t size);
int32_t fm_write(FMv2_record *reco, uint8_t path[26], uint64_t offset, uint8_t *buf, uint32_t size);
int8_t fm_rename(FMv2_record *reco, uint8_t path[26], int8_t new_name[8]);
int8_t fm_change_auth(FMv2_record *reco, uint8_t path[26], uint8_t me_auth, uint8_t you_auth);
fcb_t *fm_get_info(FMv2_record *reco, uint8_t path[26]);
uint32_t fm_allocate_space(FMv2_record *reco, uint32_t size);
void fm_deallocate_space(FMv2_record *reco, uint32_t addr, uint32_t size);
int8_t fm_mkdir(FMv2_record *reco, uint8_t path[26], int8_t dir_name[8]);
int8_t fm_rmdir(FMv2_record *reco, uint8_t path[26]);
uint8_t fm_check_available(FMv2_record *reco, uint32_t required_size);
void fm_sync(FMv2_record *reco);
*/

#endif
