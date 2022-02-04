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
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
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
    DESTINATION include/gch
)

file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/optional_ref-config.cmake [=[
get_filename_component (PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)
include ("${CMAKE_CURRENT_LIST_DIR}/optional_ref-targets.cmake")
]=])

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
    lib/cmake/optional_ref
)

install (
  EXPORT
    optional_ref-targets
  DESTINATION
    lib/cmake/optional_ref
  NAMESPACE
    gch::
)

if (GCH_OPTIONAL_REF_BUILD_DOCS)
  add_subdirectory (docs)
endif ()

if (GCH_OPTIONAL_REF_ENABLE_TESTS)
  add_subdirectory (test)
endif ()
