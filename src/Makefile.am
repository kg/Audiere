SUBDIRS = mikmod mpegsound

INCLUDES = \
	-I $(top_srcdir)/src/mikmod/mmio \
	-I $(top_srcdir)/src/mikmod/mikmod \
	-I $(top_srcdir)/src/mpegsound

lib_LTLIBRARIES = libaudiere.la
include_HEADERS = audiere.h

if HAVE_OPENAL
OPENAL_SOURCES = device_oal.cpp device_oal.h
else
OPENAL_DIST    = device_oal.cpp device_oal.h
endif

if HAVE_AL
AL_SOURCES = device_al.cpp device_al.h
else
AL_DIST    = device_al.cpp device_al.h
endif

if HAVE_OSS
OSS_SOURCES = device_oss.cpp device_oss.h
else
OSS_DIST    = device_oss.cpp device_oss.h
endif

EXTRA_DIST = $(OPENAL_DIST) $(AL_DIST) $(OSS_DIST)

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
	input_flac.cpp \
	input_flac.h \
	input_mod.cpp \
	input_mod.h \
	input_mp3.cpp \
	input_mp3.h \
	input_ogg.cpp \
	input_ogg.h \
	input_wav.cpp \
	input_wav.h \
	internal.h \
	noise.cpp \
	repeatable.cpp \
	repeatable.h \
	resampler.cpp \
	resampler.h \
	sample_buffer.cpp \
	sound.cpp \
	sound_effect.cpp \
	square_wave.cpp \
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
	-logg \
	@FLAC_LIBS@

libaudiere_la_LIBADD = \
	mikmod/mmio/libmmio.la \
	mikmod/mikmod/libmikmod.la \
	mpegsound/libmpegsound.la
