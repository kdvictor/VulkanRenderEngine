cmake_minimum_required(VERSION 3.20)

project(OGLBF)

#set c++ properties
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set _DEBUG FLAG
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	add_definitions(-D_DEBUG)
endif()

#build thirdParty libraries
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/thirdParty")

#main sources
file(GLOB MAIN_SRCS "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
message("main srcs: ${MAIN_SRCS}")

#thirdParty sources
file(GLOB THIRDPARTY_SOURCES "${CMAKE_CURRENT_LIST_DIR}/thirdParty/src/*")
message("thirdyParty srcs: ${THIRDPARTY_SOURCES}")

#set include dir
include_directories(
"${CMAKE_CURRENT_LIST_DIR}/thirdParty/include"
"${CMAKE_CURRENT_LIST_DIR}/asserts/include"
"${CMAKE_CURRENT_LIST_DIR}/framework/include"
"${CMAKE_CURRENT_LIST_DIR}/wrapper/include"
"${CMAKE_CURRENT_LIST_DIR}/app/include"
)

#set link dir
link_directories(
"${CMAKE_CURRENT_LIST_DIR}/thirdParty/lib"
)

#sub directories
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/asserts")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/framework")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/wrapper")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/app")

add_executable(${PROJECT_NAME} ${MAIN_SRCS} ${THIRDPARTY_SOURCES})

target_link_libraries(${PROJECT_NAME} glfw3 fw app wrapper)