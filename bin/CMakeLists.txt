add_executable(${CMAKE_PROJECT_NAME} main.cpp)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ../ lib)
# target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC <lib_name>) 
