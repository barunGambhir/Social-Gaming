
add_library(networking
  src/Server.cpp
  src/Client.cpp
  src/businessLogic/GameOrchestrator.cpp
  src/businessLogic/Player.cpp
)
# For including the rock paper scissors Game text file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/businessLogic/exampleGames/rockpaperscissors.game ${CMAKE_BINARY_DIR}/rockpaperscissors.game COPYONLY)

add_subdirectory(json-parser)  

target_include_directories(networking
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)



if (NETWORKING_EMSCRIPTEN_BUILD)
  target_link_libraries(networking
    PRIVATE
      json-parser
      websocket.js
  )
else()
  find_package(Boost 1.72 COMPONENTS system REQUIRED)
  target_link_libraries(networking
    PRIVATE
      json-parser
      ${Boost_LIBRARIES}
    PUBLIC
      parser
  )
  target_include_directories(networking
    PRIVATE
      ${Boost_INCLUDE_DIR}
  )
endif()

set_target_properties(networking
                      PROPERTIES
                      LINKER_LANGUAGE CXX
                      CXX_STANDARD 17
)

install(TARGETS networking
  ARCHIVE DESTINATION lib
)

