.DEFAULT_GOAL := all
DEVKITPRO ?= /opt/devkitpro
DEVKITPPC ?= $(DEVKITPRO)/devkitPPC
TOOL := $(DEVKITPPC)/bin/powerpc-eabi-
GRR := external/grrlib/GRRLIB
PPC := $(DEVKITPRO)/portlibs/ppc
WII := $(DEVKITPRO)/libogc
CC_WII := $(TOOL)gcc
ARCH := -DGEKKO -DHW_RVL -mrvl -mcpu=750 -meabi -mhard-float
CPPFLAGS_WII := -Iinclude -I$(WII)/include -I$(PPC)/include -I$(PPC)/include/freetype2 -I$(GRR)/GRRLIB -I$(GRR)/lib/pngu
CFLAGS_WII := $(ARCH) -std=gnu11 -O2 -g -Wall -Wextra -Werror -ffunction-sections -fdata-sections
APP_SRC := $(wildcard source/*.c)
APP_OBJ := $(patsubst source/%.c,build/app/%.o,$(APP_SRC)) build/app/font.o
GRR_SRC := $(wildcard $(GRR)/GRRLIB/*.c) $(GRR)/lib/pngu/pngu.c
GRR_OBJ := $(patsubst %.c,build/%.o,$(GRR_SRC))
LIBS := -L$(WII)/lib/wii -L$(PPC)/lib -lfat -lwiiuse -lbte -lmad -lasnd -lfreetype -lbz2 -lbrotlidec -lbrotlicommon -lpng -ljpeg -lz -logc -lm
.PHONY: all clean test package
all: build/mpii3.dol
build/app/%.o: source/%.c
	@mkdir -p $(@D)
	$(CC_WII) $(CPPFLAGS_WII) $(CFLAGS_WII) -MMD -MP -c $< -o $@
build/app/font.o: source/font.S assets/ui.ttf assets/icon.png
	@mkdir -p $(@D)
	$(CC_WII) $(ARCH) -c $< -o $@
build/%.o: %.c
	@mkdir -p $(@D)
	$(CC_WII) $(CPPFLAGS_WII) $(ARCH) -O2 -g -ffunction-sections -fdata-sections -MMD -MP -c $< -o $@
build/mpii3.elf: $(APP_OBJ) $(GRR_OBJ)
	$(CC_WII) $(ARCH) -Wl,--gc-sections,-Map,build/mpii3.map $^ $(LIBS) -o $@
build/mpii3.dol: build/mpii3.elf
	$(DEVKITPRO)/tools/bin/elf2dol $< $@
test:
	./scripts/test.sh
package: all
	python3 scripts/package.py
clean:
	rm -rf build dist
-include $(APP_OBJ:.o=.d) $(GRR_OBJ:.o=.d)
