# cmake_minimum_required(VERSION 3.19)
# project(json-parser)

set(PACKAGE_NAME json-parser)
set(PACKAGE_VERSION 0.0.1)
set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

add_library(json-parser INTERFACE)

target_link_libraries(json-parser
  INTERFACE
    nlohmann_json::nlohmann_json
)

target_include_directories(json-parser
  INTERFACE
    ${json_SOURCE_DIR}/include
)


install(TARGETS json-parser
        ARCHIVE DESTINATION lib
)

## Use the library in a demo program.
# add_executable(demo)
# target_sources(demo
#        PRIVATE
#        demo.cpp
# )
# target_link_libraries(demo
#   PRIVATE
#     nlohmann_json::nlohmann_json
# )

