##
## mad - MPEG audio decoder
## Copyright (C) 2000-2001 Robert Leslie
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##
## If you would like to negotiate alternate licensing terms, you may do
## so by contacting the author: Robert Leslie <rob@mars.org>
##
## $Id$
##

## Process this file with automake to produce Makefile.in

SUBDIRS =		intl po libmad winamp

bin_PROGRAMS =		madplay
bin_SCRIPTS =		abxtest
EXTRA_PROGRAMS =	madtime minimad madmix mad123

man_MANS =		madplay.1 abxtest.1

EXTRA_DIST =  \
	$(man_MANS) $(bin_SCRIPTS)  \
	CHANGES COPYRIGHT CREDITS README TODO VERSION

default_audio =  \
	audio.c audio_raw.c audio_wave.c audio_snd.c audio_null.c audio.h

extra_audio =  \
	audio_empeg.c audio_hex.c audio_oss.c audio_sun.c audio_win32.c

common_sources =  \
	version.c version.h global.h

madplay_SOURCES =  \
	$(common_sources) $(default_audio)  \
	resample.c filter.c equalizer.c id3.c id3v2.c player.c madplay.c  \
	resample.h filter.h equalizer.h id3.h id3v2.h player.h  \
	id3genre.dat getopt.c getopt1.c getopt.h

EXTRA_madplay_SOURCES =  \
	$(extra_audio)  \
	id3v2.gperf

madtime_SOURCES =	$(common_sources) madtime.c
minimad_SOURCES =	minimad.c
mad123_SOURCES =	mad123.c

madmix_SOURCES =	$(common_sources) madmix.c $(default_audio)
EXTRA_madmix_SOURCES =	$(extra_audio)

AUDIO =			@AUDIO@

LDADD =			@LIBOBJS@ libmad/libmad.la @INTLLIBS@

madplay_DEPENDENCIES =	$(AUDIO) $(LDADD)
madplay_LDADD =		$(AUDIO) $(LDADD)

madmix_DEPENDENCIES =	$(AUDIO) $(LDADD)
madmix_LDADD =		$(AUDIO) $(LDADD)

localedir =		$(datadir)/locale
INCLUDES =		-Ilibmad -Iintl -DLOCALEDIR=\"$(localedir)\"

#CFLAGS =	-Wall @CFLAGS@ $(ARCH) $(DEBUGGER) $(OPTIMIZER) $(PROFILER)
#LDFLAGS =	@LDFLAGS@ $(PROFILER)

BUILT_SOURCES =		id3v2.c

CLEANFILES =		$(EXTRA_PROGRAMS) *.exe gmon.out

$(srcdir)/id3v2.c: $(srcdir)/id3v2.gperf
	gperf -tCTonD -K id -N id3v2_hash -s -3 -k '*' $< |  \
	sed -e '/\$$''Id: /s/\$$//g' >$@

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

profile: madplay gmon.out
	gprof madplay | less

gmon.out: madplay
	./madplay -vo pcm:/dev/null test/profile.mpg

.c.s:
	$(COMPILE) -S $<

again: clean all

.PHONY: profile again
