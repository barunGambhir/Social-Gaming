add_executable(chatserver
  chatserver.cpp
)

target_include_directories(chatserver
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/networking/src/include
)
set_target_properties(chatserver
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 17
                      PREFIX ""
)

find_package(Threads REQUIRED)

target_link_libraries(chatserver
  networking
  parser
  json-parser
  ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS chatserver
  RUNTIME DESTINATION bin
)
