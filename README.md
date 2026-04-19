# myOS - aarch64 Bare Metal OS

aarch64 아키텍처를 위한 운영체제 개발 프로젝트입니다.

## 프로젝트 구조

```text
myOS/
├── boot/          - 부트로더 (aarch64 assembly)
├── src/           - 커널 소스 코드 (C)
├── include/       - 헤더 파일
├── build/         - 빌드 출력 디렉토리
├── tools/         - 개발 도구
├── linker.ld      - 링커 스크립트
├── Makefile       - 빌드 설정
└── README.md      - 이 파일
```

## 필요한 도구

### Windows에서의 설정

1. **WSL2 또는 MinGW 설치**
   - WSL2 권장 (Linux 환경이 필요함)

2. **aarch64 크로스 컴파일 도구 설치**

#### WSL2/Linux에서

```bash
sudo apt-get update
sudo apt-get install gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu
```

#### MinGW에서

- MinGW-w64 다운로드: <https://www.mingw-w64.org/>
- aarch64 크로스 컴파일러 설치 필요

## 빌드 방법

### WSL2에서

```bash
# 빌드
make

# 정리
make clean

# 재빌드
make rebuild
```

### 빌드 결과

- `build/kernel8.elf` - ELF 포맷 실행 파일
- `build/kernel8.img` - 부팅 가능한 이미지

## QEMU 에뮬레이션

aarch64 커널을 테스트하려면 QEMU를 사용할 수 있습니다.

### QEMU 설치 (WSL2/Linux)

```bash
sudo apt-get install qemu-system-arm
```

### 실행

```bash
qemu-system-aarch64 -M virt -cpu cortex-a72 -m 512M -kernel build/kernel8.elf -nographic
```

### 빠져나오기

- `Ctrl + A` 누른 후 `X` 입력

## 개발 환경 설정 (VS Code)

1. WSL 확장 설치
2. VS Code를 WSL로 열기: `code .`
3. C/C++ 확장 설치

## 다음 단계

- [ ] MMU (Memory Management Unit) 구현
- [ ] 인터럽트 핸들링
- [ ] 멀티태스킹
- [ ] 파일 시스템
- [ ] 디바이스 드라이버

## 참고 자료

- ARM64 Architecture Reference
- Raspberry Pi 4 Technical Specification
- OSDev Wiki: <https://wiki.osdev.org/>

## 라이선스

MIT License
