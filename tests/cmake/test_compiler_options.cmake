set(DAW_JSON_USE_SANITIZERS OFF CACHE STRING "Enable address and undefined sanitizers")
set(DAW_WERROR OFF CACHE STRING "Enable WError for test builds")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
    if (MSVC)
        message("Clang-CL ${CMAKE_CXX_COMPILER_VERSION} detected")
    	add_definitions(-DNOMINMAX -DD_WIN32_WINNT=0x0601)
			set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG /permissive-")
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 -DNDEBUG /permissive-")
			if( DAW_WERROR ) 
				add_compile_options( -Werror -pedantic-errors )
			endif( )
			if( DAW_ALLOW_SSE42 )
				message( "Using -march=native" )
				add_compile_options(-march=native)
 			endif( )
    else( )
      message("Clang ${CMAKE_CXX_COMPILER_VERSION} detected")
			add_compile_options( 
				-pedantic 
				-Weverything 
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
				-Wno-return-std-move-in-c++11 
				-Wno-float-equal 
				-Wno-unused-macros 
				-Wno-global-constructors 
				-Wno-used-but-marked-unused 
				-Wno-weak-vtables 
				-Wno-documentation 
				-Wno-undefined-inline 
				-Wno-poison-system-directories
			)
			if( DAW_WERROR ) 
				add_compile_options( -Werror -pedantic-errors )
			endif( )
			if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
				if( NOT CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" )
					if( CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 10.0.0 )
						message( "Adding Intel JCC bugfix" )
						add_compile_options( -mbranches-within-32B-boundaries )
					endif( )
				endif( )
			endif( )
			if( DAW_ALLOW_SSE42 )
				message( "Using -march=native" )
				add_compile_options(-march=native)
 			endif( )
			set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
			set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

			if (DAW_JSON_USE_SANITIZERS)
				message( "Using sanitizers" )
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
	message("g++ ${CMAKE_CXX_COMPILER_VERSION} detected")
	add_compile_options(--param max-gcse-memory=260000000 
		-Wall 
		-Wextra 
		-pedantic
		-Wconversion
		-Wno-deprecated-declarations
		-Wduplicated-cond 
		-Wlogical-op 
		-Wold-style-cast 
		-Wshadow 
		-Wzero-as-null-pointer-constant
	)
	if( DAW_WERROR ) 
		add_compile_options( -Werror -pedantic-errors )
	endif( )
	#-Wdisabled-optimization)
    if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
		if( CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 9.0.0 )
			if( LINUX )
				message( "Adding Intel JCC bugfix" )
				add_compile_options( -Wa,-mbranches-within-32B-boundaries )
			endif( )
    	endif( )
	endif ()
	if( DAW_ALLOW_SSE42 )
		message( "Using -march=native" )
		add_compile_options(-march=native)
 	endif( )
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

	if (DAW_JSON_USE_SANITIZERS)
		message( "Using sanitizers" )
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fsanitize=undefined")
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fsanitize=address")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
	endif ()
elseif (MSVC)
    	message("MSVC detected")
    	add_definitions(-DNOMINMAX -DD_WIN32_WINNT=0x0601)
    	add_compile_options("/permissive-")
    	add_compile_options("/wd4146")
		add_compile_options("/bigobj")
		if( DAW_WERROR )
			add_compile_options( /WX )
			add_definitions( -D_CRT_SECURE_NO_WARNINGS )
		endif( )	
else ()
	message("Unknown compiler id ${CMAKE_CXX_COMPILER_ID}")
endif ()

