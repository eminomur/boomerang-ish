#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QGroupBox>
#include <QRadioButton>

#include <QImage>
#include <QPixmap>

#include <QDebug>

#include "imageprovider.h"
#include "imagebuffer.h"
#include "frameplayer.h"

#include "app_config.h"

// Yapılacaklar:
// Menubar readme
// Uygulamaya geri tuşu koy buton vesaire süslemelerini yap, konumlarını düzelt
// Görünmez olan buton yerine kareler sergilenebilir
// Geri tuşu için bir buton yerine bir görsel, kutucuk da olabilir, araştır, ya da yazısız buton, arka planı ise geri tuşu resmi
// Bunun yanı sıra en aşağıda kareleri düzenleyecek araçlar olmalı

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Following argument is necessary to register the type used in the slot named get_new_frame
    qRegisterMetaType<cv::Mat>("const cv::Mat&");

    central_widget = new QWidget(this);
    this->setCentralWidget(central_widget);

    outermost_vertical_layout = new QVBoxLayout(central_widget);
    central_widget->setLayout(outermost_vertical_layout);

    horizontal_layout_for_save_and_discard_button = new QHBoxLayout;
    outermost_vertical_layout->addItem(horizontal_layout_for_save_and_discard_button);

    discard_button = new QPushButton("Discard", this);
    discard_button->setVisible(false);
    horizontal_layout_for_save_and_discard_button->addWidget(discard_button);

    horizontal_layout_spacer_item = new QSpacerItem(100, 0);
    horizontal_layout_for_save_and_discard_button->addSpacerItem(horizontal_layout_spacer_item);

    save_button = new QPushButton("Save", this);
    save_button->setVisible(false);
    horizontal_layout_for_save_and_discard_button->addWidget(save_button);

//    vertical_layout_spacer_item_between_horizontal_layout_and_image_label = new QSpacerItem(0, 50);
//    outermost_vertical_layout->addSpacerItem(vertical_layout_spacer_item_between_horizontal_layout_and_image_label);

    image_label = new QLabel(this);
    image_label->setFixedSize(QSize(FRAME_WIDTH, FRAME_HEIGHT));
    outermost_vertical_layout->addWidget(image_label);

    shoot_button = new QPushButton("Shoot", this);
    outermost_vertical_layout->addWidget(shoot_button);

//    outermost_vertical_layout->setAlignment(Qt::AlignTop);

    image_provider = new ImageProvider(this, 0);
    image_buffer = new ImageBuffer(this);
    frame_player = new FramePlayer(this);

    connect(image_provider, &ImageProvider::new_frame_ready, this, &MainWindow::get_new_frame);
    connect(frame_player, &FramePlayer::new_buffer_frame_ready, this, &MainWindow::get_new_frame);
    connect(shoot_button, &QPushButton::clicked, this, &MainWindow::on_shoot_button_clicked);
    connect(image_buffer, &ImageBuffer::buffer_full, this, &MainWindow::when_buffer_full);
    connect(discard_button, &QPushButton::clicked, this, &MainWindow::on_discard_button_clicked);

    // Start shooting immediately after everything is ready
    image_provider->start_shooting();
    // Specifies that there is no data in buffer
    image_buffer_status = ImageBufferStatus::EMPTY;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_shoot_button_clicked()
{
    if (image_buffer_status == ImageBufferStatus::DONE) {
        image_buffer->clean_buffer();
        image_provider->start_shooting();
    }
    image_buffer_status = RECORDING;
    shoot_button->setEnabled(false);
}

void MainWindow::get_new_frame(const cv::Mat& new_frame)
{
    image_label->setPixmap(QPixmap::fromImage(QImage(new_frame.clone().data, new_frame.cols, new_frame.rows, QImage::Format_BGR888)));
    if (image_buffer_status == ImageBufferStatus::RECORDING) {
        image_buffer->add_frame(new_frame.clone());
    }
}

void MainWindow::when_buffer_full()
{
    image_buffer_status = ImageBufferStatus::DONE;

    image_provider->stop_shooting();

    frame_player->set_buffer(image_buffer->data());
    frame_player->play();

    shoot_button->setVisible(false);
    discard_button->setVisible(true);
    save_button->setVisible(true);
}

void MainWindow::on_discard_button_clicked()
{
    frame_player->clean_buffer();
    image_buffer->clean_buffer();
    image_provider->start_shooting();

    image_buffer_status = ImageBufferStatus::EMPTY;

    shoot_button->setEnabled(true);
    shoot_button->setVisible(true);
    discard_button->setVisible(false);
    save_button->setVisible(false);
}
