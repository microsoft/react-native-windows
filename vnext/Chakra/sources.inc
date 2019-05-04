LIBLETNAME = Chakra

!include $(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\make.inc

INCLUDES = $(INCLUDES); \
	$(OPENSOURCE_CHAKRACOREDEBUGTARGETDIR)\include; \
	$(PKGJAVASCRIPTCORE_TEMP)\build\native\include; \

SOURCES_SHARED = \
	..\ChakraExecutor.cpp \
	..\ChakraHelpers.cpp \
	..\ChakraNativeModules.cpp \
	..\ChakraPlatform.cpp \
	..\ChakraTracing.cpp \
	..\ChakraUtils.cpp \
	..\ChakraValue.cpp \
	..\Utf8DebugExtensions.cpp \

SOURCES_WIN32 = $(SOURCES_SHARED) \
	..\ChakraRuntime.cpp \

SOURCES_WINRT = $(SOURCES_SHARED) \

SOURCES_JSICORE = \
	..\ChakraRuntime.cpp \
