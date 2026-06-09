ARTIFACT = testapp

PLATFORM ?= x86_64

BUILD_PROFILE ?= debug

CONFIG_NAME ?= $(PLATFORM)-$(BUILD_PROFILE)
OUTPUT_DIR = build/$(CONFIG_NAME)
TARGET = $(OUTPUT_DIR)/$(ARTIFACT)

CC = qcc -Vgcc_nto$(PLATFORM)
CXX = q++ -Vgcc_nto$(PLATFORM)_cxx
LD = $(CXX)

QT6_DIR = ../../dev/Qt6_qnx

INCLUDES += -Isrc
INCLUDES += -I$(QT6_DIR)/include
INCLUDES += -I$(QT6_DIR)/include/QtWidgets
INCLUDES += -I$(QT6_DIR)/include/QtGui
INCLUDES += -I$(QT6_DIR)/include/QtCore
INCLUDES += -I$(QT6_DIR)/mkspecs/qnx-x86-64-qcc

LDFLAGS_all += -Wl,-rpath,/data/var/qt/lib
LDFLAGS_all += -Wl,-rpath-link,$(QT6_DIR)/lib
LDFLAGS_all += -Wl,-rpath-link,$(QNX_TARGET)/x86_64/lib
LDFLAGS_all += -Wl,-rpath-link,$(QNX_TARGET)/x86_64/usr/lib

LIBS += $(QT6_DIR)/lib/libQt6Widgets.so
LIBS += $(QT6_DIR)/lib/libQt6Gui.so
LIBS += $(QT6_DIR)/lib/libQt6Core.so
LIBS += -lGLESv2 -lEGL -lm

DEFINES += -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB

CCFLAGS_release += -O2 -DQT_NO_DEBUG
CCFLAGS_debug += -g -O0 -fno-builtin -DQT_NO_DEBUG
CCFLAGS_coverage += -g -O0 -ftest-coverage -fprofile-arcs
LDFLAGS_coverage += -ftest-coverage -fprofile-arcs
CCFLAGS_profile += -g -O0 -finstrument-functions
LIBS_profile += -lprofilingS

CCFLAGS_all += -Wall -fmessage-length=0 -D_REENTRANT -fPIC
CCFLAGS_all += $(CCFLAGS_$(BUILD_PROFILE))
LDFLAGS_all += $(LDFLAGS_$(BUILD_PROFILE))
LIBS_all += $(LIBS_$(BUILD_PROFILE))
DEPS = -Wp,-MMD,$(@:%.o=%.d),-MT,$@

OBJDIR = $(OUTPUT_DIR)
CXX_OBJS = $(OBJDIR)/src/main.o

.PHONY: all clean rebuild

all: $(TARGET)

$(OBJDIR)/src/main.o: src/main.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(DEPS) -o $@ $(INCLUDES) $(CCFLAGS_all) $(CCFLAGS) $(DEFINES) $<

$(TARGET): $(CXX_OBJS)
	$(LD) -o $(TARGET) $(LDFLAGS_all) $(LDFLAGS) $(CXX_OBJS) $(LIBS_all) $(LIBS)

clean:
	rm -fr build

rebuild: clean all

-include $(CXX_OBJS:%.o=%.d)
