XPIDL     = xpidl -IF:/mozilla/dist/idl
XPIDL_H   = $(XPIDL) -m header
XPIDL_XPT = $(XPIDL) -m typelib

build:
	$(XPIDL_H)   mozIAudioService.idl # mozIAudioService.h
	$(XPIDL_H)   mozIAudioContext.idl # mozIAudioContext.h
	$(XPIDL_H)   mozIAudioStream.idl  # mozIAudioStream.h
	$(XPIDL_XPT) mozIAudioService.idl # mozIAudioService.xpt
	$(XPIDL_XPT) mozIAudioContext.idl # mozIAudioContext.xpt
	$(XPIDL_XPT) mozIAudioStream.idl  # mozIAudioStream.xpt
