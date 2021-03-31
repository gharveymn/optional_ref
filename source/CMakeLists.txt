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

IF (GCH_OPTIONAL_REF_BUILD_DOXYGEN)
  MESSAGE (STATUS "Doxygen for gch::optional_ref is enabled.")

  FIND_PACKAGE (Doxygen)
  IF (DOXYGEN_FOUND)
    MESSAGE (STATUS "Doxygen build files will be generated")

    SET (DOXYGEN_IN_FILE    "${CMAKE_CURRENT_LIST_DIR}/docs/doxygen/Doxyfile.in")
    SET (DOXYGEN_BUILD_FILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

    CONFIGURE_FILE (${DOXYGEN_IN_FILE} ${DOXYGEN_BUILD_FILE} @ONLY)

    ADD_CUSTOM_TARGET (optional_ref.doxygen
                       COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_BUILD_FILE}"
                       WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                       COMMENT "Generating Doxygen documentation for gch::optional_ref."
                       VERBATIM)

    INSTALL (DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doxygen/html"
             DESTINATION share/doc/gch/optional_ref)
  ELSE ()
    MESSAGE (WARNING "Doxygen installation not found. Build files for documentation will not be generated")
  ENDIF ()
ENDIF ()

IF (GCH_OPTIONAL_REF_BUILD_TESTS)
  MESSAGE (STATUS "Testing for gch::optional_ref is enabled.")
  ADD_SUBDIRECTORY (test)
ELSE ()
  MESSAGE (STATUS "Testing for gch::optional_ref is disabled.")
ENDIF ()
