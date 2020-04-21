#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QObject>
#include <QThread>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <exception>

#include <mutex>
#include <condition_variable>

class CameraNotFoundException : public std::exception
{
public:
    virtual const char *what() const noexcept override
    {
        return "No camera found for given index";
    }
};

class ImageProvider : public QObject
{
    Q_OBJECT
public:
    explicit ImageProvider(QObject *parent = nullptr, int index = 0);
    ~ImageProvider();

    void start_shooting();
    void stop_shooting();

signals:
    void new_frame_ready(const cv::Mat&);

private:
    cv::VideoCapture cap;
    QThread *image_provider_exec_thread;

    bool thread_execution_finished;
    bool can_shoot_image;

    std::mutex camera_mutex;
    std::condition_variable camera_condition_variable;
};

#endif // IMAGEPROVIDER_H
