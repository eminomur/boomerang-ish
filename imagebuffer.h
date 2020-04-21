#ifndef IMAGEBUFFER_H
#define IMAGEBUFFER_H

#include <QObject>

#include <vector>
#include <opencv2/core.hpp>

enum ImageBufferStatus
{
    EMPTY,
    RECORDING,
    DONE
};

class ImageBuffer : public QObject
{
    Q_OBJECT
public:
    explicit ImageBuffer(QObject *parent = nullptr);

    void add_frame(const cv::Mat& frame);
    void clean_buffer();
    bool full() const;

    std::vector<cv::Mat> *data();

signals:
    void buffer_full();

private:
    std::vector<cv::Mat> frames;
};

#endif // IMAGEBUFFER_H
