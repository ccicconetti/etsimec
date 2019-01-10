add_executable(etsimeccli
  ${CMAKE_CURRENT_SOURCE_DIR}/etsimeccli.cpp
)

target_link_libraries(etsimeccli
  uiiitetsimec
  uiiitrest
  uiiitsupport
  ${GLOG}
  ${Boost_LIBRARIES}
)

add_executable(ueapplcmproxy
  ${CMAKE_CURRENT_SOURCE_DIR}/ueapplcmproxymain.cpp
)

target_link_libraries(ueapplcmproxy
  uiiitetsimec
  uiiitrest
  uiiitsupport
  ${GLOG}
  ${Boost_LIBRARIES}
)
