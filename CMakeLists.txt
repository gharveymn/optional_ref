CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(optional_ref
        VERSION 0.0.1
        LANGUAGES CXX)

INCLUDE(cmake/create-package.cmake)

CREATE_HEADER_ONLY_PACKAGE(AUTHOR       gch
                           NAME         optional_ref
                           HEADERS      optional_ref.hpp)

# Options
OPTION(OPTIONAL_REF_BUILD_TESTS "Set to ON to build tests" ON)

# Only include tests if we are at the top level
IF(OPTIONAL_REF_BUILD_TESTS AND (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR))
  ADD_SUBDIRECTORY(test)
ENDIF()
