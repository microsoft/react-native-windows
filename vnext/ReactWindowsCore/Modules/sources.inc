LIBLETNAME = Modules

!include $(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\make.inc

SOURCES_SHARED = \
	..\ExceptionsManagerModule.cpp \
	..\I18nModule.cpp \
	..\SourceCodeModule.cpp \
	..\UIManagerModule.cpp \

SOURCES_WIN32 = $(SOURCES_SHARED) \

SOURCES_WINRT = $(SOURCES_SHARED) \
	..\	..\AppStateModule.cpp \
