cmake_minimum_required(VERSION 2.8.3)

add_executable("${PROJECT_NAME}_icaps_working_toy"
src/state_var_types.cpp   
src/icaps.cpp
  src/main.cpp
)
target_link_libraries("${PROJECT_NAME}_icaps_working_toy"
  "${PROJECT_NAME}"
)
install(TARGETS "${PROJECT_NAME}_icaps_working_toy"
  RUNTIME DESTINATION "${BINARY_INSTALL_PATH}"
)