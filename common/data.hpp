#pragma once

#include <fstream>
#include <string>
#include <tuple>

#include <opencv2/opencv.hpp>

struct header_t {
    unsigned long N : 64;
    unsigned long M : 64;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
};

auto loadfile(std::string filename)
{
    header_t header;

    std::ifstream input(filename, std::ios::in | std::ios::binary);
    if (!input) {
        std::cerr << "No such file or directory: " << filename << std::endl;
        std::abort();
    }

    input.read((char*)&header, sizeof(header));
    std::cout << header.N << ' ' << header.M << ' ' << header.xmin << ' ' << header.xmax << ' ' << header.ymin << ' ' << header.ymax << std::endl;

    std::vector<double> field(header.N * header.M);
    input.read((char*)field.data(), header.N * header.M * sizeof(double));

    double min = *std::min_element(field.begin(), field.end()), max = *std::max_element(field.begin(), field.end());

    cv::Mat image = cv::Mat::zeros(header.N, header.M, CV_8UC3);
    auto image_itr = image.begin<cv::Vec3b>();
    auto field_itr = field.begin();

    for (; field_itr != field.end() and image_itr != image.end<cv::Vec3b>(); ++field_itr, ++image_itr) {
        *image_itr = colormap(*field_itr, min, max);
    }

    return std::make_tuple(header, image);
}
