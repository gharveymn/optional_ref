add_library (optional_ref INTERFACE)

target_sources (
  optional_ref
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/gch/optional_ref.hpp>
)

target_include_directories (
  optional_ref
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_include_directories (
  optional_ref
  SYSTEM INTERFACE
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${GCH_OPTIONAL_REF_INSTALL_INCLUDE_DIR}>
)

set_target_properties (
  optional_ref
  PROPERTIES
  PUBLIC_HEADER
    include/gch/optional_ref.hpp
)

add_library (gch::optional_ref ALIAS optional_ref)

install (
  TARGETS
    optional_ref
  EXPORT
    optional_ref-targets
  PUBLIC_HEADER
    DESTINATION ${GCH_OPTIONAL_REF_INSTALL_INCLUDE_DIR}/gch
)

file (
  RELATIVE_PATH
  _PACKAGE_PREFIX_DIR
  "${CMAKE_INSTALL_PREFIX}/${GCH_OPTIONAL_REF_INSTALL_CMAKE_DIR}"
  "${CMAKE_INSTALL_PREFIX}"
)
string (PREPEND _PACKAGE_PREFIX_DIR \${CMAKE_CURRENT_LIST_DIR}/)

file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/optional_ref-config.cmake "\
get_filename_component (PACKAGE_PREFIX_DIR \"${_PACKAGE_PREFIX_DIR}\" ABSOLUTE)
include (\"\${CMAKE_CURRENT_LIST_DIR}/optional_ref-targets.cmake\")
")

include (CMakePackageConfigHelpers)
write_basic_package_version_file (
  optional_ref-version.cmake
  COMPATIBILITY
    SameMinorVersion
)

install (
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/optional_ref-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/optional_ref-version.cmake
  DESTINATION
    ${GCH_OPTIONAL_REF_INSTALL_CMAKE_DIR}
)

install (
  EXPORT
    optional_ref-targets
  DESTINATION
    ${GCH_OPTIONAL_REF_INSTALL_CMAKE_DIR}
  NAMESPACE
    gch::
)

install (
  DIRECTORY
    ${CMAKE_CURRENT_LIST_DIR}/support/
  DESTINATION
    ${GCH_OPTIONAL_REF_INSTALL_SUPPORT_DIR}
  PATTERN "__pycache__" EXCLUDE
)

if (GCH_OPTIONAL_REF_ENABLE_DOXYGEN)
  find_package (Doxygen)
  if (DOXYGEN_FOUND)
    set (DOXYGEN_EXCLUDE_SYMBOLS   gch::detail)
    set (DOXYGEN_GENERATE_TREEVIEW YES)
    set (DOXYGEN_OUTPUT_DIRECTORY  ${CMAKE_CURRENT_BINARY_DIR}/doxygen)
    set (DOXYGEN_TAB_SIZE          2)

    doxygen_add_docs (
      optional_ref.doxygen
      gch/optional_ref.hpp
      WORKING_DIRECTORY
        ${PROJECT_SOURCE_DIR}/source/include
    )
  else ()
    message (
      WARNING
      "Doxygen installation not found. Targets for Doxygen will not be generated"
    )
  endif ()
endif ()

if (GCH_OPTIONAL_REF_ENABLE_TESTS)
  add_subdirectory (test)
endif ()
