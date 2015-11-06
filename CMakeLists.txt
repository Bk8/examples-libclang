cmake_minimum_required(VERSION 3.0)
project(examples C CXX)
include(cmake/FindLLVM.cmake)
include(cmake/ThreadSupport.cmake)

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIB_DIRS})
link_libraries(${LLVM_SYSTEM_LIBRARIES})
link_libraries(${LIBCLANG_LIBRARIES})

add_definitions(-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)
add_definitions(-std=c++11 -fno-exceptions -fno-rtti)

add_executable(example
  src/example.cc)

add_executable(example-tokens
  src/example-tokens.cc)

add_executable(example-diagnose
  src/example-diagnose.cc)
