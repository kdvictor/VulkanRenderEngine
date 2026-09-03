function(buildThirdParty)
	cmake_parse_arguments(
		"arg"
		""
		"type;name;path;glad;glm"
		""
		${ARGN})
	message("name: ${arg_name} type: ${arg_type} path:${arg_path}")

	set(err 0 PARENT_SCOPE)

	#unzip
	message("****** tar -xzvf ${arg_name} ******")
	execute_process(COMMAND ${CMAKE_COMMAND} -E tar -xzvf ${arg_path}
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR} RESULT_VARIABLE result)
	if(NOT result EQUAL 0)
		set(err -1 PARENT_SCOPE)
	endif()

	if(${arg_type} STREQUAL "UNZIP")
		#glm
		if(NOT ${arg_glm} EQUAL -1)
			#copy include and src to thirdParty dir
			execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${PROJECT_BINARY_DIR}/${arg_name}/glm" "${CMAKE_CURRENT_LIST_DIR}/include/glm")
		endif()

		#glad
		if(NOT ${arg_glad} EQUAL -1)
			execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${PROJECT_BINARY_DIR}/${arg_name}/include" "${CMAKE_CURRENT_LIST_DIR}/include")
			execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${PROJECT_BINARY_DIR}/${arg_name}/src" "${CMAKE_CURRENT_LIST_DIR}/src")
		endif()
	else()
		set(source_dir "${CMAKE_BINARY_DIR}/${arg_name}")
		set(build_dir "${source_dir}/build")
		message("****** cmake -S ${source_dir} -B ${build_dir} ${arg_name} ******")
		execute_process(COMMAND ${CMAKE_COMMAND} -S ${source_dir} -B ${build_dir}
			-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_LIST_DIR} -DBUILD_SHADER_LIBS=OFF 
			-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} RESULT_VARIABLE result)
		if(NOT result EQUAL 0)
			set(err -1 PARENT_SCOPE)
		endif()

		message("****** cmake --build ${build_dir} ${arg_name} ******")
		execute_process(COMMAND ${CMAKE_COMMAND} --build ${build_dir} --config ${CMAKE_BUILD_TYPE} RESULT_VARIABLE result)
		if(NOT result EQUAL 0)
			set(err -1 PARENT_SCOPE)
		endif()

		message("****** cmake --install ******")
		execute_process(COMMAND ${CMAKE_COMMAND} --install ${build_dir} --config ${CMAKE_BUILD_TYPE} RESULT_VARIABLE result)
		if(NOT result EQUAL 0)
			set(err -1 PARENT_SCOPE)
		endif()

	endif()	

endfunction()

set(THIRDPARTY_HAS_BUILD OFF CACHE BOOL "thirdParty has build")
message("thirdParty has build: ${THIRDPARTY_HAS_BUILD}")

#loop build thirdyParty: glfw, glad, glm
file(GLOB thirdPartyZips "${CMAKE_CURRENT_LIST_DIR}/*.tar.gz")
message("thirdPartyZips: ${thirdPartyZips}")

if(NOT ${THIRDPARTY_HAS_BUILD})
	set(error_code 0)
	foreach(zip ${thirdPartyZips})
		message("zip: ${zip}")
		#get file name
		get_filename_component(zip_ext ${zip} NAME)
		string(REGEX REPLACE "\\.tar\\.gz$" "" zip_name ${zip_ext})
		#get file path
		get_filename_component(zip_path ${zip} PATH)
		message("${zip_name}:${zip_path}")

		#glad and glm only unzip
		string(FIND ${zip_name} "glad" gladIndex)
		string(FIND ${zip_name} "glm" glmIndex)

		if((NOT gladIndex EQUAL -1) OR (NOT glmIndex EQUAL -1))
			buildThirdParty(name ${zip_name} type "UNZIP" path ${zip} glad ${gladIndex} glm ${glmIndex})
		else()
			buildThirdParty(name ${zip_name} type "BUILD" path ${zip})
		endif()

		math(EXPR erro_code "${error_code} | ${err}")
	endforeach()

	if(${erro_code} EQUAL 0)
		set(THIRDPARTY_HAS_BUILD ON CACHE BOOL "thirdParty has build" FORCE)
	else()
		set(THIRDPARTY_HAS_BUILD OFF CACHE BOOL "thirdParty has build" FORCE)
	endif()
endif()