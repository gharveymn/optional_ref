if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  string (REGEX REPLACE "/[wW]([0-4deovX]|all) ?" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string (REGEX REPLACE "/GR ?" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif ()

macro (add_optional_ref_unit_test target_name)
  add_executable (${target_name} ${ARGN})
  target_link_libraries (${target_name} PRIVATE gch::optional_ref)

  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options (
      ${target_name}
      PRIVATE
        -fno-rtti
        -ftime-trace
        --coverage
        -Weverything
        -Wno-padded
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-zero-as-null-pointer-constant
        -Wno-used-but-marked-unused
    )

    target_link_options (
      ${target_name}
      PRIVATE
        -fno-rtti
        --coverage
    )

    if (GCH_USE_LIBCXX_WITH_CLANG)
      target_compile_options (${target_name} PRIVATE -stdlib=libc++)
      target_link_options (${target_name} PRIVATE LINKER:--allow-multiple-definition)
      target_link_libraries (${target_name} PRIVATE c++ c++abi)
    endif ()
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options (
      ${target_name}
      PRIVATE
        -fno-rtti
        -pedantic
        -Wall
        -Wextra
        -Wcast-align
        -Wcast-qual
        -Wctor-dtor-privacy
        -Wdisabled-optimization
        -Wformat=2
        -Winit-self
        -Wlogical-op
        -Wmissing-declarations
        -Wmissing-include-dirs
        -Wnoexcept
        -Wold-style-cast
        -Woverloaded-virtual
        -Wredundant-decls
        -Wshadow
        -Wsign-conversion
        -Wsign-promo
        -Wstrict-null-sentinel
        -Wstrict-overflow=5
        -Wswitch-default
        -Wundef
        -Wno-unused
    )
  elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options (
      ${target_name}
      PRIVATE
        /W4
        /GR-
    )
  endif ()

  if (UNIX)
    target_compile_options (${target_name} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
    target_link_options (${target_name} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
  endif ()

  add_test (
    NAME
      ${target_name}
    COMMAND
      ${target_name}
  )
endmacro ()

add_custom_target (optional_ref.ctest)

if (NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
  set (CMAKE_CXX_STANDARD_REQUIRED OFF)
endif ()

if (NOT DEFINED CMAKE_CXX_EXTENSIONS)
  set (CMAKE_CXX_EXTENSIONS OFF)
endif ()

macro (add_optional_ref_ctest_executables)
  foreach (file ${ARGN})
    get_filename_component (_TARGET_NAME_BASE "${file}" NAME_WE)
    string (PREPEND _TARGET_NAME_BASE "optional_ref.")

    foreach (version 11 14 17 20)
      set (_TARGET_NAME ${_TARGET_NAME_BASE}.c++${version})

      add_optional_ref_unit_test (${_TARGET_NAME} ${file})

      target_compile_features (${_TARGET_NAME} PRIVATE cxx_std_${version})
      add_dependencies(optional_ref.ctest ${_TARGET_NAME})
    endforeach ()
  endforeach ()
endmacro ()

add_optional_ref_ctest_executables (
  test-arrow.cpp
  test-as_const.cpp
  test-as_mutable.cpp
  test-assign.cpp
  test-bind.cpp
  test-comparison-constexpr-disparate.cpp
  test-comparison-constexpr.cpp
  test-comparison.cpp
  test-const.cpp
  test-contains.cpp
  test-deduction.cpp
  test-hash.cpp
  test-incomplete.cpp
  test-inheritence.cpp
  test-instantiation.cpp
  test-make_optional_ref.cpp
  test-movement.cpp
  test-nullopt.cpp
  test-pointer-cast.cpp
  test-swap-constexpr.cpp
  test-throw.cpp
)
