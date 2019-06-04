set(SOURCES
  ChakraExecutor.cpp
	ChakraHelpers.cpp
	ChakraNativeModules.cpp
	ChakraPlatform.cpp
	ChakraJsiRuntime.cpp
	ChakraJsiRuntime_core.cpp
	ChakraJsiRuntime_edgemode.cpp
	ChakraTracing.cpp
	ChakraUtils.cpp
	ChakraValue.cpp
	Utf8DebugExtensions.cpp)

add_library(chakra ${SOURCES})

target_include_directories(chakra PRIVATE ${REACT_DEP_STUBS})

if(NOT WINRT)
	find_package(ChakraCore REQUIRED)
	find_package(ChakraCoreDebugger REQUIRED)

	target_link_libraries(chakra PUBLIC ChakraCore::ChakraCore ChakraCoreDebugger::ChakraCoreDebugger)

	ChakraCore_copy_dll(chakra)
endif(NOT WINRT)

if(WIN32)
	target_compile_options(chakra PUBLIC /wd4290)
endif(WIN32)

target_link_libraries(chakra PUBLIC folly reactcommon reactwindowscore)

target_compile_definitions(chakra PRIVATE UNICODE=1 _UNICODE=1 JSI_EXPORT=)

if(WINRT)
	target_compile_definitions(chakra PUBLIC USE_EDGEMODE_JSRT)
endif(WINRT)
