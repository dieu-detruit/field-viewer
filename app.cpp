#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "common/colormap.hpp"
#include "common/data.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv)
{

    auto [header, image] = loadfile("../data/focus.abs");

    cv::namedWindow("Viewer App", cv::WINDOW_NORMAL);

    cv::imshow("hoge", image);

    cv::waitKey(0);


    return 0;
}
