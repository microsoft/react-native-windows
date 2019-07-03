set(SOURCES
	InstanceManager.cpp
	Logging.cpp
	OInstance.cpp
	AsyncStorage/StorageFileIO.cpp)

add_library(ReactWindowsShared ${SOURCES})

target_include_directories(ReactWindowsShared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "./../" "./../include/ReactWindowsCore")

#TODO: This looks like a circular dependency (of include directories at least); This isn't really a shared library, but rather a shared folder of code (as used in msbuild)
if(WINRT)
	find_package(CppWinRT REQUIRED)

	target_link_libraries(ReactWindowsShared PRIVATE CppWinRT::CppWinRT)
	target_include_directories(ReactWindowsShared PRIVATE "./../ReactUWP")
else(WINRT)
	target_include_directories(ReactWindowsShared PRIVATE "./../Desktop")
endif(WINRT)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

target_link_libraries(ReactWindowsShared PUBLIC folly reactcommon reactwindowscore chakra Boost::boost OpenSSL::SSL OpenSSL::Crypto)

#target_compile_definitions(ReactWindowsShared PUBLIC USE_V8)

if(WINRT)
	target_compile_definitions(ReactWindowsShared PRIVATE UNICODE=1 _UNICODE=1)
	target_compile_definitions(ReactWindowsShared PUBLIC WIN32=0 WINRT=1 BOOST_ASIO_WINDOWS_APP BOOST_BEAST_USE_WIN32_FILE=0)
else(WINRT)
	target_compile_definitions(ReactWindowsShared PUBLIC BOOST_ASIO_HAS_IOCP _WIN32_WINNT=0x0601 _WINSOCK_DEPRECATED_NO_WARNINGS REACTNATIVEWIN32_EXPORTS)
endif(WINRT)
