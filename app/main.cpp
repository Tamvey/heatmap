#include <iostream>
#include <fstream>
#include <string>
#include "build_heatmap.h"


int main(int argc, char *argv[])
{
    // Default props
    int frames_for_median = 1;
    int min_contour_area = 1;
    int usr_cols = 1;
    int usr_rows = 1;
    int gradient_colors = 12;
    int color_threshold = 9;
    float min_area = 1.0;

    // Read property file
    std::ifstream in((argv[2] != nullptr) ? argv[2] : "");
    if (in.is_open())
    {
        while (!in.eof())
        {
            std::string line;
            in >> line;

            int pos = line.find("=");
            if (line.substr(0, pos) == std::string("frames_for_median"))
                frames_for_median = std::stoi(line.substr(pos + 1, line.length()));
            if (line.substr(0, pos) == std::string("min_contour_area"))
                min_contour_area = std::stoi(line.substr(pos + 1, line.length()));
            if (line.substr(0, pos) == std::string("usr_cols"))
                usr_cols = std::stoi(line.substr(pos + 1, line.length()));
            if (line.substr(0, pos) == std::string("usr_rows"))
                usr_rows = std::stoi(line.substr(pos + 1, line.length()));
            if (line.substr(0, pos) == std::string("gradient_colors"))
                gradient_colors = std::stoi(line.substr(pos + 1, line.length()));
            if (line.substr(0, pos) == std::string("color_threshold"))
                color_threshold = std::stoi(line.substr(pos + 1, line.length()));
            if (line.substr(0, pos) == std::string("min_area"))
                min_area = std::stoi(line.substr(pos + 1, line.length())) * 1.0 / 100.0;
        }
    }
    else
    {
        std::cout << "Used default properties\n";
    }


    // Path to video or first image from group of images
    std::string path = (argv[1] != nullptr) ? argv[1] : "";
    draw_heatmap(path, frames_for_median, min_contour_area, usr_cols,
                  usr_rows,  gradient_colors, color_threshold, min_area );
    return 0;
}