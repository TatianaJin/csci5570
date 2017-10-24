include_directories(${PROJECT_SOURCE_DIR} ${HUSKY_EXTERNAL_INCLUDE})

file(GLOB server-src-files
  server_thread.cpp
  consistency/asp_model.cpp
  consistency/bsp_model.cpp
  consistency/ssp_model.cpp
  util/progress_tracker.cpp
  util/pending_buffer.cpp
  )

add_library(server-objs OBJECT ${server-src-files})
set_property(TARGET server-objs PROPERTY CXX_STANDARD 11)
add_dependencies(server-objs ${external_project_dependencies})
