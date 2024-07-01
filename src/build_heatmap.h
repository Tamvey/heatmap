#pragma once

#include <string>

int draw_heatmap(std::string path, int frames_for_median, int min_contour_area, int usr_cols,
                  int usr_rows,  int gradient_colors, int color_threshold, float min_area);
