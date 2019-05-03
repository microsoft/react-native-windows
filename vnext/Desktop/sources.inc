LIBLETNAME = ReactWin32

!include $(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\make.inc

INCLUDES = $(INCLUDES); \
	$(OPENSOURCE_CHAKRACOREDEBUGTARGETDIR)\include; \
	$(PKGJAVASCRIPTCORE_TEMP)\build\native\include; \
	$(PKGBOOST)\lib\native\include; \
	$(PKGMICROSOFT_CHAKRACORE_VC140)\build\native\include; \

SOURCES_SHARED = \
	..\DevSupportManager.cpp \
	..\HttpResource.cpp \
	..\JsBigStringResourceDll.cpp \
	..\LazyDevSupportManager.cpp \
	..\LegacyWebSocket.cpp \
	..\TimingModule.cpp \
	..\WebSocket.cpp \
	..\WebSocketModule.cpp \
	..\WebSocketJSExecutorFactory.cpp \
	..\WebSocketJSExecutor.cpp \

SOURCES_WIN32 = $(SOURCES_SHARED) \
