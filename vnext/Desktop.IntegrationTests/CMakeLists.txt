set(SOURCES
	DesktopTestInstance.cpp
	HttpResourceIntegrationTests.cpp
	RNTesterIntegrationTests.cpp
	TestMessageQueueThread.cpp
	DesktopTestRunner.cpp
	WebSocketIntegrationTest.cpp
	WebSocketJSExecutorIntegrationTest.cpp
	WebSocketModuleIntegrationTest.cpp)

add_library(ReactWindows.Desktop.IntegrationTests SHARED ${SOURCES})

find_package(VSCppUnitTest REQUIRED)

target_include_directories(ReactWindows.Desktop.IntegrationTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${VC_UNITTEST_INCLUDE_DIRS} "${CMAKE_CURRENT_SOURCE_DIR}/../Desktop")

# This is needed because the CppUnitTest headers use a #pragma lib to bring this in through a hard-coded path
set_target_properties(ReactWindows.Desktop.IntegrationTests PROPERTIES LINK_FLAGS "/NODEFAULTLIB:${WIN32_BUILD_ARCH}\\Microsoft.VisualStudio.TestTools.CppUnitTestFramework.lib")

target_link_libraries(ReactWindows.Desktop.IntegrationTests PRIVATE ReactWindowsStatic IntegrationTests ${VC_UNITTEST_LIB})

# This test hangs, even after it's fixed to use the dll path from arguments. I don't think this belongs in the unit test category (it should take milliseconds to run), perhaps we can add an "integration tests" section
#add_test(NAME Desktop.IntegrationTests.Run COMMAND node "${CMAKE_SOURCE_DIR}/react-native-win/scripts/run-desktop-integration-tests.js" "$<TARGET_FILE:ReactWindows.Desktop.IntegrationTests>" )
