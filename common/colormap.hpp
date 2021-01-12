#pragma once

#include <opencv2/opencv.hpp>

const cv::Vec3b black{0, 0, 0};
const cv::Vec3b purple{255, 0, 128};
const cv::Vec3b red{0, 0, 255};
const cv::Vec3b orange{0, 128, 255};
const cv::Vec3b yellow{0, 255, 255};


cv::Vec3b colormap(double x, double min, double max)
{
    constexpr int N = 5;
    const cv::Vec3b colors[N] = {black, purple, red, orange, yellow};
    const double width = (1.0 + 1.0e-5) / (N - 1);

    if (min == max) {
        return black;
    }

    double t = (x - min) / (max - min);
    int stage = int(t / width);
    double alpha = (t - stage * width) / width;

    return (1.0 - alpha) * colors[stage] + alpha * colors[stage + 1];
}
