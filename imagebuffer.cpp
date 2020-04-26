#include "imagebuffer.h"

#include "app_config.h"

#include <opencv2/videoio.hpp>

ImageBuffer::ImageBuffer(QObject *parent)
    : QObject(parent)
{
    frames.reserve(FRAME_BUFFER_SIZE);
}

void ImageBuffer::add_frame(const cv::Mat& frame)
{
    frames.push_back(frame);
    if (full()) {
        emit buffer_full();
    }
}

void ImageBuffer::clean_buffer()
{
    frames.clear();
}

bool ImageBuffer::full() const
{
    return frames.size() == FRAME_BUFFER_SIZE;
}

std::vector<cv::Mat> *ImageBuffer::data()
{
    return &frames;
}


void ImageBuffer::export_buffer(const std::string &file_name)
{
    cv::VideoWriter writer(file_name, cv::VideoWriter::fourcc('m', 'p', '4', 'v'), FRAME_PER_SECOND, cv::Size(FRAME_WIDTH, FRAME_HEIGHT));

    for (auto frames_forward_iterator = frames.begin();
         frames_forward_iterator != frames.end();
         ++frames_forward_iterator) {
        writer << *frames_forward_iterator;
    }

    for (auto frames_backward_iterator = frames.rbegin();
         frames_backward_iterator != frames.rend();
         ++frames_backward_iterator) {
        writer << *frames_backward_iterator;
    }
}
