MODULE = System/OpenAL

CCFLAGS = \
    -I$(SYSDIR) \
    -I$(SYSDIR)/OpenAL/include \
    -I$(SYSDIR)/OpenAL/OpenAL32/Include \
    -DAL_BUILD_LIBRARY \
    -DAL_ALEXT_PROTOTYPES \
    -DVERDE_USE_REAL_FILE_IO \

OBJECTS = \

ifneq ($(OS), mac)
OBJECTS += \
 OpenAL32/alAuxEffectSlot.o \
 OpenAL32/alBuffer.o        \
 OpenAL32/alDatabuffer.o    \
 OpenAL32/alEffect.o        \
 OpenAL32/alError.o         \
 OpenAL32/alExtension.o     \
 OpenAL32/alFilter.o        \
 OpenAL32/alListener.o      \
 OpenAL32/alSource.o        \
 OpenAL32/alState.o         \
 OpenAL32/alThunk.o         \
 Alc/ALc.o                  \
 Alc/alcConfig.o            \
 Alc/alcEcho.o              \
 Alc/alcModulator.o         \
 Alc/alcReverb.o            \
 Alc/alcRing.o              \
 Alc/alcThread.o            \
 Alc/ALu.o                  \
 Alc/bs2b.o                 \
 Alc/null.o                 \
 Alc/panning.o              \
 Alc/mixer.o                \

endif

ifeq ($(TARGET_OS), android)
OBJECTS += Alc/audiotrack.o
ifdef POST_FROYO
OBJECTS += Alc/opensles.o
endif
CCFLAGS += -I/Developer/AndroidNDK/platforms/android-9/arch-arm/usr/include
CCFLAGS += -DOPENAL_FIXED_POINT -DOPENAL_FIXED_POINT_SHIFT=16
endif

ifeq ($(OS), linux)
OBJECTS += Alc/oss.o
endif

include $(ROOTDIR)/module.mk
