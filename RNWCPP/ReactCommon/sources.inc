LIBLETNAME = ReactCommonPch
LIBLET_NO_PRECOMP = 1

!include $(WAREHOUSEIMPORTDIR)\libletmake\x-none\nmake\make.inc

SOURCES_SHARED = \
	..\pch.cpp \

SOURCES_WIN32 = $(SOURCES_SHARED) \

SOURCES_WINRT = $(SOURCES_SHARED) \

