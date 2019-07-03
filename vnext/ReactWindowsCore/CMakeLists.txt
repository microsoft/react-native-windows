set(SOURCES
	AsyncStorage/AsyncStorageManager.cpp
	AsyncStorage/FollyDynamicConverter.cpp
	AsyncStorage/KeyValueStorage.cpp
	Modules/AppStateModule.cpp
	Modules/AsyncStorageModule.cpp
	Modules/ExceptionsManagerModule.cpp
	Modules/I18nModule.cpp
	Modules/SourceCodeModule.cpp
	Modules/UIManagerModule.cpp
	CxxMessageQueue.cpp
	JSBigAbiString.cpp
	LayoutAnimation.cpp
	MemoryTracker.cpp
	ShadowNode.cpp
	ShadowNodeRegistry.cpp
	unicode.cpp
	Utils.cpp
	ViewManager.cpp)

add_library(reactwindowscore ${SOURCES})

target_include_directories(reactwindowscore PRIVATE ${REACT_DEP_STUBS})
target_include_directories(reactwindowscore PRIVATE "./../shared" ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(reactwindowscore PUBLIC "./../include" ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(reactwindowscore PUBLIC "./../include/ReactWindowsCore" ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(reactwindowscore PUBLIC folly reactcommon)
