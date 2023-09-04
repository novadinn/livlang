cmake_minimum_required(VERSION 3.9)

project(vlad)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(
  ${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/logger.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/token.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
)
