SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

ADD_EXECUTABLE(test_optional_ref main.cpp)
TARGET_LINK_LIBRARIES(test_optional_ref optional_ref)

IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  TARGET_COMPILE_OPTIONS(test_optional_ref PUBLIC -stdlib=libc++ -ftime-trace --coverage -Wall -Wextra)
  TARGET_LINK_OPTIONS(test_optional_ref PUBLIC -Wl,--allow-multiple-definition --coverage)
  TARGET_LINK_LIBRARIES(test_optional_ref c++ c++abi)
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  TARGET_COMPILE_OPTIONS(test_optional_ref PUBLIC -Wall -Wextra)
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  TARGET_COMPILE_OPTIONS(test_optional_ref PUBLIC /W4)
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")

ENDIF()

SET_TARGET_PROPERTIES(test_optional_ref PROPERTIES
          CXX_STANDARD          11
          CXX_STANDARD_REQUIRED NO
          CXX_EXTENSIONS        NO)
