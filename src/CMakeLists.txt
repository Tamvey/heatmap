find_package(OpenCV REQUIRED)
add_library(src build_heatmap.cpp)

include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(src ${OpenCV_LIBS})

