SUBDIRS = loaders virtch

INCLUDES = \
	-I ../mmio

noinst_LTLIBRARIES = libmikmod.la
libmikmod_la_SOURCES = \
	drv_nos.c \
	mdriver.c \
	mloader.c \
	mplayer.c \
	munitrk.c \
	npertab.c \
	sloader.c \
	unimod.c \
	voiceset.c \
	mdsfx.h \
	mikmod.h \
	mpforbid.h \
	mplayer.h \
	mwav.h \
	uniform.h \
	virtch.h

libmikmod_la_LIBADD = \
	loaders/libloaders.la \
	virtch/libvirtch.la
