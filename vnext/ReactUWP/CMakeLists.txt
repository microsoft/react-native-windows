set(SOURCES
	Pch/pch.cpp
	ABI/ABIModuleLoader.cpp
	ABI/HStringHelper.cpp
	ABI/idl/Module.idl
	ABI/idl/Instance.idl
	ABI/idl/ReactControl.idl
	ABI/Instance_rt.cpp
	ABI/ReactControl_rt.cpp
	Base/ChakraJSIRuntimeHolder.cpp
	Base/InstanceFactory.cpp
	Base/UwpReactInstance.cpp
	CxxReactUWP/JSBigString.cpp
	EndPoints/dll/dllmain.cpp
	EndPoints/dll/JSCGlue.cpp
	Executors/WebSocketJSExecutorFactory.cpp
	Executors/WebSocketJSExecutorUwp.cpp
	Modules/AppStateModuleUwp.cpp
	Modules/ClipboardModule.cpp
	Modules/DeviceInfoModule.cpp
	Modules/DevSupportManagerUwp.cpp
	Modules/I18nModule.cpp
	Modules/LinkingManagerModule.cpp
	Modules/LocationObserverModule.cpp
	Modules/NativeUIManager.cpp
	Modules/NetworkingModule.cpp
	Modules/TimingModule.cpp
	Modules/WebSocketModuleUwp.cpp
	Polyester/ButtonContentViewManager.cpp
	Polyester/ButtonViewManager.cpp
	Polyester/ContentControlViewManager.cpp
	Polyester/HyperlinkViewManager.cpp
	Polyester/IconViewManager.cpp
	Sandbox/DevSupportManagerUwpSandbox.cpp
	Threading/AsyncWorkQueue.cpp
	Threading/JSQueueThread.cpp
	Threading/UIMessageQueueThread.cpp
	Threading/WorkerMessageQueueThread.cpp
	Utils/LocalBundleReader.cpp
	Utils/ValueUtils.cpp
	Utils/AccessibilityUtils.cpp
	Views/ActivityIndicatorViewManager.cpp
	Views/CheckboxViewManager.cpp
	Views/ControlViewManager.cpp
	Views/DatePickerViewManager.cpp
	Views/FlyoutViewManager.cpp
  	Views/MenuFlyoutViewManager.cpp
	Views/MenuFlyoutSubItemViewManager.cpp
	Views/MenuFlyoutItemViewManager.cpp
	Views/FrameworkElementViewManager.cpp
	Views/ImageViewManager.cpp
	Views/Impl/ScrollViewUWPImplementation.cpp
	Views/Impl/SnapPointManagingContentControl.cpp
	Views/PickerViewManager.cpp
	Views/PopupViewManager.cpp
	Views/RawTextViewManager.cpp
	Views/ReactControl.cpp
	Views/ReactRootView.cpp
	Views/RootViewManager.cpp
	Views/ScrollViewManager.cpp
	Views/ShadowNodeBase.cpp
	Views/SwitchViewManager.cpp
	Views/TextInputViewManager.cpp
	Views/TextViewManager.cpp
	Views/TouchEventHandler.cpp
	Views/ViewManagerBase.cpp
	Views/ViewControl.cpp
	Views/ViewPanel.cpp
	Views/ViewViewManager.cpp
	Views/VirtualTextViewManager.cpp
	Views/WebViewManager.cpp)

add_library(React.UWPStatic ${SOURCES} ${DEF_FILE})

add_definitions(-DRN_PLATFORM=uwp)

target_compile_definitions(React.UWPStatic PRIVATE UNICODE _UNICODE JSI_EXPORT REACTWINDOWS_BUILD REACTWINDOWS_STATIC)
target_compile_options(React.UWPStatic PRIVATE /await /wd4996)

target_include_directories(React.UWPStatic PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../shared")
target_include_directories(React.UWPStatic PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ABI/idl")

target_include_directories(React.UWPStatic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories(React.UWPStatic PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include/ReactWindowsCore")
target_include_directories(React.UWPStatic PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include/ReactUWP")

# In theory, setting this would allow building ReactUWP and ReactWin32 at the same time (with the same generator), but I didn't find the magical genie incantation required to make that work; I suspect this never actually worked
#set_property(TARGET React.UWPStatic PROPERTY VS_WINRT_COMPONENT TRUE)

target_link_libraries(React.UWPStatic PRIVATE reactcommon reactwindowscore ReactWindowsShared chakra folly RuntimeObject Version)

# Now create a DLL that links the static library

set(DEF_FILE Endpoints/dll/react-native-uwp.${WIN32_BUILD_ARCH}.def)
if(WIN32_BUILD_ARCH MATCHES x86)
  #The MsBuild-based build is hacked together to force __stdcall everywhere; until we figure out a proper fix (starting with the hacks in our Folly clone), we need to use a corrected .def file for x86
  set(DEF_FILE Endpoints/dll/react-native-uwp.${WIN32_BUILD_ARCH}sdx.def)
endif()

add_library(React.UWP SHARED ${DEF_FILE})

target_link_libraries(React.UWP PRIVATE React.UWPStatic)
