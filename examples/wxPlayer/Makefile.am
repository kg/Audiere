INCLUDES = -I $(top_srcdir)/src

SRC = \
	Commands.h \
	DeviceFrame.cpp \
	DeviceFrame.h \
	EditLoopPointsDialog.cpp \
	EditLoopPointsDialog.h \
	NewDeviceDialog.cpp \
	NewDeviceDialog.h \
	SoundEffectFrame.cpp \
	SoundEffectFrame.h \
	StreamFrame.cpp \
	StreamFrame.h \
	resource.h \
	wx.h \
	wxPlayer.cpp \
	wxPlayer.h

if HAVE_WXWINDOWS

bin_PROGRAMS = wxPlayer
wxPlayer_LDADD = $(top_builddir)/src/libaudiere.la
wxPlayer_CXXFLAGS = @WX_CPPFLAGS@
wxPlayer_LDFLAGS = @WX_LIBS@

wxPlayer_SOURCES = $(SRC)

else

EXTRA_DIST = $(SRC)

endif
