###########################################################
#
#  Common include file for NMAKE
#
###########################################################

LIBLET_NO_PRECOMP = 1

!include $(WAREHOUSEIMPORTDIR)\libletmake\x-none\nmake\make.inc

INCLUDES = $(INCLUDES); \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\Folly; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactCommon; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\stubs; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\shared; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\include; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\include\ReactWindowsCore; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\Desktop; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\ReactWindowsCore; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\ChakraCoreShim\ChakraHeaders; \
	$(OPENSOURCE_OPENSSL); \

!LISTFILES -recursive JAVASCRIPTCORE_TEMP_HEADERS = $(PKGJAVASCRIPTCORE_TEMP) *.h
!LISTFILES -recursive BOOST_HEADERS = $(PKGBOOST) *

HEADERS = $(HEADERS) \
	$(JAVASCRIPTCORE_TEMP_HEADERS) \
	$(BOOST_HEADERS) \

C_DEFINES = $(C_DEFINES) -D REACTWINDOWS_BUILD
C_DEFINES = $(C_DEFINES) -D FOLLY_NO_CONFIG
C_DEFINES = $(C_DEFINES) -D RN_EXPORT=
C_DEFINES = $(C_DEFINES) -D JSI_EXPORT=
C_DEFINES = $(C_DEFINES) -D RN_JSC_EXECUTOR_EXPORT=__declspec(dllexport)
C_DEFINES = $(C_DEFINES) -D RN_PLATFORM=win32
C_DEFINES = $(C_DEFINES) -D NOMINMAX

NOJSC = 1;
C_DEFINES = $(C_DEFINES) -D NOJSC

# Avoid dependencies on pre-built Boost.System LIB.
C_DEFINES = $(C_DEFINES) -D BOOST_SYSTEM_SOURCE
C_DEFINES = $(C_DEFINES) -D BOOST_ERROR_CODE_HEADER_ONLY

!if "$(PLATFORM)" == "x86" || "$(PLATFORM)" == "x64"
# Set to 1 for more verbose tracing.
C_DEFINES = $(C_DEFINES) -D WITH_OFFICE_TRACING=0
!endif

C_FLAGS = $(C_FLAGS) -wd4146 -wd4068 -wd4273 -wd4244 -wd4100 -wd4275 -wd4251 -wd4290 -wd4003 -wd4018 -wd4101 -wd4267 -wd4005 -wd4189 -wd4309 -wd4127 -wd4293 -wd4239 -wd4311 -wd4302 -wd4389 -wd4541 -wd4324
C_FLAGS = $(C_FLAGS) /bigobj
