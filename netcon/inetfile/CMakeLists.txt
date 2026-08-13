set(HEADERS
  httpclient.h
)

set(CPPS
  httpclient.cpp
)

add_library(inetfile STATIC ${HEADERS} ${CPPS})
target_link_libraries(inetfile PRIVATE
  ddio
  CURL::libcurl
)
target_link_libraries(inetfile PUBLIC
  SDL3::SDL3
)
target_include_directories(inetfile PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/netcon/inetfile
  >
  mem
)
target_include_directories(inetfile PRIVATE ${SDL3_INCLUDE_DIRS})

