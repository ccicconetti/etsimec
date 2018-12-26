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
