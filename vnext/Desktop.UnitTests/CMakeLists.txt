set(SOURCES
	AsyncStorageManagerTest.cpp
	AsyncStorageTest.cpp
	EmptyUIManagerModule.cpp
	LayoutAnimationTests.cpp
	UIManagerModuleTest.cpp
	UtilsTest.cpp
	WebSocketJSExecutorTest.cpp
	WebSocketModuleTest.cpp
	WebSocketTest.cpp)

add_library(ReactWindows.Desktop.UnitTests SHARED ${SOURCES})

find_package(VSCppUnitTest REQUIRED)

target_include_directories(ReactWindows.Desktop.UnitTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${VC_UNITTEST_INCLUDE_DIRS} "./../Desktop")

# This is needed because the CppUnitTest headers use a #pragma lib to bring this in through a hard-coded path
set_target_properties(ReactWindows.Desktop.UnitTests PROPERTIES LINK_FLAGS "/NODEFAULTLIB:${WIN32_BUILD_ARCH}\\Microsoft.VisualStudio.TestTools.CppUnitTestFramework.lib")

target_link_libraries(ReactWindows.Desktop.UnitTests PRIVATE ReactWindowsStatic ${VC_UNITTEST_LIB})

# Use a custom cmake script to enable vstest - based tests
add_test(NAME ReactWindows.Desktop.UnitTests COMMAND ${CMAKE_COMMAND} -DVS_TEST_RUNNER=${VS_TEST_RUNNER}
	-DTEST_DLL=$<TARGET_FILE:ReactWindows.Desktop.UnitTests> -P "${CMAKE_SOURCE_DIR}/../cmake/runvstest.cmake")
