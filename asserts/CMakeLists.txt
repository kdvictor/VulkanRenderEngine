#copy asserts to des
file(COPY "./model" DESTINATION "${CMAKE_BINARY_DIR}/asserts")

#get shaders
set(SHADERS)
file(GLOB_RECURSE VS_SHADER "${CMAKE_CURRENT_LIST_DIR}/*.vs")
list(APPEND SHADERS ${VS_SHADER})

file(GLOB_RECURSE FS_SHADER "${CMAKE_CURRENT_LIST_DIR}/*.fs")
list(APPEND SHADERS ${FS_SHADER})

#read shader content
foreach(shader IN LISTS SHADERS)
	get_filename_component(shader_name ${shader} NAME)
	string(REPLACE "." "_" shader_name ${shader_name})
	string(TOUPPER ${shader_name} shader_name)
	file(READ "${shader}" ${shader_name})
endforeach()

#write shader content to header
file(REMOVE "${CMAKE_CURRENT_LIST_DIR}/include/shader.h")
configure_file("${CMAKE_CURRENT_LIST_DIR}/shader.h.in"
				"${CMAKE_CURRENT_LIST_DIR}/include/shader.h" @ONLY)