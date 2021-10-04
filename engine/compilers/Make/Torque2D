APPNAME := ../../../Torque2D

2D_SOURCES :=            $(shell find ../../source/2d/ -name "*.cc") + \
						 $(shell find ../../source/2d/ -name "*.cpp")
ALGORITHM_SOURCES :=     $(shell find ../../source/algorithm/ -name "*.cc")
ASSETS_SOURCES :=        $(shell find ../../source/assets/ -name "*.cc")
AUDIO_SOURCES :=         $(shell find ../../source/audio/ -name "*.cc")
BITMAPFONT_SOURCES :=    $(shell find ../../source/bitmapFont/ -name "*.cc")
BOX2D_SOURCES :=         $(shell find ../../source/Box2D/ -name "*.cpp")
COLLECTION_SOURCES :=    $(shell find ../../source/collection/ -name "*.cc")
COMPONENT_SOURCES :=     $(shell find ../../source/component/ -name "*.cpp")
CONSOLE_SOURCES :=       $(shell find ../../source/console/ -name "*.cc")
DEBUG_SOURCES :=         $(shell find ../../source/debug/ -name "*.cc")
DELEGATES_SOURCES :=     $(shell find ../../source/delegates/ -name "*.cc")
GAME_SOURCES :=          $(shell find ../../source/game/ -name "*.cc")
GRAPHICS_SOURCES :=      $(shell find ../../source/graphics/ -name "*.cc")
GUI_SOURCES :=           $(shell find ../../source/gui/ -name "*.cc")
INPUT_SOURCES :=         $(shell find ../../source/input/ -name "*.cc")
IO_SOURCES :=            $(shell find ../../source/io/ -name "*.cc") 
MATH_SOURCES :=          $(shell find ../../source/math/ -name "*.cc") + \
                         $(shell find ../../source/math/ -name "*.cpp") + \
                         $(shell find ../../source/math/ -name "*.asm")
MEMORY_SOURCES :=        $(shell find ../../source/memory/ -name "*.cc")
MESSAGING_SOURCES :=     $(shell find ../../source/messaging/ -name "*.cc")
MODULE_SOURCES :=        $(shell find ../../source/module/ -name "*.cc")
NETWORK_SOURCES :=       $(shell find ../../source/network/ -name "*.cc")
PERSISTENCE_SOURCES :=   $(shell find ../../source/persistence/ -name "*.cc") + \
                         $(shell find ../../source/persistence/ -name "*.cpp")
PLATFORM_SOURCES :=      $(shell find ../../source/platform/ -name "*.cc") + \
                         $(shell find ../../source/platform/ -name "*.cpp") + \
						 $(shell find ../../source/platform/ -name "*.asm")
PLATFORM_UNIX_SOURCES := $(shell find ../../source/platformX86UNIX/ -name "*.cc")
SIM_SOURCES :=           $(shell find ../../source/sim/ -name "*.cc") + \
                         $(shell find ../../source/sim/ -name "*.cpp")
STRING_SOURCES :=        $(shell find ../../source/string/ -name "*.cc") + \
                         $(shell find ../../source/string/ -name "*.cpp") 

SOURCES := $(2D_SOURCES) + \
	$(ALGORITHM_SOURCES) + \
	$(ASSETS_SOURCES) + \
	$(AUDIO_SOURCES) + \
	$(BITMAPFONT_SOURCES) + \
	$(BOX2D_SOURCES) + \
	$(COLLECTION_SOURCES) + \
	$(COMPONENT_SOURCES) + \
	$(CONSOLE_SOURCES) + \
	$(DEBUG_SOURCES) + \
	$(DELEGATES_SOURCES) + \
	$(GAME_SOURCES) + \
	$(GRAPHICS_SOURCES) + \
	$(GUI_SOURCES) + \
	$(INPUT_SOURCES) + \
	$(IO_SOURCES) + \
	$(MATH_SOURCES) + \
	$(MEMORY_SOURCES) + \
	$(MESSAGING_SOURCES) + \
	$(MODULE_SOURCES) + \
	$(NETWORK_SOURCES) + \
	$(PERSISTENCE_SOURCES) + \
	$(PLATFORM_SOURCES) + \
	$(PLATFORM_UNIX_SOURCES) + \
	$(SIM_SOURCES) + \
	$(STRING_SOURCES)

LDFLAGS := -g -m32
LDLIBS := -lstdc++ -lm -ldl -lpthread -lrt -lX11 -lXft -lSDL -lopenal

CFLAGS := -std=gnu++11 -MMD -I. -Wfatal-errors -Wunused -m32 -msse -march=i686 -pipe

CFLAGS += -I/usr/include
CFLAGS += -I/usr/include/freetype2
CFLAGS += -I../../source
CFLAGS += -I../../source/persistence/rapidjson/include
CFLAGS += -I../../lib/ljpeg
CFLAGS += -I../../lib/zlib
CFLAGS += -I../../lib/lpng
CFLAGS += -I../../lib/freetype
CFLAGS += -I../../lib/libvorbis/include
CFLAGS += -I../../lib/libogg/include
CFLAGS += -I../../lib/openal/LINUX/

CFLAGS += -DLINUX
CFLAGS += -Di386


CFLAGS_DEBUG := $(CFLAGS) -ggdb
CFLAGS_DEBUG += -DTORQUE_DEBUG
CFLAGS_DEBUG += -DTORQUE_DEBUG_GUARD
CFLAGS_DEBUG += -DTORQUE_NET_STATS

CFLAGS += -O0

NASMFLAGS := -f elf -D LINUX

CC := gcc
LD := gcc

APP_TARGETS += $(APPNAME)
APP_TARGETS_DEBUG += $(APPNAME)_DEBUG

OBJS := $(patsubst ../../source/%,Release/%.o,$(SOURCES))
OBJS := $(filter %.o, $(OBJS))

OBJS_DEBUG := $(patsubst ../../source/%,Debug/%.o,$(SOURCES))
OBJS_DEBUG := $(filter %.o,$(OBJS_DEBUG))

$(APP_TARGETS): $(OBJS) $(LIB_TARGETS)
	@echo Linking release
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LIB_TARGETS) $(LDLIBS)

$(APP_TARGETS_DEBUG): $(OBJS_DEBUG) $(LIB_TARGETS_DEBUG)
	@echo Linking debug
	$(LD) $(LDFLAGS) -o $@ $(OBJS_DEBUG) $(LIB_TARGETS_DEBUG) $(LDLIBS)

Release/%.asm.o:	../../source/%.asm
	@echo Building release asm $@
	@mkdir -p $(dir $@)
	nasm $(NASMFLAGS) $< -o $@

Release/%.o:	../../source/%
	@echo Building release object $@
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) $< -o $@

Debug/%.asm.o:	../../source/%.asm
	@echo Building debug asm $@
	@mkdir -p $(dir $@)
	nasm $(NASMFLAGS) $< -o $@

Debug/%.o:	../../source/%
	@echo Building debug object $@
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS_DEBUG) $< -o $@

release: $(APP_TARGETS)
debug: $(APP_TARGETS_DEBUG)

.PHONY: $(APP_TARGETS) $(APP_TARGETS_DEBUG)

DEPS += $(patsubst %.o,%.d,$(OBJS))
DEPS += $(patsubst %.o,%.d,$(OBJS_DEBUG))

APPNAME :=
SOURCES :=
