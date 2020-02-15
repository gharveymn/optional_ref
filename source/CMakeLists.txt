FIND_PROGRAM(HAS_CREATE_PACKAGE CREATE_PACKAGE)
IF(NOT HAS_CREATE_PACKAGE)
  EXECUTE_PROCESS(COMMAND git submodule update --init -- "external/create-package"
                  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                  OUTPUT_QUIET)
  INCLUDE("${PROJECT_SOURCE_DIR}/external/create-package/cmake/create-package.cmake")
ENDIF()

CREATE_PACKAGE(AUTHOR        gharveymn
               NAME          optional_ref
               HEADERS       optional_ref.hpp
               COMPATIBILITY SameMinorVersion)

# Only include tests if we are at the top level
IF(OPTIONAL_REF_BUILD_TESTS)
  ADD_SUBDIRECTORY(test)
ENDIF()
