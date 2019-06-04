LIBLETNAME = react-native-win32
LIBLET_NO_PRECOMP = 1

!include $(WAREHOUSEIMPORTDIR)\libletmake\x-none\nmake\make.inc

INCLUDES = $(INCLUDES); \
	$(OPENSOURCE_REACTNATIVE)\OfficeISS\ReactCommon\yoga; \

SOURCES_SHARED = \

!if "$(PLATFORM)" == "x64"
DLLDEFLIST = $(DLLDEFLIST) \
	..\Desktop.DLL\react-native-win32.x64.def \
!endif

!if "$(PLATFORM)" == "x86" || "$(PLATFORM)" == "chpe"
LINK_GENERATES_LIB=1
DLLDEFLIST = $(DLLDEFLIST) \
	..\Desktop.DLL\react-native-win32.x86.def \
!endif

# chpe needs this
!if "$(ENABLE_HYBRID)" == "1"
USE_REAL_OBJS_FOR_IMPORTLIB = 1
!endif

TARGETLIBS = $(TARGETLIBS) \
	$(CORELIBDIR)\Advapi32.lib \
	$(CORELIBDIR)\Bcrypt.lib \
	$(CORELIBDIR)\Kernel32.lib \
	$(CORELIBDIR)\MsWSock.lib \
	$(CORELIBDIR)\Shell32.lib \
	$(CORELIBDIR)\Shlwapi.lib \
	$(CORELIBDIR)\Version.lib \
	$(CORELIBDIR)\Ws2_32.lib \
	$(PKGMICROSOFT_CHAKRACORE_VC140)\lib\native\v140\$(PLATFORM:chpe=x86)\$(FLAVOR:ship=release)\ChakraCore.lib \
	$(PKGJAVASCRIPTCORE_TEMP)\lib\native\$(PLATFORM:chpe=x86)\JavaScriptCore.lib \
	$(OPENSOURCE_OPENSSLTARGETLIBDIR)\libeay32.lib \
	$(OPENSOURCE_OPENSSLTARGETLIBDIR)\ssleay32.lib \
	$(PKGOFFICE_GOOGLE_V8_1_6_0_0)\lib\$(PLATFORM:chpe=x86)\$(FLAVOR:ship=release)\v8.dll.lib \

TARGETLOBS = $(TARGETLOBS) \
	$(OPENSOURCE_BOOSTTARGETLIBDIR)\boost_date_time.lob \
	$(OPENSOURCE_CHAKRACOREDEBUGTARGETLIBDIR)\ChakraCore.Debugger.Protocol.GeneratedWin32.lob \
	$(OPENSOURCE_CHAKRACOREDEBUGTARGETLIBDIR)\ChakraCore.Debugger.ProtocolWin32.lob \
	$(OPENSOURCE_CHAKRACOREDEBUGTARGETLIBDIR)\ChakraCore.Debugger.ProtocolHandlerWin32.lob \
	$(OPENSOURCE_CHAKRACOREDEBUGTARGETLIBDIR)\ChakraCore.Debugger.ServiceWin32.lob \
	$(PROJLIBDIR)\AsyncStorageWin32.lob \
	$(PROJLIBDIR)\ChakraWin32.lob \
	$(PROJLIBDIR)\CxxReactWin32.lob \
	$(PROJLIBDIR)\CxxReactWin32Win32.lob \
	$(PROJLIBDIR)\FollyDetailWin32.lob \
	$(PROJLIBDIR)\FollyHashWin32.lob \
	$(PROJLIBDIR)\FollyLangWin32.lob \
	$(PROJLIBDIR)\FollyMemoryDetailWin32.lob \
	$(PROJLIBDIR)\FollyPortabilityWin32.lob \
	$(PROJLIBDIR)\FollyWin32.lob \
	$(PROJLIBDIR)\jsiWin32.lob \
	$(PROJLIBDIR)\jsireactWin32.lob \
	$(PROJLIBDIR)\ModuleProviderWin32.lob \
	$(PROJLIBDIR)\ModulesWin32.lob \
	$(PROJLIBDIR)\PrivateDataWin32.lob \
	$(PROJLIBDIR)\ReactCommonPchWin32.lob \
	$(PROJLIBDIR)\ReactWinSharedWin32.lob \
	$(PROJLIBDIR)\SandboxWin32.lob \
	$(PROJLIBDIR)\YogaWin32.lob \
	$(PROJLIBDIR)\ReactWin32Win32.lob \

!ifndef NOJSC
DELAYLOAD = \
	$(DELAYLOAD) \
	JavaScriptCore.dll; \
!endif

DELAYLOAD = \
	$(DELAYLOAD) \
	v8.dll; \

