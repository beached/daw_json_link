if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../.git)
	find_package(Git REQUIRED)
	if(GIT_FOUND)
    	execute_process(
    		COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    		OUTPUT_VARIABLE "BUILD_VERSION"
    		ERROR_QUIET
    		OUTPUT_STRIP_TRAILING_WHITESPACE)
			message( STATUS "GIT TAG: ${BUILD_VERSION}" )
	else(GIT_FOUND)
	    set(BUILD_VERSION "unknown")
			message( WARNING "GIT TAG: Could not find git tag" )
	endif(GIT_FOUND)
	target_compile_definitions( json_benchmark PRIVATE -DSOURCE_CONTROL_REVISION="${BUILD_VERSION}" )
else( )
	message( WARNING "GIT: Could not find git folder" )
endif( )


