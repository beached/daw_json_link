# This cmake file provides the two lists below
set(DAW_TEST_DEFINITIONS "")
set(DAW_COMPILER_OPTIONS "")

set(DAW_NUM_RUNS OFF CACHE STRING "Force override number of times to run tests (positive integer)")
if (DAW_NUM_RUNS)
  list(APPEND DAW_TEST_DEFINITIONS "DAW_NUM_RUNS=${DAW_NUM_RUNS}")
endif ()

option(DAW_JSON_USE_SANITIZERS "Enable address and undefined sanitizers" OFF)

if (DAW_ALLOW_SSE42 AND NOT (MSVC AND NOT (CMAKE_CXX_COMPILER_ID MATCHES "Clang")))
  list(APPEND DAW_COMPILER_OPTIONS -march=native)
endif ()

if (MSVC)
  list(APPEND DAW_TEST_DEFINITIONS NOMINMAX _WIN32_WINNT=0x0601)
endif ()

if (DAW_JSON_USE_SANITIZERS)
  if (MSVC)
    message(NOTICE "Can't use sanitizers")
  else ()
    list(APPEND DAW_COMPILER_OPTIONS -fsanitize=undefined -fsanitize=address)
  endif ()
endif ()

if (NOT MSVC AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  list(APPEND DAW_COMPILER_OPTIONS
          -Wall
          -Wextra
          -pedantic
          -Weverything
          -Wno-c++98-compat
          -Wno-covered-switch-default
          -Wno-padded
          -Wno-exit-time-destructors
          -Wno-c++98-compat-pedantic
          -Wno-unused-parameter
          -Wno-missing-noreturn
          -Wno-missing-prototypes
          -Wno-disabled-macro-expansion
          -Wno-missing-braces
          -Wno-unneeded-internal-declaration
          -Wunreachable-code
          -Wno-tautological-type-limit-compare
          -Wno-return-std-move-in-c++11
          -Wno-float-equal
          -Wzero-as-null-pointer-constant
          -Wno-unused-macros)

  if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)"
          AND NOT (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
          AND (CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 10.0.0))
    message(STATUS "Adding Intel JCC bugfix")
    list(APPEND DAW_COMPILER_OPTIONS -mbranches-within-32B-boundaries)
  endif ()

  if (CMAKE_BUILD_TYPE STREQUAL "coverage" OR CODE_COVERAGE)
    list(APPEND DAW_COMPILER_OPTIONS -fprofile-instr-generate -fcoverage-mapping)
  endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  list(APPEND DAW_COMPILER_OPTIONS
          -Wall
          -Wextra
          -pedantic
          -Wno-deprecated-declarations
          -Wduplicated-cond
          -Wlogical-op
          -Wold-style-cast
          -Wshadow
          -Wzero-as-null-pointer-constant
          -Wdisabled-optimization)

  if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)"
          AND CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL 9.0.0
          AND LINUX)
    message(STATUS "Adding Intel JCC bugfix")
    list(APPEND DAW_COMPILER_OPTIONS -Wa,-mbranches-within-32B-boundaries)
  endif ()
elseif (MSVC)
  list(APPEND DAW_COMPILER_OPTIONS /permissive- /wd4146)
else ()
	message(WARNING "Unknown compiler ${CMAKE_CXX_COMPILER_ID}")
endif ()
