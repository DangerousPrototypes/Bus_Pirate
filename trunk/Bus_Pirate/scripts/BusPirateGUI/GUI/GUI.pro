PROJECT = BusPirateGui
TARGET = BusPirateGui
TEMPLATE = app

SUBDIRS += qextserialport
DEPENDS += qextserialport

CONFIG += warn_off qt thread
CONFIG += debug_and_release
CONFIG += staticlib
CONFIG += static

CONFIG(debug, debug|release) {
    LIBS += -L../qextserialport/build -lqextserialportd
} else {
   LIBS += -L../qextserialport/build -lqextserialport
}

OBJECTS_DIR = build/obj
MOC_DIR = build/moc
DEPENDPATH = . ..
INCLUDEPATH = . ..

DESTDIR = ../build
unix:VERSION = 1.0.0

HEADERS += 	\
			configure.h \
			BinMode.h \
			BPSettings.h \
			Events.h \
			Interface.h \
			MainWin.h

SOURCES += 	\
			BinMode.cpp \
			BPSettings.cpp \
			Events.cpp \
			Interface_i2c.cpp \
			Interface_jtag.cpp \
			Interface_onewire.cpp \
			#Interface_power.cpp \
			Interface_rawtext.cpp \
			Interface_rawwire.cpp \
			#Interface_spi.cpp \
			MainWin.cpp \
			main.cpp

