cmake_minimum_required(VERSION 2.8.3)

add_executable("${PROJECT_NAME}_robocup_cleanroom"
src/state_var_types.cpp  
src/robocup_cleanroom.cpp
  src/main.cpp
)
target_link_libraries("${PROJECT_NAME}_robocup_cleanroom"
  "${PROJECT_NAME}"
)
install(TARGETS "${PROJECT_NAME}_robocup_cleanroom"
  RUNTIME DESTINATION "${BINARY_INSTALL_PATH}"
)