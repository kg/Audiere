EXTRA_DIST = documentation

lib_LTLIBRARIES = libaudiere.la

include_HEADERS = \
	audiere.h

libaudiere_la_SOURCES = \
	config.h \
	audiere.cpp \
	file_ansi.cpp \
	file.hpp \
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
	resampler.cpp \
	resampler.hpp \
	threads_posix.cpp \
	threads.hpp

libaudiere_la_LDFLAGS = \
	-release $(VERSION)
