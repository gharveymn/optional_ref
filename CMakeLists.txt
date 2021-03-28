CMAKE_MINIMUM_REQUIRED(VERSION 3.13)
PROJECT(optional_ref
        VERSION 0.1.1
        LANGUAGES CXX)

# Options
IF(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  OPTION(GCH_OPTIONAL_REF_BUILD_TESTS "Set to ON to build tests for gch::optional_ref" ON)
ELSE()
  OPTION(GCH_OPTIONAL_REF_BUILD_TESTS "Set to ON to build tests for gch::optional_ref" OFF)
ENDIF()

OPTION (GCH_USE_LIBCXX_WITH_CLANG "Set to ON to use libc++ when compiling with Clang" OFF)

ADD_SUBDIRECTORY(source)
