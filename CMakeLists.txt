cmake_policy(SET CMP0048 NEW)

project(mmbots VERSION 1.0.0)
cmake_minimum_required(VERSION 3.2)

set (CMAKE_CXX_STANDARD 17)

# Cmake find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/rest/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/rest/support/cmake")

if(APPLE)
  set(OPENSSL_LIBRARIES /usr/local/opt/openssl/lib/libssl.dylib /usr/local/opt/openssl/lib/libcrypto.dylib)
  set(OPENSSL_INCLUDE_DIR /usr/local/opt/openssl/include)
  find_package(glog REQUIRED)
  set(GLOG glog::glog)
else()
  find_package(OpenSSL REQUIRED)
  set(GLOG glog)
endif()
find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)
find_package(Boost
  REQUIRED
  COMPONENTS program_options filesystem system chrono thread
)
find_package(CppREST REQUIRED) # populates CPP_REST_INCLUDE_DIR and CPP_REST_LIBRARY

set (DISABLE_WARNINGS "-Wno-missing-field-initializers -Wno-unused-parameter -Wnon-virtual-dtor")
set (COMPILER_COMMON_FLAGS "-Wall -Wextra -Werror -DGTEST_CREATE_SHARED_LIBRARY=1 -DGTEST_LINKED_AS_SHARED_LIBRARY=1 -fPIC ${DISABLE_WARNINGS}")
set (CMAKE_CXX_FLAGS_DEBUG "${COMPILER_COMMON_FLAGS} -g -O0")
set (CMAKE_CXX_FLAGS_RELEASE "${COMPILER_COMMON_FLAGS} -O2 -DNDEBUG")

MESSAGE("============CONFIGURATION SUMMARY================")
MESSAGE("")
MESSAGE("CMAKE_SOURCE_DIR:         ${CMAKE_CURRENT_SOURCE_DIR}")
MESSAGE("COMPILER:                 ${CMAKE_CXX_COMPILER}")
MESSAGE("COMPILER FLAGS DEBUG:     ${CMAKE_CXX_FLAGS_DEBUG}")
MESSAGE("COMPILER FLAGS RELEASE:   ${CMAKE_CXX_FLAGS_RELEASE}")
MESSAGE("CMAKE_BUILD_TYPE:         ${CMAKE_BUILD_TYPE}")
MESSAGE("Protobuf Library:         ${PROTOBUF_LIBRARY}")
MESSAGE("Protobuf Includes Path:   ${PROTOBUF_INCLUDE_DIR}")
MESSAGE("CppREST Library:          ${CPP_REST_LIBRARY}")
MESSAGE("CppREST Includes Path:    ${CPP_REST_INCLUDE_DIR}")
MESSAGE("OpenSSL Libraries:        ${OPENSSL_LIBRARIES}")
MESSAGE("OpenSSL Includes Path:    ${OPENSSL_INCLUDE_DIR}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rest)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rest/support)
set(PROTO_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto-src)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})
include_directories(${PROTO_SRC_DIR})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})

add_subdirectory(rest/Rest)
add_subdirectory(rest/support/Support)
add_subdirectory(rest/support/RpcSupport)
add_subdirectory(EtsiMec)
add_subdirectory(Executables)
add_subdirectory(OpenWhisk)

# unit tests
if (${CMAKE_BUILD_TYPE} STREQUAL "debug")
  set(WITH_TEST_RPC ON)
  enable_testing()
  add_subdirectory(Test)
  set(skip_google_test TRUE)
  add_subdirectory(rest/Test)
  add_subdirectory(rest/support/Test)
endif()
