SUBDIRS = resfilter

INCLUDES = \
	-I .. \
	-I ../../mmio

noinst_LTLIBRARIES = libvirtch.la
libvirtch_la_SOURCES = \
	nc16.c \
	nc8.c \
	placebo.c \
	vc16.c \
	vc8.c \
	virtch.c \
	wrap16.c \
	wrap8.c

libvirtch_la_LIBADD = \
	resfilter/libresfilter.la
