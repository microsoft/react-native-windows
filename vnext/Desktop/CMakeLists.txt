set(SOURCES
	CxxReactWin32/JSBigString.cpp
	DevSupportManager.cpp
	Executors/WebSocketJSExecutor.cpp
	Executors/WebSocketJSExecutorFactory.cpp
	HttpResource.cpp
	JSBigStringResourceDll.cpp
	LazyDevSupportManager.cpp
 	LegacyWebSocket.cpp
	Modules/NetworkingModule.cpp
	Modules/TimingModule.cpp
	Modules/WebSocketModule.cpp
	Sandbox/DevSupportManagerSandbox.cpp
	Sandbox/SandboxBridge.cpp
	Sandbox/NamedPipeEndpoint.cpp
	Sandbox/SandboxJSExecutor.cpp
	WebSocket.cpp)

add_library(ReactWindowsStatic ${SOURCES})

find_package(Boost REQUIRED COMPONENTS system date_time)

target_include_directories(ReactWindowsStatic PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${REACT_DEP_STUBS})

target_link_libraries(ReactWindowsStatic PUBLIC folly reactcommon reactwindowscore ReactWindowsShared chakra Boost::system Boost::date_time Shlwapi Version)

target_compile_definitions(ReactWindowsStatic PRIVATE JSI_EXPORT)

if(WIN32)
	target_compile_options(ReactWindowsStatic PRIVATE /bigobj)
endif(WIN32)
