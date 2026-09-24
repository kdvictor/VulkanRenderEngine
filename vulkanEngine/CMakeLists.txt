project(vulkanEngine)

# Vulkan
find_package(Vulkan REQUIRED)

file(GLOB srcs "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_LIST_DIR}/include/*.h")

add_library(${PROJECT_NAME} STATIC ${srcs} ${headers})
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${Vulkan_INCLUDE_DIRS})
