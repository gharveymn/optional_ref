EXECUTE_PROCESS(COMMAND "git submodule update --init -- \"external/create-package\""
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

INCLUDE("${PROJECT_SOURCE_DIR}/external/create-package/cmake/create-package.cmake" OPTIONAL)

CREATE_PACKAGE(AUTHOR       gharveymn
               NAME         optional_ref
               HEADERS      optional_ref.hpp)

# Only include tests if we are at the top level
IF(OPTIONAL_REF_BUILD_TESTS)
  ADD_SUBDIRECTORY(test)
ENDIF()
