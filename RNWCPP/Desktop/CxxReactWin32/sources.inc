LIBLETNAME = CxxReactWin32

!include $(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\make.inc

SOURCES_SHARED = \
	..\JSBigString.cpp \

SOURCES_WIN32 = $(SOURCES_SHARED) \
