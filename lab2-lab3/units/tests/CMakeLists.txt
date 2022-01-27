include_directories("/home/oem/Documents/ACS/acs-spring-l3/src")
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(Google_Tests_run RunTests.cpp
                                TestExecutor.cpp
                                TestDecoder.cpp)

target_link_libraries(Google_Tests_run gtest gtest_main)
