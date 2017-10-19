include_directories(${PROJECT_SOURCE_DIR} ${HUSKY_EXTERNAL_INCLUDE})

file(GLOB base-src-files
  serialization.cpp
  abstract_partition_manager.cpp
  range_partition_manager.cpp
  )

add_library(base-objs OBJECT ${base-src-files})
set_property(TARGET base-objs PROPERTY CXX_STANDARD 11)
add_dependencies(base-objs ${external_project_dependencies})
