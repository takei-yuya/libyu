include_directories(${PROJECT_SOURCE_DIR})

file(GLOB SOURCES "*_test.cpp")
foreach(source ${SOURCES})
  get_filename_component(executable ${source} NAME_WE)
  add_executable(${executable} ${source})
  add_test(NAME ${executable} COMMAND ${executable})
endforeach()
