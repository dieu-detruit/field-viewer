#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <tuple>

#include <vector>
#include <zlib.h>

#include <opencv2/opencv.hpp>

struct info_t {
    unsigned long N;
    unsigned long M;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
};

#if 0
auto loadImage(std::string filename)
{
    std::ifstream input(filename, std::ios::in | std::ios::binary);
    if (!input) {
        std::cerr << "No such file or directory: " << filename << std::endl;
        std::abort();
    }

    auto filesize = std::filesystem::file_size(std::filesystem::path(filename));  // Bytes
    std::vector<char> deflated_field(filesize);
    input.read((char*)deflated_field.data(), filesize);

    std::cout << filesize << std::endl;

    std::vector<double> field;
    std::size_t N;

    {
        z_stream z;

        z.zalloc = Z_NULL;
        z.zfree = Z_NULL;
        z.opaque = Z_NULL;
        if (inflateInit(&z) != Z_OK) {
            std::cerr << "Zlib Inflation Initialize Error" << std::endl;
            std::abort();
        }

        z.avail_in = filesize;
        z.next_in = (Bytef*)deflated_field.data();

        std::ptrdiff_t out_pos = 0;
        while (true) {
            field.reserve(field.capacity() + filesize);

            z.next_out = (Bytef*)(field.data() + out_pos);
            z.avail_out = filesize * sizeof(double);

            auto ret = inflate(&z, Z_NO_FLUSH);

            if (ret == Z_STREAM_END) {
                auto append_size = filesize * sizeof(double) - z.avail_out;
                field.erase(field.begin() + out_pos + append_size, field.end());
                break;
            }
            out_pos += filesize;
        }

        std::cout << "field.size(): " << field.size() << ", N: " << std::sqrt(field.size()) << std::endl;
        N = std::sqrt(field.size());

        if (deflateEnd(&z) != Z_OK) {
            std::cerr << "Zlib Inflation Error" << std::endl;
            std::abort();
        }
    }


    double min = *std::min_element(field.begin(), field.end()), max = *std::max_element(field.begin(), field.end());

    cv::Mat image = cv::Mat::zeros(N, N, CV_8UC3);
    auto image_itr = image.begin<cv::Vec3b>();
    auto field_itr = field.begin();

    for (; field_itr != field.end() and image_itr != image.end<cv::Vec3b>(); ++field_itr, ++image_itr) {
        *image_itr = colormap(*field_itr, min, max);
    }

    return std::make_tuple(image, field);
}
#endif

#if 1
auto loadImage(std::string filename)
{
    std::ifstream input(filename, std::ios::in | std::ios::binary);
    if (!input) {
        std::cerr << "No such file or directory: " << filename << std::endl;
        std::abort();
    }

    auto filesize = std::filesystem::file_size(std::filesystem::path(filename));
    std::size_t size = filesize / sizeof(double);
    std::size_t N = std::sqrt(size);

    std::cout << N << " * " << N << std::endl;

    std::vector<double> field(size);
    input.read((char*)field.data(), size * sizeof(double));

    double min = *std::min_element(field.begin(), field.end()), max = *std::max_element(field.begin(), field.end());

    cv::Mat image = cv::Mat::zeros(N, N, CV_8UC3);
    auto image_itr = image.begin<cv::Vec3b>();
    auto field_itr = field.begin();

    for (; field_itr != field.end() and image_itr != image.end<cv::Vec3b>(); ++field_itr, ++image_itr) {
        *image_itr = colormap(*field_itr, min, max);
    }

    return std::make_tuple(image, field);
}
#endif

auto loadInfo(std::string filename)
{
    std::ifstream input(filename);
    if (!input) {
        std::cerr << "No such file or directory: " << filename << std::endl;
        std::abort();
    }

    info_t info;
    input >> info.N >> info.M >> info.xmin >> info.xmax >> info.ymin >> info.ymax;

    return info;
}
