set(PROTOS
  ${CMAKE_CURRENT_SOURCE_DIR}/ueapplcmproxy.proto
)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_DIR} ${PROTOS})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${PROTOS})

add_library(uiiitetsimec SHARED
  ${PROTO_SRCS}
  ${GRPC_SRCS}
  ${CMAKE_CURRENT_SOURCE_DIR}/appcharcs.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/appcontext.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/appcontextmanager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/appinfo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/applicationlist.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/applistclient.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/etsimecoptions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/grpcueapplcmproxy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/notificationevent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/staticfileueapplcmproxy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/staticueapplcmproxy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ueapplcmproxy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
)

target_link_libraries(uiiitetsimec
  uiiitrest
  uiiitsupport
  uiiitrpcsupport
  ${GLOG}
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPP_REST_LIBRARY}
  gRPC::grpc++_reflection
  protobuf::libprotobuf
)
