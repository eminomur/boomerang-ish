#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpacerItem>

#include <QImage>
#include <opencv2/core.hpp>

#include "imageprovider.h"
#include "imagebuffer.h"
#include "frameplayer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Following error can be solved if following line is commented out
    // QMetaObject::connectSlotsByName: No matching signal for on_shoot_button_clicked()
private slots:
    void get_new_frame(const cv::Mat& new_frame);
    void on_shoot_button_clicked();
    void when_buffer_full();
    void on_discard_button_clicked();

private:
    Ui::MainWindow *ui;

    QWidget *central_widget;
    QVBoxLayout *outermost_vertical_layout;
    QHBoxLayout *horizontal_layout_for_save_and_discard_button;

    QPushButton *discard_button;
    QSpacerItem *horizontal_layout_spacer_item;
    QPushButton *save_button;

    QSpacerItem *vertical_layout_spacer_item_between_horizontal_layout_and_image_label;

    QLabel *image_label;
    QPushButton *shoot_button;

    ImageProvider *image_provider;
    ImageBuffer *image_buffer;
    FramePlayer *frame_player;

    ImageBufferStatus image_buffer_status;
};
#endif // MAINWINDOW_H
