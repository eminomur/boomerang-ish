#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>

#include <QMenu>
#include <QAction>

#include <QMessageBox>

#include <QImage>
#include <QPixmap>

#include <QDebug>

#include "imageprovider.h"
#include "imagebuffer.h"
#include "frameplayer.h"

#include "app_config.h"

// Yapılacaklar:
// Save button

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Following argument is necessary to register the type used in the slot named get_new_frame
    qRegisterMetaType<cv::Mat>("const cv::Mat&");

    initialize_menubar();
    initialize_central_widget();
    initialize_layouts_and_widgets();
    initialize_image_buffer();
    initialize_frame_player();
    initialize_image_provider();
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

void MainWindow::on_about_boomerangish_action_triggered()
{
    QMessageBox::about(this, "About Boomerangish", "An Implementation of Boomerang Application.\n"
                                                   "Created for fun (and because of boredom, a bit).\n"
                                                   "You are free to modify as you like.\n\n"
                                                   "Muhammed Emin ÖMÜR - Software Engineer");
}

void MainWindow::initialize_menubar()
{
    help_menu = new QMenu("Help", this->menuBar());
    this->menuBar()->addMenu(help_menu);

    about_boomerangish_action = new QAction("About Boomerangish", help_menu);
    help_menu->addAction(about_boomerangish_action);
    about_boomerangish_action->setShortcut(Qt::Key_F1);

    connect(about_boomerangish_action, &QAction::triggered, this, &MainWindow::on_about_boomerangish_action_triggered);
}

void MainWindow::initialize_central_widget()
{
    central_widget = new QWidget(this);
    this->setCentralWidget(central_widget);
}

void MainWindow::initialize_layouts_and_widgets()
{
    outermost_vertical_layout = new QVBoxLayout(central_widget);
    central_widget->setLayout(outermost_vertical_layout);

    horizontal_layout_for_save_and_discard_button = new QHBoxLayout;
    outermost_vertical_layout->addItem(horizontal_layout_for_save_and_discard_button);

    discard_button = new QPushButton("Discard", this);
    discard_button->setShortcut(Qt::Key_Escape);
    discard_button->setVisible(false);
    horizontal_layout_for_save_and_discard_button->addWidget(discard_button);

    horizontal_layout_spacer_item = new QSpacerItem(100, discard_button->size().height());
    horizontal_layout_for_save_and_discard_button->addSpacerItem(horizontal_layout_spacer_item);

    save_button = new QPushButton("Save", this);
    save_button->setShortcut(Qt::Key_Control | Qt::Key_S);
    save_button->setVisible(false);
    horizontal_layout_for_save_and_discard_button->addWidget(save_button);

    image_label = new QLabel(this);
    image_label->setFixedSize(QSize(FRAME_WIDTH, FRAME_HEIGHT));
    outermost_vertical_layout->addWidget(image_label);

    shoot_button = new QPushButton("Shoot", this);
    shoot_button->setShortcut(Qt::Key_Space);
    outermost_vertical_layout->addWidget(shoot_button);

    outermost_vertical_layout->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter | Qt::AlignCenter);

    connect(shoot_button, &QPushButton::clicked, this, &MainWindow::on_shoot_button_clicked);
    connect(discard_button, &QPushButton::clicked, this, &MainWindow::on_discard_button_clicked);
}

void MainWindow::initialize_image_buffer()
{
    image_buffer = new ImageBuffer(this);
    connect(image_buffer, &ImageBuffer::buffer_full, this, &MainWindow::when_buffer_full);
}

void MainWindow::initialize_frame_player()
{
    frame_player = new FramePlayer(this);
    connect(frame_player, &FramePlayer::new_buffer_frame_ready, this, &MainWindow::get_new_frame);
}

void MainWindow::initialize_image_provider()
{
    image_provider = new ImageProvider(this, 0);
    connect(image_provider, &ImageProvider::new_frame_ready, this, &MainWindow::get_new_frame);

    // Start shooting right after everything is ready
    image_provider->start_shooting();
    // Specifies that there is no data in the buffer
    image_buffer_status = ImageBufferStatus::EMPTY;
}
