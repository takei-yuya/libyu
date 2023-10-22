include_directories(${PROJECT_SOURCE_DIR})

file(GLOB SOURCES "*_sample.cpp")
foreach(source ${SOURCES})
  get_filename_component(executable ${source} NAME_WE)
  add_executable(${executable} ${source})
endforeach()
