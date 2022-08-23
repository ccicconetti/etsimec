if(NOT skip_google_test)

  # Download and unpack googletest at configure time
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt.in googletest-download/CMakeLists.txt)
  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Test/googletest-download )
  if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Test/googletest-download )
  if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
  endif()

  # Prevent overriding the parent project's compiler/linker
  # settings on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  # Add googletest directly to our build. This defines
  # the gtest and gtest_main targets.
  add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                   ${CMAKE_BINARY_DIR}/googletest-build
                   EXCLUDE_FROM_ALL)

endif()

include_directories(${CMAKE_BINARY_DIR}/googletest-src/googletest/include)

include(GoogleTest)

set(LIBS
  uiiitetsimec
  uiiitopenwhisk
  uiiitrest
  gtest
  ${Boost_LIBRARIES}
)

add_executable(testetsimec testmain.cpp testetsimec.cpp)
target_link_libraries(testetsimec ${LIBS})
gtest_discover_tests(testetsimec)

add_executable(testinvoker testmain.cpp testinvoker.cpp)
target_link_libraries(testinvoker ${LIBS})
gtest_discover_tests(testinvoker)

add_executable(testgrpcueapplcmproxy testmain.cpp testgrpcueapplcmproxy.cpp)
target_link_libraries(testgrpcueapplcmproxy ${LIBS})
gtest_discover_tests(testgrpcueapplcmproxy)

add_executable(testlister testmain.cpp testlister.cpp)
target_link_libraries(testlister ${LIBS})
gtest_discover_tests(testlister)

add_executable(teststaticueapplcmproxy testmain.cpp teststaticueapplcmproxy.cpp)
target_link_libraries(teststaticueapplcmproxy ${LIBS})
gtest_discover_tests(teststaticueapplcmproxy)