lib_LTLIBRARIES = libaudiere.la

include_HEADERS = \
	audiere.h

libaudiere_la_SOURCES = \
	config.h \
	audiere.cpp \
	file_ansi.cpp \
	file.hpp \
	output.cpp \
	output.hpp \
	@AL_SOURCES@ \
	output_oss.cpp \
	output_oss.hpp \
	output_null.cpp \
	output_null.hpp \
	threads_posix.cpp \
	threads.hpp

libaudiere_la_LDFLAGS = \
	-release $(VERSION)
