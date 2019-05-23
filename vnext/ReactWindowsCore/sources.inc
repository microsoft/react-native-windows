LIBLETNAME = ModuleProvider

!include $(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\make.inc

C_DEFINES = $(C_DEFINES) -D NOJSC

SOURCES_SHARED = \
	..\CxxMessageQueue.cpp \
	..\JSBigAbiString.cpp \
	..\LayoutAnimation.cpp \
	..\ShadowNode.cpp \
	..\ShadowNodeRegistry.cpp \
	..\unicode.cpp \
	..\Utils.cpp \
	..\ViewManager.cpp \

SOURCES_UNICONV = \
	..\unicode.cpp \

SOURCES_WIN32 = $(SOURCES_SHARED) \

SOURCES_WINRT = $(SOURCES_SHARED) \
