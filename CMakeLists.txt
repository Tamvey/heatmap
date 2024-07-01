cmake_minimum_required(VERSION 3.5)
project(heatmap)

add_subdirectory(src)
add_executable(${PROJECT_NAME} app/main.cpp src)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} src)
