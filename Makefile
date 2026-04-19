# 검증 필요
# aarch64 OS 커널 빌드 설정

# 크로스 컴파일러
CC = aarch64-linux-gnu-gcc
AS = aarch64-linux-gnu-as
LD = aarch64-linux-gnu-ld
OBJCOPY = aarch64-linux-gnu-objcopy

# 컴파일 플래그
# -Iinclude 를 추가해서 헤더 파일을 편하게 찾게 했어!
CFLAGS = -mcpu=cortex-a72 -ffreestanding -nostdlib -nostdinc -O0 -g -Iinclude
ASFLAGS = -mcpu=cortex-a72
LDFLAGS = -T linker.ld

# 대상
BUILD_DIR = build
KERNEL_IMG = $(BUILD_DIR)/kernel8.img
ELF_FILE = $(BUILD_DIR)/kernel8.elf

# 소스 파일 탐색 (자동)
# src 폴더의 모든 .c 파일과 boot 폴더의 모든 .S 파일을 찾음
C_SOURCES = $(wildcard src/*.c)
S_SOURCES = $(wildcard boot/*.S)

# 오브젝트 파일 목록 생성 (build/파일명.o 형식)
# src/main.c -> build/main.o 로 변환됨
OBJS = $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
OBJS += $(patsubst boot/%.S, $(BUILD_DIR)/%.o, $(S_SOURCES))

# 기본 타겟
.PHONY: all clean rebuild

all: $(BUILD_DIR) $(KERNEL_IMG)

# build 폴더 생성
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# C 파일 컴파일 규칙
$(BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# ASM 파일 컴파일 규칙
$(BUILD_DIR)/%.o: boot/%.S
	$(CC) $(CFLAGS) -c $< -o $@

# 링킹
$(ELF_FILE): $(OBJS)
	$(LD) $(LDFLAGS) --gc-sections -o $(ELF_FILE) $(OBJS)

# 이미지 변환
$(KERNEL_IMG): $(ELF_FILE)
	$(OBJCOPY) $(ELF_FILE) -O binary $(KERNEL_IMG)
	@echo "---------------------------------------"
	@echo "  Kernel image built: $(KERNEL_IMG)"
	@echo "---------------------------------------"

clean:
	rm -rf $(BUILD_DIR)/

rebuild: clean all