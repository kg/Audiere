SUBDIRS = resfilter

INCLUDES = \
	-I .. \
	-I ../../mmio

noinst_LTLIBRARIES = libvirtch.la

libvirtch_la_SOURCES =				\
	nc16.c					\
	nc16ss.c				\
	nc8.c					\
	nc8ss.c					\
	placebo.c				\
	regmix_ss.c				\
	ssmix.h					\
	stdmix.h				\
	vc16.c					\
	vc16ss.c				\
	vc8.c					\
	vc8ss.c					\
	vchcrap.h				\
	virtch.c				\
	wrap16.c				\
	wrap16.h				\
	wrap8.c					\
	wrap8.h

libvirtch_la_LIBADD = \
	resfilter/libresfilter.la
