INCLUDES = -I $(top_srcdir)/src

if HAVE_WXWINDOWS

bin_PROGRAMS = wxPlayer
wxPlayer_SOURCES = wxPlayer.cpp
wxPlayer_LDADD = $(top_builddir)/src/libaudiere.la
wxPlayer_CXXFLAGS = @WX_CPPFLAGS@
wxPlayer_LDFLAGS = @WX_LIBS@

else

EXTRA_DIST = wxPlayer.cpp

endif
