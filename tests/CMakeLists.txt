add_executable(saturei_tests
  TestMain.cpp
  ColorMathTests.cpp
  ProfileStoreTests.cpp
)
target_link_libraries(saturei_tests PRIVATE saturei_core)
if(MSVC)
  target_compile_options(saturei_tests PRIVATE /W4 /permissive- /utf-8)
endif()

add_test(NAME saturei_tests COMMAND saturei_tests)
