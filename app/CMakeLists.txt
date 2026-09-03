project(app)

file(GLOB srcs "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_LIST_DIR}/include/*.h")

add_library(${PROJECT_NAME} STATIC ${srcs} ${headers})