add_library(uiiitopenwhisk SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/invoker.cpp
)

target_link_libraries(uiiitopenwhisk
  uiiitrest
  uiiitsupport
  ${GLOG}
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPP_REST_LIBRARY}
)
