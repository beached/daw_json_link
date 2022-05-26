set(DAW_JSON_USE_SANITIZERS OFF CACHE STRING "Enable address and undefined sanitizers")
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
    if (MSVC)
			message( STATUS "Clang-CL ${CMAKE_CXX_COMPILER_VERSION} detected")
    	add_definitions(-DNOMINMAX -DD_WIN32_WINNT=0x0601)
			set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG /permissive-")
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 -DNDEBUG /permissive-")
			if( DAW_ALLOW_SSE42 )
				message( STATUS "Using -march=native" )
				add_compile_options(-march=native)
 			endif( )
    else( )
			message( STATUS "Clang ${CMAKE_CXX_COMPILER_VERSION} detected")
			add_compile_options( 
				-Wall -Wextra -pedantic -Weverything 
				-Wunreachable-code 
				-Wzero-as-null-pointer-constant 
				-ftemplate-backtrace-limit=0 
				-Wno-c++98-compat 
				-Wno-covered-switch-default 
				-Wno-padded 
				-Wno-exit-time-destructors 
				-Wno-c++98-compat-pedantic 
				-Wno-unused-parameter 
				-Wno-missing-prototypes 
				-Wno-disabled-macro-expansion 
				-Wno-missing-braces 
				-Wno-unneeded-internal-declaration 
				-Wno-tautological-type-limit-compare 
				-Wno-float-equal 
				-Wno-unused-macros 
				-Wno-global-constructors 
				-Wno-used-but-marked-unused 
				-Wno-weak-vtables 
				-Wno-documentation 
				-Wno-undefined-inline 
				)
			if ((${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" AND CMAKE_CXX_COMPILER_VERSION LESS "13.1.6") OR CMAKE_CXX_COMPILER_VERSION LESS 13.0.0)
        # This was removed in clang-13
        add_compile_options(-Wno-return-std-move-in-c++11)
      elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 14.0.0)
        add_compile_options(-Wno-bitwise-instead-of-logical)
      endif ()
      if (${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang" OR CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 10.0.0)
      	add_compile_options(-Wno-poison-system-directories)
      endif ()
			if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
				if( NOT CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" )
					if( CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 10.0.0 )
						message( STATUS "Adding Intel JCC bugfix" )
						add_compile_options( -mbranches-within-32B-boundaries )
					endif( )
				endif( )
			endif( )
			if( DAW_ALLOW_SSE42 )
				message( STATUS "Using -march=native" )
				add_compile_options(-march=native)
 			endif( )
			set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -DNDEBUG")

			if (DAW_JSON_USE_SANITIZERS)
				message( STATUS "Using sanitizers" )
				#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=null")
				#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fsanitize=null")
				set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fsanitize=undefined")
				set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fsanitize=address")
				set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
				set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
			endif ()
			if (CMAKE_BUILD_TYPE STREQUAL "coverage" OR CODE_COVERAGE)
				set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fprofile-instr-generate -fcoverage-mapping")
				set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-instr-generate -fcoverage-mapping")
			endif ()
		endif ()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	message( STATUS "g++ ${CMAKE_CXX_COMPILER_VERSION} detected")
	add_compile_options(--param max-gcse-memory=260000000  -Wall -Wextra -pedantic -Wno-deprecated-declarations -Wduplicated-cond -Wlogical-op -Wold-style-cast -Wshadow -Wzero-as-null-pointer-constant)
	#-Wdisabled-optimization)
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
		if( CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 9.0.0 )
			if( LINUX )
				message( STATUS "Adding Intel JCC bugfix" )
				add_compile_options( -Wa,-mbranches-within-32B-boundaries )
			endif( )
    	endif( )
	endif ()
	if( DAW_ALLOW_SSE42 )
		message( STATUS "Using -march=native" )
		add_compile_options(-march=native)
 	endif( )
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g -DNDEBUG")

	if (DAW_JSON_USE_SANITIZERS)
		message( STATUS "Using sanitizers" )
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fsanitize=undefined")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fsanitize=address")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
	endif ()
elseif (MSVC)
	message(STATUS "MSVC detected")
    	add_definitions(-DNOMINMAX -DD_WIN32_WINNT=0x0601)
    	add_compile_options("/permissive-")
    	add_compile_options("/wd4146")
		add_compile_options("/bigobj")
else ()
	message(STATUS "Unknown compiler id ${CMAKE_CXX_COMPILER_ID}")
endif ()

