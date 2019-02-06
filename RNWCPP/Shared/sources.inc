LIBLETNAME = ReactWinShared

INCLUDES = $(INCLUDES); \
	$(OPENSOURCE_CHAKRACOREDEBUGTARGETDIR)\include; \
	$(PKGBOOST)\lib\native\include; \
	$(PKGJAVASCRIPTCORE_TEMP)\build\native\include; \

SOURCES_SHARED = \
	..\AsyncStorageModule.cpp \
	..\InstanceManager.cpp \
	..\Logging.cpp \
	..\MemoryTracker.cpp \
	..\OInstance.cpp \

SOURCES_WIN32 = $(SOURCES_SHARED) \

SOURCES_WINRT = $(SOURCES_SHARED) \
