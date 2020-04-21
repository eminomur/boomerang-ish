#include "imagebuffer.h"

#include "app_config.h"

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
