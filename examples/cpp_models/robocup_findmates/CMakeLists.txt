cmake_minimum_required(VERSION 2.8.3)

add_executable("${PROJECT_NAME}_robocup_findmates"
src/state_var_types.cpp  
src/robocup_findmates.cpp
  src/main.cpp
)
target_link_libraries("${PROJECT_NAME}_robocup_findmates"
  "${PROJECT_NAME}"
)
install(TARGETS "${PROJECT_NAME}_robocup_findmates"
  RUNTIME DESTINATION "${BINARY_INSTALL_PATH}"
)