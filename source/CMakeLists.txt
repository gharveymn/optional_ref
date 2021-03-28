IF (NOT CREATE_PACKAGE_VERSION VERSION_GREATER_EQUAL 0.0.5)
  IF (NOT EXISTS "${PROJECT_SOURCE_DIR}/external/create-package/cmake/create-package.cmake")
    MESSAGE (STATUS "Initializing create-package-0.0.5 in git submodule.")
    EXECUTE_PROCESS (COMMAND git submodule --quiet update --init -- "${PROJECT_SOURCE_DIR}/external/create-package"
                     WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
  ENDIF ()
  INCLUDE ("${PROJECT_SOURCE_DIR}/external/create-package/cmake/create-package.cmake")
ENDIF ()

CREATE_PACKAGE (AUTHOR        gharveymn
                NAMESPACE     gch
                NAME          optional_ref
                HEADERS       optional_ref.hpp
                COMPATIBILITY SameMinorVersion)

# Only include tests if we are at the top level
IF (GCH_OPTIONAL_REF_BUILD_TESTS)
  MESSAGE (STATUS "Testing for gch::optional_ref is enabled.")
  ADD_SUBDIRECTORY (test)
ELSE ()
  MESSAGE (STATUS "Testing for gch::optional_ref is disabled.")
ENDIF ()
