add_library(uiiitetsimec SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/appcharcs.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/appcontext.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/appcontextmanager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/appinfo.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/applicationlist.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/applistclient.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/etsimecoptions.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/notificationevent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/staticfileueapplcmproxy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/staticueapplcmproxy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ueapplcmproxy.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
)

target_link_libraries(uiiitetsimec
  uiiitrest
  uiiitsupport
  ${GLOG}
  ${OPENSSL_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPP_REST_LIBRARY}
)
