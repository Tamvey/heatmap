set(OpenCV_DIR D:\\Libs\\OpenCV-MSVC\\opencv\\build\\x64\\vc16\\lib)
find_package(OpenCV REQUIRED)
add_library(src build_heatmap.cpp)

target_include_directories(src PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(src ${OpenCV_LIBS})

