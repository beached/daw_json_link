if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)
	find_package(Git)
	if(GIT_FOUND)
    	execute_process(
    		COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    		OUTPUT_VARIABLE "BUILD_VERSION"
    		ERROR_QUIET
    		OUTPUT_STRIP_TRAILING_WHITESPACE)
	    message( STATUS "Git version: ${BUILD_VERSION}" )
	else(GIT_FOUND)
	    set(BUILD_VERSION "unknown")
	endif(GIT_FOUND)
	add_definitions( -DSOURCE_CONTROL_REVISION="${BUILD_VERSION}" )
endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/.git)


