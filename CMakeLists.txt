cmake_minimum_required(VERSION 3.9)

project(livlang)

add_executable(${PROJECT_NAME} 
  src/main.c
  src/logger.c
  src/vector.c
  src/file_io.c
  src/token.c
  src/lexer.c
  src/ast_node.c
  src/parser.c
  src/environment.c
  src/eval.c
)

set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 23)