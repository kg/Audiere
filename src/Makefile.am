SUBDIRS = mpegsound

INCLUDES = -I $(top_srcdir)/src/mpegsound

lib_LTLIBRARIES = libaudiere.la
include_HEADERS = audiere.h

if HAVE_FLAC
FLAC_SOURCES = input_flac.cpp input_flac.h
else
FLAC_DIST    = input_flac.cpp input_flac.h
endif

if HAVE_DUMB
DUMB_SOURCES = input_mod.cpp input_mod.h
else
DUMB_DIST    = input_mod.cpp input_mod.h
endif

if HAVE_OGG
OGG_SOURCES = input_ogg.cpp input_ogg.h
else
OGG_DIST    = input_ogg.cpp input_ogg.h
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

DSOUND_SRC = \
	device_ds.cpp device_ds.h \
	device_ds_buffer.cpp device_ds_buffer.h \
	device_ds_stream.cpp device_ds_stream.h \
	dxguid.cpp

if HAVE_DSOUND
DSOUND_SOURCES = $(DSOUND_SRC)
else
DSOUND_DIST = $(DSOUND_SRC)
endif

if HAVE_WINMM
WINMM_SOURCES = device_mm.cpp device_mm.h
else
WINMM_DIST    = device_mm.cpp device_mm.h
endif


EXTRA_DIST = \
	$(FLAC_DIST) $(DUMB_DIST) $(OGG_DIST) $(AL_DIST) $(OSS_DIST) \
	$(DSOUND_DIST) $(WINMM_DIST)

libaudiere_la_SOURCES = \
	basic_source.cpp \
	basic_source.h \
	debug.cpp \
	debug.h \
	default_file.h \
	device.cpp \
	$(DSOUND_SOURCES) \
	device_mixer.cpp \
	device_mixer.h \
	device_null.cpp \
	device_null.h \
	$(AL_SOURCES) \
	$(OSS_SOURCES) \
	$(WINMM_SOURCES) \
	file_ansi.cpp \
	input.cpp \
	$(FLAC_SOURCES) \
	$(DUMB_SOURCES) \
	input_mp3.cpp \
	input_mp3.h \
	$(OGG_SOURCES) \
	input_wav.cpp \
	input_wav.h \
	internal.h \
	memory_file.cpp \
	memory_file.h \
	noise.cpp \
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

libaudiere_la_LDFLAGS =	-release $(VERSION)
libaudiere_la_LIBADD = mpegsound/libmpegsound.la
