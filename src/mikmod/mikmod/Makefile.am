SUBDIRS = loaders virtch

INCLUDES = \
	-I $(top_srcdir)/src/mikmod/mmio

noinst_LTLIBRARIES = libmikmod.la

libmikmod_la_SOURCES =				\
	drv_nos.c				\
	mdriver.c				\
	mdsfx.c					\
	mdsfx.h                                 \
	mikmod.h				\
	mloader.c				\
	mlreg.c					\
	mpforbid.h				\
	mplayer.c				\
	mplayer.h				\
	munitrk.c				\
	mwav.c					\
	mwav.h					\
	npertab.c				\
	sloader.c				\
	snglen.c				\
	uniform.h				\
	unimod.c				\
	virtch.h				\
	voiceset.c

libmikmod_la_LIBADD = \
	loaders/libloaders.la \
	virtch/libvirtch.la
