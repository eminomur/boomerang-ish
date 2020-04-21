#ifndef FRAMEPLAYER_H
#define FRAMEPLAYER_H

#include <QObject>
#include <QThread>

#include <vector>
#include <mutex>
#include <condition_variable>

#include <opencv2/core.hpp>

class FramePlayer : public QObject
{
    Q_OBJECT
public:
    explicit FramePlayer(QObject *parent = nullptr);
    ~FramePlayer();

    void set_buffer(const std::vector<cv::Mat> *buffer_ptr);
    void clean_buffer();

    void play();
    void stop();

signals:
    void new_buffer_frame_ready(const cv::Mat&);

private:
    const std::vector<cv::Mat> *buffer;
    int millisecond_between_frames;

    QThread *player_thread;

    bool thread_execution_finished;
    bool play_frames;

    std::mutex player_mutex;
    std::condition_variable player_condition_variable;
};

#endif // FRAMEPLAYER_H
