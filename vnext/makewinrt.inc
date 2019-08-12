###########################################################
#
#  Common include file for NMAKE
#  Need to keep consistent with the reactUWP.vcxproj
#
###########################################################

LIBLET_WINRT = 1
LIBLET_NO_PRECOMP = 1

!include $(WAREHOUSEIMPORTDIR)\libletmake\x-none\nmake\make.inc

INCLUDES = $(INCLUDES); \
	$(ATLINCDIR); \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\Folly; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactCommon; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactCommon\Yoga; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\stubs; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\shared; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\include; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\include\ReactWindowsCore; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\ReactWindowsCore; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\ReactUWP; \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactWindows\ReactUWP\Pch; \
	$(PKGJAVASCRIPTCORE_TEMP)\build\native\include; \
	$(PKGBOOST)\lib\native\include; \
	$(PKGMICROSOFT_CHAKRACORE_VC140)\build\native\include; \
	$(OTOOLSINCROOT)\cppwinrt; \

!LISTFILES -recursive JAVASCRIPTCORE_TEMP_HEADERS = $(PKGJAVASCRIPTCORE_TEMP) *.h
!LISTFILES -recursive BOOST_HEADERS = $(PKGBOOST) *

HEADERS = $(HEADERS) \
	$(JAVASCRIPTCORE_TEMP_HEADERS) \
	$(BOOST_HEADERS) \

CPP_ENABLE_STD_17=1
CPP_ENABLE_AWAIT=1

C_DEFINES = $(C_DEFINES) -D REACTWINDOWS_BUILD
C_DEFINES = $(C_DEFINES) -D RN_PLATFORM=windows
C_DEFINES = $(C_DEFINES) -D USE_EDGEMODE_JSRT
C_DEFINES = $(C_DEFINES) -D NOMINMAX
C_DEFINES = $(C_DEFINES) -D FOLLY_NO_CONFIG
C_DEFINES = $(C_DEFINES) -D RN_EXPORT=
C_DEFINES = $(C_DEFINES) -D JSI_EXPORT=
C_DEFINES = $(C_DEFINES) -D WIN32=0
C_DEFINES = $(C_DEFINES) -D WINRT=1
C_DEFINES = $(C_DEFINES) -D _HAS_AUTO_PTR_ETC


C_FLAGS = $(C_FLAGS) -wd4068 -wd4189 -wd4100 -wd4239 -wd4458 -wd4244 -wd4456
C_FLAGS = $(C_FLAGS) /bigobj
