#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "common/colormap.hpp"
#include "common/data.hpp"

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#define CVUI_IMPLEMENTATION
#include <cvui.h>

#define WINDOW_NAME "Viewer App"

const cv::Point image_pos{130, 430};
constexpr int srcimage_width = 512;
constexpr int image_width = 512;
constexpr int profile_width = 100;
constexpr int normal_margin = 10;
constexpr int trackbarWidth = 260;
const cv::Point rgb_pos{20, 50};
const cv::Point hsv_pos{342, 50};
constexpr int rgb_height = 250;
constexpr int rgb_width = 300;
constexpr int rgb_margin = 20;

int main(int argc, char** argv)
{

    if (argc < 2) {
        std::cout << "Usage: plot [filename]" << std::endl;
        return 1;
    }

    std::string input_filename;
    input_filename = argv[1];
    std::cout << "Input file: " << input_filename << std::endl;

    auto [image, field] = loadImage(input_filename);

    //auto info = loadInfo("../data/focus.info");

    std::size_t N = image.cols;
    int shrink_ratio = N / 512;
    std::vector<double> field_transposed(field.size());
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            field_transposed[N * i + j] = field[N * j + i];
        }
    }
    //double field_norm = *std::max_element(field.begin(), field.end()) - *std::min_element(field.begin(), field.end());

    cv::Mat image_original;
    cv::resize(image, image_original, cv::Size(512, 512));
    cv::Mat image_view = image_original.clone();
    cv::Mat image_cursor = image_original.clone();

    cv::Mat hprofile_image = cv::Mat(cv::Size(image_width, profile_width), CV_8UC3, cv::Scalar{255, 255, 255});
    cv::Mat vprofile_image_rot = cv::Mat(cv::Size(image_width, profile_width), CV_8UC3, cv::Scalar{255, 255, 255});
    cv::Mat vprofile_image;

    cv::Mat frame = cv::Mat(cv::Size(662, 1000), CV_8UC3);

    int profile_x = image_width / 2, profile_y = image_width / 2;

    cvui::init(WINDOW_NAME, 20);

    while (true) {

        if (cvui::button(frame, frame.cols - 80, frame.rows - 40, "&Quit")) {
            break;
        }

        // RGB調整Window
        {
            cvui::window(frame, rgb_pos.x, rgb_pos.y, rgb_width, rgb_height, "RGB adjust");

            cvui::beginColumn(frame, rgb_pos.x + rgb_margin, 100, -1, -1, 10);
            static double rgb[3]{1., 1., 1};
            bool rgbModified = false;

            // Trackbar accept a pointer to a variable that controls their value
            // They return true upon edition
            if (cvui::trackbar(trackbarWidth, &rgb[0], (double)0., (double)2., 2, "%3.02Lf")) {
                rgbModified = true;
            }
            if (cvui::trackbar(trackbarWidth, &rgb[1], (double)0., (double)2., 2, "%3.02Lf")) {
                rgbModified = true;
            }
            if (cvui::trackbar(trackbarWidth, &rgb[2], (double)0., (double)2., 2, "%3.02Lf")) {
                rgbModified = true;
            }

            cvui::space(2);
            cvui::printf(0.35, 0xcccccc, "   RGB: %3.02lf,%3.02lf,%3.02lf", rgb[0], rgb[1], rgb[2]);

            if (rgbModified) {
                std::vector<cv::Mat> channels(3);
                cv::split(image_original, channels);
                for (int c = 0; c < 3; c++) {
                    channels[c] = channels[c] * rgb[c];
                }
                cv::merge(channels, image_view);
            }
            cvui::endColumn();
        }

        // HSV調整Window
        {
            cvui::window(frame, hsv_pos.x, hsv_pos.y, rgb_width, rgb_height, "HSV adjust");
            cvui::beginColumn(frame, hsv_pos.x + rgb_margin, 100, -1, -1, 10);
            static double hsv[3]{1., 1., 1};
            bool hsvModified = false;

            if (cvui::trackbar(trackbarWidth, &hsv[0], (double)0., (double)2., 2, "%3.02Lf")) {
                hsvModified = true;
            }
            if (cvui::trackbar(trackbarWidth, &hsv[1], (double)0., (double)2., 2, "%3.02Lf")) {
                hsvModified = true;
            }
            if (cvui::trackbar(trackbarWidth, &hsv[2], (double)0., (double)2., 2, "%3.02Lf")) {
                hsvModified = true;
            }

            cvui::space(2);
            cvui::printf(0.35, 0xcccccc, "   HSV: %3.02lf,%3.02lf,%3.02lf", hsv[0], hsv[1], hsv[2]);

            if (hsvModified) {
                cv::Mat hsvMat;
                cv::cvtColor(image_original, hsvMat, cv::COLOR_BGR2HSV);
                std::vector<cv::Mat> channels(3);
                cv::split(hsvMat, channels);

                for (int c = 0; c < 3; c++) {
                    channels[c] = channels[c] * hsv[c];
                }

                cv::merge(channels, hsvMat);
                cv::cvtColor(hsvMat, image_view, cv::COLOR_HSV2BGR);
            }
            cvui::endColumn();
        }

        // カーソル表示
        int image_mouse_status = cvui::iarea(image_pos.x, image_pos.y, image_width, image_width);
        if (image_mouse_status == cvui::DOWN) {
            cv::Point cursor = cvui::mouse();
            profile_x = cursor.x - image_pos.x;
            profile_y = cursor.y - image_pos.y;
            cv::rectangle(hprofile_image, cv::Point(0, 0), cv::Point(image_width - 1, profile_width - 1), cv::Scalar{255, 255, 255}, cv::FILLED);
            cv::rectangle(vprofile_image_rot, cv::Point(0, 0), cv::Point(image_width - 1, profile_width - 1), cv::Scalar{255, 255, 255}, cv::FILLED);
        }

        std::cout << "profile_x: " << profile_x << ", profile_y: " << profile_y << std::endl;

        image_view.copyTo(image_cursor);
        cv::line(image_cursor, cv::Point(profile_x, 0), cv::Point(profile_x, image_width - 1), cv::Scalar{255, 255, 255});
        cv::line(image_cursor, cv::Point(0, profile_y), cv::Point(image_width - 1, profile_y), cv::Scalar{255, 255, 255});

        // 画像表示
        cvui::rect(frame, image_pos.x - 1, image_pos.y - 1, image_width + 2, image_width + 2, 0xffffffff);
        cvui::image(frame, image_pos.x, image_pos.y, image_cursor);

        // プロファイル
        std::vector<double> hprofile{field.begin() + N * shrink_ratio * profile_y, field.begin() + N * (shrink_ratio * profile_y + 1) - 1};
        std::vector<double> vprofile{field_transposed.begin() + N * shrink_ratio * profile_x, field_transposed.begin() + N * (shrink_ratio * profile_x + 1) - 1};
        //double hprofile_norm = *std::max_element(hprofile.begin(), hprofile.end()) - *std::min_element(hprofile.begin(), hprofile.end());
        //double vprofile_norm = *std::max_element(vprofile.begin(), vprofile.end()) - *std::min_element(vprofile.begin(), vprofile.end());

        //int hprofile_width = profile_width * hprofile_norm / field_norm;
        //int vprofile_width = profile_width * vprofile_norm / field_norm;

        //cvui::sparkline(hprofile_image, hprofile, 0, profile_width - hprofile_width, image_width, hprofile_width, 0xff0000ff);
        //cvui::sparkline(vprofile_image_rot, vprofile, 0, profile_width - vprofile_width, image_width, vprofile_width, 0xff0000ff);
        cvui::sparkline(hprofile_image, hprofile, 0, 0, image_width, profile_width, 0xff0000ff);
        cvui::sparkline(vprofile_image_rot, vprofile, 0, 0, image_width, profile_width, 0xff0000ff);
        cv::rotate(vprofile_image_rot, vprofile_image, cv::ROTATE_90_COUNTERCLOCKWISE);
        cvui::image(frame, image_pos.x, image_pos.y - profile_width - normal_margin, hprofile_image);
        cvui::image(frame, image_pos.x - profile_width - normal_margin, image_pos.y, vprofile_image);

        // 画面更新
        cvui::update();
        cv::imshow(WINDOW_NAME, frame);
    }

    return 0;
}
