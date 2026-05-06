# I release this sample under the MIT license: free for any use, provided
# you hold me harmless from any such use you make, and you retain my
# copyright on the actual sources.
# Copyright 2005 Jon Watte.

LIBNAME := TmxParser
SOURCES :=  $(shell find ../../lib/TmxParser -name "*.cpp")

LDFLAGS_TmxParser := -g -m64

CFLAGS_TmxParser := -MMD -I. -m64 -msse -mmmx -march=x86-64

CFLAGS_TmxParser += -I../../lib/TmxParser
CFLAGS_TmxParser += -I../../lib/TmxParser/base64
CFLAGS_TmxParser += -I../../lib/TmxParser/tinyxml

CFLAGS_TmxParser += -DUNICODE
CFLAGS_TmxParser += -DLINUX

CFLAGS_DEBUG_TmxParser := $(CFLAGS_TmxParser) -ggdb
CFLAGS_DEBUG_TmxParser += -DTORQUE_DEBUG
CFLAGS_DEBUG_TmxParser += -DTORQUE_DEBUG_GUARD
CFLAGS_DEBUG_TmxParser += -DTORQUE_NET_STATS

CFLAGS_TmxParser += -O3

CC := gcc
LD := gcc

TARGET_TmxParser := lib/libTmxParser.a
TARGET_TmxParser_DEBUG := lib/libTmxParser_DEBUG.a

LIB_TARGETS += $(TARGET_TmxParser)
LIB_TARGETS_DEBUG += $(TARGET_TmxParser_DEBUG)

OBJS_TmxParser := $(patsubst ../../lib/TmxParser/%,Release/TmxParser/%.o,$(SOURCES))
OBJS_TmxParser_DEBUG := $(patsubst ../../lib/TmxParser/%,Debug/TmxParser/%.o,$(SOURCES))

# Deriving the variable name from the target name is the secret sauce
# of the build system.
#
$(TARGET_TmxParser):	$(OBJS_TmxParser)
	@mkdir -p $(dir $@)
	ar cr $@ $(OBJS_TmxParser)

$(TARGET_TmxParser_DEBUG):	$(OBJS_TmxParser_DEBUG)
	@mkdir -p $(dir $@)
	ar cr $@ $(OBJS_TmxParser_DEBUG)

Release/TmxParser/%.o:	../../lib/TmxParser/%
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS_TmxParser) $< -o $@

Debug/TmxParser/%.o:	../../lib/TmxParser/%
	@mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS_DEBUG_TmxParser) $< -o $@

release_TmxParser: $(TARGET_TmxParser)
debug_TmxParser: $(TARGET_TmxParser_DEBUG)

.PHONY: debug_TmxParser release_TmxParser

DEPS += $(patsubst %.o,%.d,$(OBJS_TmxParser))
DEPS += $(patsubst %.o,%.d,$(OBJS_TmxParser_DEBUG))
