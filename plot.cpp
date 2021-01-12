#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "common/colormap.hpp"
#include "common/data.hpp"

#include <opencv2/opencv.hpp>

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage: plot [filename]" << std::endl;
        return 1;
    }

    std::string input_filename;
    std::string output_filename;

    input_filename = argv[1];

    if (argc == 2) {
        output_filename = input_filename + ".png";
    } else {
        output_filename = argv[2];
    }

    std::cout << "Input file: " << input_filename << std::endl;

    auto [image, field] = loadImage(input_filename);

    std::cout << "Output file: " << output_filename << std::endl;

    cv::imwrite(output_filename.data(), image);

    return 0;
}
