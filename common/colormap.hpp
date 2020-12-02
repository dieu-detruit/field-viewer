#pragma once

#include <opencv2/opencv.hpp>

cv::Vec3b colormap(double x, double min, double max)
{
    constexpr int N = 5;
    const cv::Vec3b colors[N] = {{0, 0, 0}, {128, 0, 255}, {255, 0, 0}, {255, 128, 0}, {255, 255, 0}};
    const double width = (1.0 + 1.0e-5) / (N - 1);

    if (min == max) {
        return cv::Vec3b{0, 0, 0};
    }

    double t = (x - min) / (max - min);
    int stage = int(t / width);
    double alpha = (t - stage * width) / width;

    return colors[stage] + alpha * (colors[stage + 1] - colors[stage]);
}
