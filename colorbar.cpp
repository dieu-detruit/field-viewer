#include <iostream>

#include <opencv2/opencv.hpp>

#include "common/colormap.hpp"

int main()
{
    cv::Mat image = cv::Mat::zeros(100, 500, CV_8UC3);

    for (int j = 0; j < 500; ++j) {
        auto color = colormap(j / 500.0, 0.0, 1.0);
        for (int i = 0; i < 100; ++i) {
            image.at<cv::Vec3b>(i, j) = color;
        }
    }

    cv::imwrite("colorbar.png", image);

    return 0;
}
