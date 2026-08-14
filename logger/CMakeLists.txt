set(HEADERS
  log.h
)
set(CPPS
  log.cpp
)

add_library(logger STATIC ${HEADERS} ${CPPS})

target_include_directories(logger PUBLIC
  $<BUILD_INTERFACE:
    ${PROJECT_SOURCE_DIR}/logger
  >
)
