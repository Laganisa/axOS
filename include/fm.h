#include "../include/defs.h"

#ifndef __FM_H__
#define __FM_H__

// 파일 객체 블록
typedef struct fcb_t
{
    // 파일 속성들
    int8_t alias[12];   // 파일 이름(사용자가 지정)
    uint8_t id : 6;     // 파일 id 겸 위치
    uint8_t depth : 1;  // 파일 깊이
    uint8_t is_dir : 1; // 파일인지

    uint8_t me_auth : 4;  // 나의 권한
    uint8_t you_auth : 4; // 너의 권한

    // 파일 특징들 크기라던가 가용공간 등등
    uint8_t lens; // 파일 길이

    // 위치 자료들
    uint32_t pdir_addr; // 부모 디렉토리 위치 (오프셋 포함)

} fcb_t;

// 파일 관리자 구조체
// 이 구조체 밑에 모든 파일이 있는 것
typedef struct FMv1_record
{

} FMv1_record;

#endif