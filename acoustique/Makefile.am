SUBDIRS = mikmod

INCLUDES = \
	-I mikmod/mmio \
	-I mikmod/mikmod \
        -I mad-0.13.0b

lib_LTLIBRARIES = libacoustique.la
include_HEADERS = acoustique.h
libacoustique_la_SOURCES = \
	acoustique.cpp \
	acq_mod.cpp \
	acq_mp3.cpp \
	acq_ogg.cpp \
	acq_wav.cpp

libacoustique_la_LDFLAGS = \
	-release $(VERSION) \
	-lmad \
	-lvorbisfile

libacoustique_la_LIBADD = \
	mikmod/mmio/libmmio.la \
	mikmod/mikmod/libmikmod.la
