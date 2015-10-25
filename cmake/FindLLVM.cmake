#
# Check if the llvm-config gives us the path for the llvm libs.
#
# The following variables are set:
#  LLVM_CONFIG
#  LLVM_VERSION
#  LLVM_INCLUDE_DIRS - include directories for the llvm headers.
#  LLVM_SYSTEM_LIBRARIES - list of system libraries which llvm libraries depend on
#  LLVM_STATIC_LIBRARIES - list of paths for the static llvm libraries.
#  CLANG_STATIC_LIBRARIES - list of paths for the static clang libraries.
if (USE_LLVM_PATH)
  find_program(LLVM_CONFIG NAMES "llvm-config"
    PATHS ${USE_LLVM_PATH}/bin
    NO_DEFAULT_PATH)
else()
  find_program(LLVM_CONFIG NAMES "llvm-config")
endif()

if (NOT LLVM_CONFIG)
  message(FATAL_ERROR "unable to find llvm-config")
endif()

message(STATUS "Found LLVM: ${LLVM_CONFIG}")

execute_process(COMMAND ${LLVM_CONFIG} --version OUTPUT_VARIABLE LLVM_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG} --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG} --libdir OUTPUT_VARIABLE LLVM_LIB_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG} --system-libs OUTPUT_VARIABLE LLVM_SYSTEM_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG} --libfiles OUTPUT_VARIABLE LLVM_STATIC_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG} --libfiles Support OUTPUT_VARIABLE LLVM_SUPPORT_STATIC_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)

# convert the list of paths into a cmake list
separate_arguments(LLVM_SYSTEM_LIBRARIES)
separate_arguments(LLVM_STATIC_LIBRARIES)
separate_arguments(LLVM_SUPPORT_STATIC_LIBRARIES)

# find clang libraries
set(CLANG_STATIC_LIBRARIES)

set(STATIC_LIBRARIES
  libclangIndex.a
  libclangLex.a
  libclangSema.a
  libclangTooling.a
  libclangARCMigrate.a
  libclangFormat.a
  libclangToolingCore.a
  libclangASTMatchers.a
  libclangFrontend.a
  libclangDriver.a
  libclangParse.a
  libclangSerialization.a
  libclangSema.a
  libclangEdit.a
  libclangStaticAnalyzerCheckers.a
  libclangStaticAnalyzerCore.a
  libclangRewrite.a
  libclangAnalysis.a
  libclangAST.a
  libclangLex.a
  libclangBasic.a
  )

foreach(STATIC_LIBRARY ${STATIC_LIBRARIES})
  find_library(lib_${STATIC_LIBRARY} ${STATIC_LIBRARY}
    PATHS ${LLVM_LIB_DIRS}
    NO_DEFAULT_PATH)
  list(APPEND CLANG_STATIC_LIBRARIES ${lib_${STATIC_LIBRARY}})
endforeach()

find_library(libclang clang
  PATHS ${LLVM_LIB_DIRS}
  NO_DEFAULT_PATH)
list(APPEND LIBCLANG_LIBRARIES ${libclang})

set(CLANG_RESOURCE_DIR ${LLVM_LIB_DIRS}/clang/${LLVM_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LLVM DEFAULT_MSG LLVM_VERSION
  LLVM_INCLUDE_DIRS LLVM_LIB_DIRS LLVM_SYSTEM_LIBRARIES LLVM_STATIC_LIBRARIES
  LLVM_SUPPORT_STATIC_LIBRARIES CLANG_STATIC_LIBRARIES LIBCLANG_LIBRARIES CLANG_RESOURCE_DIR)

mark_as_advanced(LLVM_VERSION LLVM_INCLUDE_DIRS LLVM_LIB_DIRS
  LLVM_SYSTEM_LIBRARIES LLVM_STATIC_LIBRARIES LLVM_SUPPORT_STATIC_LIBRARIES
  LIBCLANG_LIBRARIES CLANG_STATIC_LIBRARIES CLANG_RESOURCE_DIR)
