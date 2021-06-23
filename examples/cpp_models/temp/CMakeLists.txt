cmake_minimum_required(VERSION 2.8.3)

add_executable("${PROJECT_NAME}_temp"
  #cpplinq.hpp
  src/main.cpp
)
target_link_libraries("${PROJECT_NAME}_temp"
  "${PROJECT_NAME}"
)
install(TARGETS "${PROJECT_NAME}_temp"
  RUNTIME DESTINATION "${BINARY_INSTALL_PATH}"
)