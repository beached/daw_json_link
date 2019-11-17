cmake_minimum_required(VERSION 3.10 )

project(dependency_prj)

include("CMakeListsCompiler.txt")

include_directories("./include")

include("dependencies.txt")

add_library(dependency_stub STATIC stub.cpp)
add_dependencies(dependency_stub ${DEP_PROJECT_DEPS})

