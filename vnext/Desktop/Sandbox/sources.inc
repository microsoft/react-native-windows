LIBLETNAME = Sandbox

!include $(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\make.inc

INCLUDES = $(INCLUDES); \
	$(PKGJAVASCRIPTCORE_TEMP)\build\native\include; \
	$(PKGBOOST)\lib\native\include; \

SOURCES_SHARED = \
	..\DevSupportManagerSandbox.cpp \
	..\NamedPipeEndpoint.cpp \
	..\SandboxBridge.cpp \
	..\SandboxJSExecutor.cpp \

SOURCES_WIN32 = $(SOURCES_SHARED) \
