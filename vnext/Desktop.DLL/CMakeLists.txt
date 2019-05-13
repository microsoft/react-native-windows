set(DEF_FILE react-native-win32.${WIN32_BUILD_ARCH}.def)
if(WIN32_BUILD_ARCH MATCHES x86)
	#The MsBuild-based build is hacked together to force __stdcall everywhere; until we figure out a proper fix (starting with the hacks in our Folly clone), we need to use a corrected .def file for x86
	set(DEF_FILE react-native-win32.${WIN32_BUILD_ARCH}sdx.def)
endif()

add_library(react-native-win32 SHARED pch.cpp ${DEF_FILE})

target_link_libraries(react-native-win32 PRIVATE ReactWindowsStatic)
