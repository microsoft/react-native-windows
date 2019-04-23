set(SOURCES
	TestInstance.cpp
	TestModule.cpp
	TestRootView.cpp
	TestRunner.cpp)

add_library(IntegrationTests ${SOURCES})

target_include_directories(IntegrationTests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(IntegrationTests PRIVATE reactwindowscore)
