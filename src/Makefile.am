SUBDIRS = mikmod

INCLUDES = \
	-I mikmod/mmio \
	-I mikmod/mikmod

lib_LTLIBRARIES = libaudiere.la
include_HEADERS = audiere.h

libaudiere_la_SOURCES = \
	audiere.cpp \
	config.h \
	context.cpp \
	context.hpp \
	debug.cpp \
	debug.hpp \
	default_file.hpp \
	endian.hpp \
	file_ansi.cpp \
	file.cpp \
	file.hpp \
	input.cpp \
	input.hpp \
	input_mod.cpp \
	input_mod.hpp \
	input_ogg.cpp \
	input_ogg.hpp \
	input_wav.cpp \
	input_wav.hpp \
	mixer.cpp \
	mixer.hpp \
	output.cpp \
	output.hpp \
	output_al.cpp \
	output_al.hpp \
	output_oss.cpp \
	output_oss.hpp \
	output_null.cpp \
	output_null.hpp \
	repeatable.cpp \
	repeatable.hpp \
	resampler.cpp \
	resampler.hpp \
	stream.cpp \
	stream.hpp \
	threads_posix.cpp \
	threads.hpp \
	timer_posix.cpp \
	timer.hpp \
	utility.cpp \
	utility.hpp

libaudiere_la_LDFLAGS = \
	-release $(VERSION) \
	-lvorbisfile \
	-lvorbis \
	-logg

libaudiere_la_LIBADD = \
	mikmod/mmio/libmmio.la \
	mikmod/mikmod/libmikmod.la
