#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <set>
#include <vector>
#include <algorithm>

using namespace cv;
using namespace std;
using M2D = vector<vector<int>>;
using vi = vector<int>;

void file_print(vi arr, int n, string filename)
{
    ofstream out(filename, ios_base::app);
    out << "Region with high workload: ";
    for (int j = 0; j < n; j++)
    {
        out << arr[j] << ' ';
    }
    out << '\n';
    out.close();
}

M2D gradient(int colors_amount)
{
    vi red{255, 0, 0};
    vi yellow{255, 255, 0};
    vi green{0, 255, 0};
    vi blue{0, 0, 255};
    int in_one = colors_amount / 3;
    M2D res;
    // red -> yellow
    for (int i = 0; i < in_one; i++)
    {
        res.push_back({255, i * 255 / (in_one - 1), 0});
    }
    // yellow -> green
    for (int i = 0; i < in_one; i++)
    {
        res.push_back({i * 255 / (in_one - 1), 255, 0});
    }
    // green -> blue
    for (int i = 0; i < in_one; i++)
    {
        res.push_back({0, 255 - i * 255 / (in_one - 1), i * 255 / (in_one - 1)});
    }
    return res;
}

vi count_states(M2D& arr)
{
    set<int> res;
    for (int i = 0; i < arr.size(); i++)
    {
        for (int j = 0; j < arr[0].size(); j++)
        {
            res.insert(arr[i][j]);
        }
    }
    return vi(res.begin(), res.end());
}


int compute_median_one_channel(vector<int> elements)
{
  nth_element(elements.begin(), elements.begin()+elements.size()/2, elements.end());

  return elements[elements.size()/2];
}

Mat compute_median(vector<Mat> vec)
{

  Mat medianImg(vec[0].rows, vec[0].cols, vec[0].type(), Scalar(0, 0, 0));

  for (int row = 0; row < vec[0].rows; row++)
  {
    for (int col = 0; col < vec[0].cols; col++)
    {
      vi elements_B;
      vi elements_G;
      vi elements_R;

      for(int imgNumber = 0; imgNumber < vec.size(); imgNumber++)
      {
        int B = vec[imgNumber].at<Vec3b>(row, col)[0];
        int G = vec[imgNumber].at<Vec3b>(row, col)[1];
        int R = vec[imgNumber].at<Vec3b>(row, col)[2];

        elements_B.push_back(B);
        elements_G.push_back(G);
        elements_R.push_back(R);
      }

      medianImg.at<Vec3b>(row, col)[0] = compute_median_one_channel(elements_B);
      medianImg.at<Vec3b>(row, col)[1] = compute_median_one_channel(elements_G);
      medianImg.at<Vec3b>(row, col)[2] = compute_median_one_channel(elements_R);
    }
  }
  return medianImg;
}


vector<pair<int, int>> check_areas(Mat &mat, int y, int x, int y_p, int x_p,
                                  vi threshold, M2D& gradient,
                                  float min_area, string regions_file)
{
    vector<pair<int, int>> res;
    for (int i = 0; i < y; i += y_p)
    {
        for (int j = 0; j < x; j += x_p)
        {
            float area = 0.0;
            for (int i1 = i; i1 < (i / y_p + 1) * y_p; i1++)
            {
                for (int j1 = j; j1 < (j / x_p + 1) * x_p; j1++)
                {
                    // Vec3b -> vector<int>
                    vi for_f(3);
                    Vec3b now = mat.at<Vec3b>(i1, j1);
                    for_f[0] = now[0];
                    for_f[1] = now[1];
                    for_f[2] = now[2];

                    // (now_color >= border_color) => area++
                    auto f = find(gradient.begin(), gradient.end(), for_f);
                    auto s = find(gradient.begin(), gradient.end(), threshold);
                    if (f - gradient.begin() >= s - gradient.begin())
                        area++;
                }
            }
            if (area / (y_p * x_p * 1.0) >= min_area)
            {
                res.push_back(make_pair(i, j));
                file_print({i / y_p + 1, j / x_p + 1}, 2, regions_file);
            }
        }
    }
    return res;
}


int draw_heatmap(string path, int frames_for_median, int min_contour_area, int usr_cols,
                 int usr_rows, int gradient_colors, int color_threshold, float min_area )
{
    // Clear file with coords of regions
    string regions_file = "regions.txt";
    ofstream ofs;
    ofs.open(regions_file, ofstream::out | ofstream::trunc);
    ofs.close();

    // Create a VideoCapture object and open the input file
    VideoCapture cap(path);
    // CAP_MODE_BGR = 0
    cap.set(CAP_PROP_MODE, 0);
    vector<Mat> frames;
    if(!cap.isOpened())
    {
        cout << "Couldn't open file\n";
        return 0;
    }

    // Pass several frames
    for (int i = 0; i < frames_for_median; i++)
    {
        Mat now_frame;
        cap >> now_frame;
        frames.push_back(now_frame);
    }

    //
    M2D dest(frames[0].rows, vi(frames[0].cols, 0));
    Mat median_frame = compute_median(frames);
    int y_p = frames[0].rows / usr_rows;
    int x_p = frames[0].cols / usr_cols;
    vector<pair<int, int>> areas;
    Mat heatmap = Mat::zeros((int)dest.size(), (int)dest[0].size(), frames[0].type());
    M2D colors = gradient(gradient_colors);

    //  Reset frame number to 0
    cap.release();
    cap.open(path);
    cap.set(CAP_PROP_MODE, 0);

    // Convert background to grayscale
    Mat gray_median_frame;
    cvtColor(median_frame, gray_median_frame, COLOR_BGR2GRAY);

    // Loop over all frames
    Mat frame;
    int frame_counter = 1;
    while(true)
    {
        // Read frame
        cap >> frame;

        if (frame.empty())
            break;

        Mat gframe;

        // Convert current frame to grayscale
        cvtColor(frame, gframe, COLOR_BGR2GRAY);

        // Calculate absolute difference of current frame and the median frame
        Mat dframe;
        absdiff(gframe, gray_median_frame, dframe);

        // Threshold to binarize
        threshold(dframe, dframe, 30, 255, THRESH_BINARY);

        // Morphology method
        Mat kernel = getStructuringElement(MORPH_ELLIPSE, {3, 3});
        morphologyEx(dframe, dframe, MORPH_OPEN, kernel);

        // Find contours
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        vector<vector<Point> > rect_contours;

        findContours(dframe, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        for (auto cntr : contours)
        {
            // Drawing rect according to contours
            if (!(contourArea(cntr) > min_contour_area))
                continue;
            Rect rect = boundingRect(cntr);
            rectangle(frame, {rect.x, rect.y},
                     {rect.x + rect.width, rect.y + rect.height}, {0, 255, 0}, 3);

            // Filling dest for building heatmap
            for (int i = rect.y; i < min(rect.y + rect.height, frame.rows); i++)
            {
                for (int j = rect.x; j < min(rect.x + rect.width, frame.cols); j++)
                {
                    dest[i][j]++;
                }
            }
        }

        // Draw areas with high workload
        if (!areas.empty())
        {
            for (auto coord : areas)
            {
                rectangle(frame, {coord.second, coord.first},
                         {coord.second + x_p, coord.first + y_p}, {0, 0, 255}, 1);
            }
        }

        // Display image
        imshow("frame", frame);

        // Keyboard action
        char c = (char)waitKey(25);

        if (c == 27) // ESC button
            break;
        else if (c == 'h')
        {
            // Define unique states of dest elements
            vi states = count_states(dest);
            sort(states.begin(), states.end());

            // Draw heatmap
            for (int i = 0; i < dest.size(); i++)
            {
                for (int j = 0; j < dest[0].size(); j++)
                {
                    int sts_clrs = (int) (states.size() / colors.size());
                    int place = (sts_clrs != 0) ? dest[i][j] / sts_clrs : 0;
                    vi now_color = colors[(place >= gradient_colors) ? gradient_colors - 1 : place];
                    heatmap.at<Vec3b>(i, j)[0] = now_color[0];
                    heatmap.at<Vec3b>(i, j)[1] = now_color[1];
                    heatmap.at<Vec3b>(i, j)[2] = now_color[2];
                }
            }
            // Display and save image
            imshow("Heatmap", heatmap);
            imwrite("img" + to_string(frame_counter) + ".jpg", heatmap);

            // Define areas with high workload
            areas = check_areas(heatmap, frame.rows, frame.cols,
                               y_p, x_p,
                               colors[color_threshold], colors, min_area,
                               regions_file);
        }
        else if (c == 'm')
        {
            // Display median frame
            imshow("MedianFrame", median_frame);
        }
        frame_counter++;
    }
    // Work out last heatmap
    imshow("Heatmap", heatmap);
    imwrite("img_final.jpg", heatmap);

    cap.release();
    return 0;
}