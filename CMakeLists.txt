cmake_minimum_required (VERSION 3.15)

project (
  optional_ref
  VERSION
    0.3.0
  LANGUAGES
    CXX
)

string (COMPARE EQUAL "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}" _ENABLE_TESTS_DEFAULT)

option (
  GCH_OPTIONAL_REF_ENABLE_TESTS
  "Set to ON to enable generation of test targets for gch::optional_ref."
  ${_ENABLE_TESTS_DEFAULT}
)

option (
  GCH_OPTIONAL_REF_ENABLE_DOXYGEN
  "Set to ON to enable generation of Doxygen targets for gch::optional_ref."
  OFF
)

include (CMakeDependentOption)
cmake_dependent_option (
  GCH_USE_LIBCXX_WITH_CLANG
  "Set to ON to use libc++ when compiling with Clang."
  OFF
  "CMAKE_CXX_COMPILER_ID MATCHES Clang"
  OFF
)

if (GCH_OPTIONAL_REF_ENABLE_TESTS)
  enable_testing ()
endif ()

add_subdirectory (source)

export (
  EXPORT
    optional_ref-targets
  NAMESPACE
    gch::
)
