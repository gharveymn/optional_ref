cmake_minimum_required (VERSION 3.15)

project (
  optional_ref
  VERSION
    0.3.1
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

if (GCH_OPTIONAL_REF_ENABLE_TESTS)
  include (CMakeDependentOption)

  cmake_dependent_option (
    GCH_USE_LIBCXX_WITH_CLANG
    "Set to ON to use libc++ when compiling with Clang."
    OFF
    "CMAKE_CXX_COMPILER_ID MATCHES Clang"
    OFF
  )

  enable_testing ()
endif ()

set (
  GCH_OPTIONAL_REF_INSTALL_CMAKE_DIR
  "lib/cmake/optional_ref"
  CACHE STRING
  "Specify where to install the CMake config files."
)

set (
  GCH_OPTIONAL_REF_INSTALL_INCLUDE_DIR
  "include"
  CACHE STRING
  "Specify where to install gch/optional_ref.hpp."
)

set (
  GCH_OPTIONAL_REF_INSTALL_LICENSE_DIR
  "share/licenses/gch/optional_ref"
  CACHE STRING
  "Specify where to install the license."
)

set (
  GCH_OPTIONAL_REF_INSTALL_SUPPORT_DIR
  "share/gch"
  CACHE STRING
  "Specify where to install support files like GDB pretty printers."
)

add_subdirectory (source)

export (
  EXPORT
    optional_ref-targets
  NAMESPACE
    gch::
)

install (
  FILES
    docs/LICENSE
  DESTINATION
    ${GCH_OPTIONAL_REF_INSTALL_LICENSE_DIR}
)
