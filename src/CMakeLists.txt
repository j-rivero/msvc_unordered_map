set (sources simplebug.cc)
ign_create_core_library(SOURCES ${sources} CXX_STANDARD 17)

target_link_libraries(${PROJECT_LIBRARY_TARGET_NAME}
  PUBLIC
  ignition-common3::ignition-common3
)
