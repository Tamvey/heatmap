add_subdirectory(src)
add_executable(heatmap app/main.cpp src)

target_include_directories(heatmap PRIVATE src)
target_link_libraries(heatmap src)
