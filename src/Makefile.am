SUBDIRS = mikmod

INCLUDES = \
	-I $(top_srcdir)/src/mikmod/mmio \
	-I $(top_srcdir)/src/mikmod/mikmod

lib_LTLIBRARIES = libaudiere.la
include_HEADERS = audiere.h

if HAVE_OPENAL
OPENAL_SOURCES = device_oal.cpp device_oal.h
endif

if HAVE_AL
AL_SOURCES = device_al.cpp device_al.h
endif

if HAVE_OSS
OSS_SOURCES = device_oss.cpp device_oss.h
endif

libaudiere_la_SOURCES = \
	debug.cpp \
	debug.h \
	default_file.h \
	device.cpp \
	device_mixer.cpp \
	device_mixer.h \
	device_null.cpp \
	device_null.h \
	$(OPENAL_SOURCES) \
	$(AL_SOURCES) \
	$(OSS_SOURCES) \
	file_ansi.cpp \
	input.cpp \
	input_mod.cpp \
	input_mod.h \
	input_ogg.cpp \
	input_ogg.h \
	input_wav.cpp \
	input_wav.h \
	internal.h \
	repeatable.cpp \
	repeatable.h \
	resampler.cpp \
	resampler.h \
	sample_buffer.cpp \
	sound.cpp \
	threads_posix.cpp \
	threads.h \
	timer_posix.cpp \
	timer.h \
	tone.cpp \
	types.h \
	utility.cpp \
	utility.h \
	version.cpp

libaudiere_la_LDFLAGS = \
	-release $(VERSION) \
	-lvorbisfile \
	-lvorbis \
	-logg

libaudiere_la_LIBADD = \
	mikmod/mmio/libmmio.la \
	mikmod/mikmod/libmikmod.la
