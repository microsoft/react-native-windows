set(SOURCE_FILES
	Tests/AppStateModuleTests.cpp
	Tests/CreateModulesTests.cpp
	Tests/CreateViewManagersTests.cpp
	Tests/StringConversionTests_Universal.cpp
  App.xaml.cpp
  MainPage.xaml.cpp
  pch.cpp)

set(HEADER_FILES
  App.xaml.h
  MainPage.xaml.h
  pch.h)

set(XAML_FILES
  App.xaml
  MainPage.xaml)

set(ASSET_FILES
	Assets/LockScreenLogo.scale-200.png
	Assets/SplashScreen.scale-200.png
	Assets/Square150x150Logo.scale-200.png
	Assets/Square44x44Logo.scale-200.png
	Assets/Square44x44Logo.targetsize-24_altform-unplated.png
	Assets/StoreLogo.png
	Assets/Wide310x150Logo.scale-200.png)

set(CONTENT_FILES
    Package.appxmanifest
    )

set(RESOURCE_FILES
  ${CONTENT_FILES} ${ASSET_FILES}
	React.Windows.Universal.UnitTests_TemporaryKey.pfx)

set_property(SOURCE ${CONTENT_FILES} PROPERTY VS_DEPLOYMENT_CONTENT 1)
set_property(SOURCE ${ASSET_FILES} PROPERTY VS_DEPLOYMENT_CONTENT 1)
set_property(SOURCE ${ASSET_FILES} PROPERTY VS_DEPLOYMENT_LOCATION "Assets")

set_property(SOURCE "App.xaml" PROPERTY VS_XAML_TYPE "ApplicationDefinition")

source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})
source_group("Resource Files" FILES ${RESOURCE_FILES})
source_group("Xaml Files" FILES ${XAML_FILES})

add_executable(ReactWindows.Universal.UnitTests WIN32 ${SOURCE_FILES} ${HEADER_FILES} ${RESOURCE_FILES} ${XAML_FILES})
set_property(TARGET ReactWindows.Universal.UnitTests PROPERTY VS_WINRT_COMPONENT TRUE)

find_package(VSCppUnitTest REQUIRED)

add_definitions(-DREACTWINDOWS_STATIC)

# The Visual Studio builds are a complete spaghetti crapshoot of interdependencies; please let's kill it so we can start cleaning up the code
target_include_directories(ReactWindows.Universal.UnitTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../shared")
target_include_directories(ReactWindows.Universal.UnitTests PRIVATE ${VC_UNITTEST_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}
	"./../ReactUWP"
	"./../ReactWindowsCore"
	"./../include"
	"./../include/ReactUWP")

# This is needed because the CppUnitTest headers use a #pragma lib to bring this in through a hard-coded path
set_target_properties(ReactWindows.Universal.UnitTests PROPERTIES LINK_FLAGS "/NODEFAULTLIB:${WIN32_BUILD_ARCH}\\Microsoft.VisualStudio.TestTools.CppUnitTestFramework.lib")

target_link_libraries(ReactWindows.Universal.UnitTests PRIVATE reactcommon React.UWPStatic ${VC_UNITTEST_LIB})

target_compile_definitions(ReactWindows.Universal.UnitTests PRIVATE USE_EDGEMODE_JSRT)

# Use a custom cmake script to enable vstest - based tests

# Turning this off until we figure out how to actually run UWP unit tests from the command line

#add_test(NAME ReactWindows.Universal.UnitTests COMMAND ${CMAKE_COMMAND} -DVS_TEST_RUNNER=${VS_TEST_RUNNER}
#	-DTEST_DLL=$<TARGET_FILE:ReactWindows.Universal.UnitTests> -P "${CMAKE_SOURCE_DIR}/../cmake/runvstest.cmake")
