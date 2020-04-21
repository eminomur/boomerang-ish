#include "frameplayer.h"

#include "app_config.h"

#include <QThread>

#include <opencv2/highgui.hpp>

#include <mutex>
#include <condition_variable>

FramePlayer::FramePlayer(QObject *parent)
    : QObject(parent),
      buffer(nullptr),
      millisecond_between_frames(MILLISECONDS_IN_ONE_SECOND / FRAME_PER_SECOND),
      thread_execution_finished(false),
      play_frames(false)
{
    player_thread = QThread::create([this]() {
        while (!thread_execution_finished) {
            std::unique_lock player_thread_unique_lock(player_mutex);
            player_condition_variable.wait(player_thread_unique_lock, [this]() {
                return play_frames || thread_execution_finished;
            });
            while (play_frames) {
                for (auto it = buffer->begin(); it != buffer->end() && play_frames; ++it) {
                    emit new_buffer_frame_ready(*it);
                    QThread::msleep(static_cast<unsigned long>(millisecond_between_frames));
                }
                for (auto it = buffer->rbegin(); it != buffer->rend() && play_frames; ++it) {
                    emit new_buffer_frame_ready(*it);
                    QThread::msleep(static_cast<unsigned long>(millisecond_between_frames));
                }
            }
        }
    });
    connect(player_thread, &QThread::finished, player_thread, &QThread::deleteLater);
    player_thread->start();
}

FramePlayer::~FramePlayer()
{
    thread_execution_finished = true;
    play_frames = false;
    player_condition_variable.notify_one();
    player_thread->wait();
}

void FramePlayer::set_buffer(const std::vector<cv::Mat> *buffer_ptr)
{
    buffer = buffer_ptr;
}

void FramePlayer::clean_buffer()
{
    stop();
    std::lock_guard guard(player_mutex);
    buffer = nullptr;
}

void FramePlayer::play()
{
    play_frames = true;
    player_condition_variable.notify_one();
}

void FramePlayer::stop()
{
    play_frames = false;
}
