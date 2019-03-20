LIBLETNAME = AsyncStorage

!include $(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\make.inc

SOURCES_SHARED = \
	..\AsyncStorageManager.cpp \
	..\FollyDynamicConverter.cpp \
	..\KeyValueStorage.cpp \
	..\StorageFileIO.cpp \

SOURCES_WIN32 = $(SOURCES_SHARED) \

SOURCES_WINRT = $(SOURCES_SHARED) \
