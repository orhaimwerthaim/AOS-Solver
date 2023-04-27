cmake_minimum_required(VERSION 2.8.3)

##################################
#adding json
set(JSON_BuildTests OFF CACHE INTERNAL "")
include_directories("/home/or/AOS/AOS-Solver/nlohmann_json")  
##################################

add_executable("${PROJECT_NAME}_collectValuableToys"
#src/state_var_types.cpp   
src/collectValuableToys.cpp
src/main.cpp
src/closed_model.cpp
#src/mongoDB_Bridge.cpp
)

 target_link_libraries("${PROJECT_NAME}_collectValuableToys"
   ${TinyXML_LIBRARIES}
   PRIVATE nlohmann_json::nlohmann_json #adding json
 )


##############
#added for mongo DB client
find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)
include_directories(${LIBMONGOCXX_INCLUDE_DIR})
include_directories(${LIBBSONCXX_INCLUDE_DIR})
include_directories("/usr/local/include/mongocxx/v_noabi")
include_directories("/usr/local/include/bsoncxx/v_noabi")
include_directories("/usr/local/include/libmongoc-1.0")
include_directories("/usr/local/include/libbson-1.0")
include_directories("/usr/local/lib")
 

target_link_libraries("${PROJECT_NAME}_collectValuableToys" ${LIBMONGOCXX_LIBRARIES})
target_link_libraries("${PROJECT_NAME}_collectValuableToys" ${LIBBSONCXX_LIBRARIES})
##############


target_link_libraries("${PROJECT_NAME}_collectValuableToys"
  "${PROJECT_NAME}"
)
install(TARGETS "${PROJECT_NAME}_collectValuableToys"
  RUNTIME DESTINATION "${BINARY_INSTALL_PATH}"
)