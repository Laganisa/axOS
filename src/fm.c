#include "../include/defs.h"
#include "../include/fm.h"

// init 함수

void fm_init(uint64_t *addr)
{
    fm_record->base = addr;
    fm_record->cur_ptr = 0;
    fm_record->all_num = 0;
}

// 파일 만들기
// 이름 입력하기 뭐 등등
// 만들때 별칭은 8글자가 되도록 패딩을 해준다
// 고로 글자는 전부 8글자이다(한국어 제외, 한국어는 3바이트를 쓰니까)

/*
    글자 8자 - 구문자 - 글자 8자 - 구문자 - 글자 8자
    해시함수에 넣기 좋게 자르면
    3글자 - 구문자 - 3글자 - 구문자
    3/3/3으로 쪼개고 차이를 저장하기
    매핑 테이블도 필요함
*/
int8_t token(int8_t lang[26])
{
    int8_t tokn[9];

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            tokn[3 * i + j] = lang[j + 9 * i];
        }
    }

    int8_t carve_lang = tokn[0]; // 해시에 넣을거

    // 토큰의 차이들을 저장하기
    // 현재 토큰의 첫 글자를 다른 곳에 저장하고
    // 매핑 테이블이 모음이 속해있는 문자를 자음이 속한 문자로 매핑하기
    // 2번째 부터 그 차이를 적어나가는 식으로
}

// 파일 관리

// 파일 생성

fcb_t *fm_creat(FMv1_record *reco, int8_t name[8], uint8_t path[26], uint8_t is_dir)
{
    // 이름이 만들수 있는지 확인
    for (int i = 0; i < 8; i++)
    {
        if (name[i] == "." && i > 4)
        {
            break;
        }
    }

    // 만들수 있는지 확인

    if (path[25]) // 3번째 경로인지
    {
        fcb_t new_file = reco->FMv1_mem_S[1];
    }
    else if (path[17] == 0x20) // 2번째 경로인지
    {
        fcb_t new_file = reco->FMv1_mem_M[1];
    }
    else if (path[8] == 0x20) // 첫번째 경로인지
    {
        fcb_t new_file = reco->FMv1_mem_L[reco->last_addr]; // 마지막에 준 번호로 주기
        reco->last_addr += 1;
        reco->all_num += 1;
    }
    else
    {
        return 0; // 맞지 않는 경로 값
    }
}

// 파일 삭제

// 파일 탐색

uint8_t fm_find(FMv1_record *reco)
{
}

// 파일 조작

// 파일 정보 관리

// 토큰으로 나누기

// 파일 만들기
