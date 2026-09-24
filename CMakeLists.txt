cmake_minimum_required(VERSION 3.20)

project(VulkanRenderEngine)

#set c++ properties
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#QT6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

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
"${CMAKE_CURRENT_LIST_DIR}/vulkanEngine/include"
)

#set link dir
link_directories(
"${CMAKE_CURRENT_LIST_DIR}/thirdParty/lib"
)

#sub directories
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/vulkanEngine")

qt_add_executable(${PROJECT_NAME}
    ${MAIN_SRCS} 
    ${THIRDPARTY_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets vulkanEngine)

# 确保包含 Qt6
target_include_directories(${PROJECT_NAME} PRIVATE
    ${Qt6_INCLUDE_DIRS}
)

# 自动运行 windeployqt
if(WIN32)
    # 获取 windeployqt 可执行文件路径
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    if(WINDEPLOYQT_EXECUTABLE)
        # 添加自定义命令，在构建后自动运行 windeployqt
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --verbose 0
                --no-compiler-runtime
                "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMENT "Running windeployqt to deploy Qt libraries..."
        )
        message(STATUS "windeployqt found: ${WINDEPLOYQT_EXECUTABLE}")
    else()
        message(WARNING "windeployqt not found. Qt libraries will not be automatically deployed.")
    endif()
endif()