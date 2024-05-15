cmake_minimum_required(VERSION 3.19)
project(cpp-tree-sitter)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/CPM.cmake)

# Downloads this wrapper library and tree-sitter.
CPMAddPackage(
  NAME cpp-tree-sitter
  GIT_REPOSITORY https://github.com/nsumner/cpp-tree-sitter.git
  GIT_TAG v0.0.1
)

# Downloads a tree-sitter grammar from github and makes it available as a cmake library target.
add_grammar_from_repo(tree-sitter-json
  https://github.com/tree-sitter/tree-sitter-json.git
  0.19.0
)

add_grammar_from_repo(socialgaming-tree-sitter              # Defines the library name for a grammar
  https://github.com/nsumner/tree-sitter-socialgaming.git   # Repository URL of a tree-sitter grammar
  0.0.1
)  

# Include your new project structure
include_directories(include)  # Include the "include" directory

# Use the library in a parser program.
add_library(parser)

# Update the target sources to use files from the "src" directory
file(GLOB SOURCES "src/*.cpp")

# Exclude demo.cpp from the list of source files for parsing
# Demo uses it for its main() function, but it prevents networking from compiling due to multiple mains
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/socialgaming_parser_demo.cpp")

target_sources(parser PRIVATE ${SOURCES})


# Add the "src" directory to the include directories for your target
target_include_directories(parser PRIVATE src)

target_link_libraries(parser
  PUBLIC
    socialgaming-tree-sitter
    tree-sitter-json
    cpp-tree-sitter
)


# #---- make demo for testing -----------------
# add_executable(demo)
# target_sources(demo
#   PRIVATE
#     src/socialgaming_parser_demo.cpp
#     src/Map.cpp
#     src/MapHandler.cpp
#     src/MapEntry.cpp
#     src/Helper.cpp
#     src/Setup.cpp
#     src/Configuration.cpp
#     src/GameLogic.cpp
#     src/Environment.cpp
#     src/Expression.cpp
#     src/Rules.cpp
#     src/GameNode.cpp
#     src/ExtractWhatItem.cpp
# )
# target_include_directories(demo PRIVATE src)

# target_link_libraries(demo
#   socialgaming-tree-sitter
#   tree-sitter-json
#   cpp-tree-sitter
# )
