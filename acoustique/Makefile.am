SUBDIRS = mikmod resample

INCLUDES = \
	-I mikmod/mmio \
	-I mikmod/mikmod \
        -I resample

lib_LTLIBRARIES = libacoustique.la
include_HEADERS = acoustique.h
libacoustique_la_SOURCES = \
	acoustique.cpp \
	acq_mod.cpp \
	acq_mp3.cpp \
	acq_ogg.cpp \
	acq_wav.cpp \
	acoustique.h \
	acq_internal.hpp \
	endian.hpp

libacoustique_la_LDFLAGS = \
	-release $(VERSION) \
	-lmad \
	-lvorbisfile \
	-lvorbis \
	-logg

libacoustique_la_LIBADD = \
	mikmod/mmio/libmmio.la \
	mikmod/mikmod/libmikmod.la \
	resample/libresample.la
