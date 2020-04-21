#include "imageprovider.h"

#include <opencv2/videoio.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/cvconfig.h>

#include <QThread>

#include "app_config.h"

#include <mutex>
#include <condition_variable>

ImageProvider::ImageProvider(QObject *parent, int index)
    : QObject(parent),
      thread_execution_finished(false),
      can_shoot_image(false)
{
    cap.open(index, cv::CAP_ANY);

    if (!cap.isOpened()) {
        throw CameraNotFoundException();
    }

    image_provider_exec_thread = QThread::create([this]() {
        cv::Mat frame;
        while (!thread_execution_finished) {
            std::unique_lock camera_thread_shared_lock(camera_mutex);
            camera_condition_variable.wait(camera_thread_shared_lock, [this]() {
                return can_shoot_image || thread_execution_finished;
            });
            while (can_shoot_image) {
                cap >> frame;
                cv::resize(frame, frame, cv::Size(FRAME_WIDTH, FRAME_HEIGHT), 0, 0, cv::INTER_AREA);
                // Third argument means flipping around y-axis
                cv::flip(frame, frame, 1);
                emit new_frame_ready(frame);
            }
        }
    });
    connect(image_provider_exec_thread, &QThread::finished, image_provider_exec_thread, &QThread::deleteLater);
    image_provider_exec_thread->start();
}

ImageProvider::~ImageProvider()
{
    thread_execution_finished = true;
    can_shoot_image = false;
    camera_condition_variable.notify_one();
    image_provider_exec_thread->wait();
}

void ImageProvider::start_shooting()
{
    can_shoot_image = true;
    camera_condition_variable.notify_one();
}

void ImageProvider::stop_shooting()
{
    can_shoot_image = false;
}
