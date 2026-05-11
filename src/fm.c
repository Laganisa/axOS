#include "../include/defs.h"
#include "../include/fm.h"
#include "../include/io.h"

// init 함수

/*
void fm_init(uint64_t *addr)
{
    fmv2_record->base = addr;
    fmv2_record->cur_ptr = 0;
    fmv2_record->all_num = 0;
}
*/

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
int8_t token(int8_t lang[27])
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
    tokn[0] - tokn[1];
    // 토큰의 차이들을 저장하기 공백은 제외
    // 공백이 나오면 '.'이 나올 때까지 가기

    // 현재 토큰의 첫 글자를 다른 곳에 저장하고
    // 매핑 테이블이 모음이 속해있는 문자를 자음이 속한 문자로 매핑하기
    // 2번째 부터 그 차이를 적어나가는 식으로
}

//  init 함수
void fm_init(uint64_t *addr)
{
    fm_record->base = addr;
    fm_record->cur_ptr = 0;
    fm_record->all_num = 0;
    fm_record->last_addr = 0;
}

// size는 B 단위로 받음
// 최대 1MB (1048576 바이트)
// 디렉토리당 최대 2MB (2097152 바이트)
// 파일 생성 함수
/*
    뭐 적기
*/
fcb_t *fm_creat(FMv2_record *reco, int8_t name[8], uint8_t path[27], uint32_t size, uint8_t ok_dir)
{
    // 확인: 1MB 초과 파일 거부
    if (size > MAX_FILE_SIZE)
    {
        return 0;
    }

    // 디렉토리인 경우 2MB 초과 거부
    if (ok_dir && size > MAX_DIR_SIZE)
    {
        return 0;
    }

    // 이름이 만들수 있는지 확인
    // 이름은 8글자여야 하고, '.'은 5번째 글자에만 올 수 있음

    for (int i = 0; i < 8; i++)
    {
        if (name[i] == 0x2E && i > 0x4) // '.' 이면서 뒤에 있으면
        {
            break; // or 리턴 만들수 없는 파일
        }
    }

    fcb_t *new_file;
    uint16_t top_addr; // 맞나?
    uint16_t mid_addr;
    uint16_t bot_addr;
    uint8_t auth; // 권한

    uint8_t new_depth;

    if (fm_check(reco, 0, path) == FALSE)
    {
        return 0; // 경로가 유효하지 않음
    }

    // 경로가 유효하다
    if (path[17] == 0x20) // 2번째 경로인지
    {

        // ! 나중에
    }
    else if (path[8] == 0x20 && reco->last_addr < 16) // 첫번째 경로인지 && 루트 디렉토리에 빈 공간이 있는지
    {
        // ! 디렉토리에 규칙아래 같은 이름이 있는지 확인
        fm_check(reco, 1, path);
        new_file->fid = reco->last_addr;                     // 자신의 주소를 받고
        new_file = &(reco->FMv2_mem[new_file->fid][64][64]); // 자신의 메타데이터가 들어갈 위치를 찾고
        reco->last_addr += 1;                                // 마지막 할당한 값을 하나 증가
        reco->all_num += 1;
    }
    else // 3번째 경로
    {
        // ! 나중에
    }

    // 새 파일에 넣는 로직

    // 이름 넣기
    for (int i = 0; i < MAX_FILE_NAME; i++)
    {
        new_file->alias[i] = name[i];
    }
    new_file->is_dir = ok_dir; // 디렉토리 여부
    new_file->depth = new_depth;
    new_file->lens = size >> 10; // 1KB 단위로 변환 (1MB = 1024개)
    // 권한 로직
    new_file->me_auth = 7;
    new_file->you_auth = 7;

    // 위치 로직
    new_file->ppdir_addr = top_addr;
    new_file->pdir_addr = mid_addr;
    new_file->me_addr = bot_addr;
    // 날짜 로직
    // ! 나중에 짜기

    return new_file;
}

//  파일 삭제
// ! 나중에 구현

//  파일 탐색
// ! 나중에 구현
uint8_t fm_find(FMv2_record *reco)
{
    // ! 나중에
}

//  파일 목록 조회
// ! 나중에 구현
void fm_list(FMv2_record *reco, int8_t path[27])
{
    // ! 나중에
}

//  경로 유효성 검사
/*
    규칙에 따라 경로가 유효한지 확인
    cmd == 0
    1. 구문자가 고정된 위치가 맞는지 검사
    2. 각 마디에 '.' 의 위치가 맞는지 검사
    3. 공백 후 문자가 나오는지 검사
    cmd == 1
    1. 경로에서 최근 디렉토리를 알아낸다
    2. 디랙토리를 알아냈다면 mapping 테이블에서
    현재 주소를 변환한 값(저기 위에 token 함수를 통과한 값)이랑
    같은 문자가 있는지 확인
*/
bool fm_check(FMv2_record *reco, uint8_t cmd, int8_t path[27])
{
    // 경로 유호성 검사
    if (cmd == 0)
    {
        bool is_ok = TRUE;

        // 1. 구문자 및 널 문자 고정 위치 검사
        if (path[8] != '/' || path[17] != '/' || path[26] != '\0')
        {
            return FALSE;
        }
        if (path[22] != '.')
        {
            return FALSE; // 파일명 영역의 '.' 위치 고정
        }
        // 2. 이중 루프로 각 마디(8글자씩) 검사
        int segments[3] = {0, 9, 18}; // 각 마디의 시작 인덱스

        for (int s = 0; s < 3; s++)
        {
            int start = segments[s];
            bool space_started = FALSE;

            for (int i = 0; i < 8; i++)
            {
                int8_t current_char = path[start + i];

                // 파일명 마디(s==2)에서 '.' 위치(인덱스 22)는 건너뜀
                if (s == 2 && (start + i) == 22)
                {
                    continue;
                }

                if (current_char == 0x20)
                {
                    space_started = TRUE; // 이제부터는 계속 공백이어야 함
                }
                else
                {
                    // 공백이 이미 시작됐는데 문자가 나타나면 규칙 위반
                    if (space_started)
                    {
                        is_ok = FALSE;
                        break;
                    }
                    // ! 특수 문자 로직 넣는 자리
                    // 보통은 특수 문자를 허용하지 않음
                }
            }
            if (!is_ok)
            {
                break;
            }
        }
        return is_ok;
    }
    // 디랙토리에 중복 있는지 검사
    else if (cmd == 1)
    {
        // 검사하는 법
        /*
            경로에서 최근 디렉토리를 알아낸다
            디랙토리를 알아냈다면 mapping 테이블에서
            현재 주소를 변환한 값(저기 위에 token 함수를 통과한 값)이랑
            같은 문자가 있는지 확인
            확인은 일단 저 mapping 테이블이 8비트여서 문자 8개씩 비교할 수 있음
            또한 한 디렉토리당 파일 수가 16개 여서 2번 반복하면 됨
        */
        // 그럼 일단 현재 경로에서 위에 디렉토리가 몇개인지 확인하는 로직
        // 경로에서 앞의 문자가 공백이 아니면 디렉토리가 있다 판단
        // 항상 경로를 만들때 앞에서 부터 채우기 때문
        // 8번쨰는 구문자여서
        // 9번째와 18번째 글자가 공백이 아니면 각각 디렉토리가 있다고 판단

        uint8_t dir_count = 0;

        if (path[9] != 0x20)
        {
            dir_count++;
        }
        if (path[18] != 0x20)
        {
            dir_count++;
        }

        // 디렉토리는 다음에 나오는 위치가 16임
        if (dir_count == 0)
        {
            // 루트 디렉토리인 경우
            // 매핑 테이블에서 루트 디렉토리에 해당하는 값이 있는지 확인
            for (int i = 0; i < 16; i++)
            {
                if (reco->mapping[0][i][16] == 0)
                {
                    // 빈 공간 발견
                    break;
                }
            }
        }
        else if (dir_count == 1)
        {
            // 첫번째 디렉토리가 있는 경우
            // 매핑 테이블에서 첫번째 디렉토리에 해당하는 값이 있는지 확인
        }
        else if (dir_count == 2)
        {
            // 두번째 디렉토리가 있는 경우
            // 매핑 테이블에서 두번째 디렉토리에 해당하는 값이 있는지 확인
        }
    }
    // 맞지 않는 명령
    else
    {
        return FALSE;
    }
}

/*
fcb__t *fm_delete_(FMv2_record *reco, uint8_t path[27]) {}

fcb__t *fm_find__path(FMv2_record *reco, uint8_t path[27]) {}

void fm_list__dir(FMv2_record *reco, uint8_t path[27]) {}

int32_t fm_read_(FMv2_record *reco, uint8_t path[27], uint64_t offset, uint8_t *buf, uint32_t size) {}

int32_t fm_write_(FMv2_record *reco, uint8_t path[27], uint64_t offset, uint8_t *buf, uint32_t size) {}

int8_t fm_rename_(FMv2_record *reco, uint8_t path[27], int8_t new_name[8]) {}

int8_t fm_change_auth_(FMv2_record *reco, uint8_t path[27], uint8_t me_auth, uint8_t you_auth) {}

fcb__t *fm_get_info_(FMv2_record *reco, uint8_t path[27]) {}

uint32_t fm_allocate_space_(FMv2_record *reco, uint32_t size) {}

void fm_deallocate_space_(FMv2_record *reco, uint32_t addr, uint32_t size) {}

int8_t fm_mkdir_(FMv2_record *reco, uint8_t path[27], int8_t dir_name[8]) {}

int8_t fm_rmdir_(FMv2_record *reco, uint8_t path[27]) {}

uint8_t fm_check_available_(FMv2_record *reco, uint32_t required_size) {}

void fm_sync_(FMv2_record *reco) {}
*/