add_library(uiiitopenwhisk SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/command.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/invoker.cpp
  #${CMAKE_CURRENT_SOURCE_DIR}/lister.cpp
)

target_link_libraries(uiiitopenwhisk
  uiiitrest
  uiiitsupport
  ${GLOG}
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPP_REST_LIBRARY}
)
