file(GLOB_RECURSE ARG_PARSER_SOURCES src/*.cpp)

add_library(ArgParser STATIC ${ARG_PARSER_SOURCES})

target_include_directories(ArgParser PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ArgParser utils)