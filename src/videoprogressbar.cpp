#include "videoprogressbar.h"
#include <QVBoxLayout>
#include <QDebug>

VideoProgressBar::VideoProgressBar(libvlc_media_player_t* player, QWidget* parent)
    : QWidget(parent), mediaPlayer(player)
{
    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, 1000);   // 0–1000 for simplicity
    slider->setSingleStep(1);
    slider->setPageStep(10);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5,5,5,5);
    layout->addWidget(slider);
    setLayout(layout);

    // Timer to update slider while playing
    updateTimer = new QTimer(this);
    updateTimer->setInterval(200); // 5 times/sec
    connect(updateTimer, &QTimer::timeout, this, &VideoProgressBar::updateSlider);
    updateTimer->start();

    // Handle dragging
    connect(slider, &QSlider::sliderPressed, this, &VideoProgressBar::sliderPressed);
    connect(slider, &QSlider::sliderReleased, this, &VideoProgressBar::sliderReleased);
}

void VideoProgressBar::updateSlider()
{
    if (isDragging || !mediaPlayer) return;

    libvlc_time_t current = libvlc_media_player_get_time(mediaPlayer);
    libvlc_time_t duration = libvlc_media_player_get_length(mediaPlayer);
    if (duration > 0)
    {
        int value = static_cast<int>((current * 1000) / duration);
        slider->setValue(value);
    }
}

void VideoProgressBar::sliderPressed()
{
    isDragging = true;
}

void VideoProgressBar::sliderReleased()
{
    isDragging = false;
    if (!mediaPlayer) return;

    int value = slider->value();
    libvlc_time_t duration = libvlc_media_player_get_length(mediaPlayer);
    libvlc_time_t newTime = (value * duration) / 1000;
    libvlc_media_player_set_time(mediaPlayer, newTime);

    emit seekRequested(newTime);
}

void VideoProgressBar::setVisibleBasedOnMouseY(int mouseY, int videoHeight)
{
    int normalOffsetY = -20;   // distance from bottom
    int hiddenOffsetY = 500;  // move down far off screen
    int offsetY;

    if (mouseY >= videoHeight - 150) {
        offsetY = normalOffsetY;
        this->show();
    } else {
        offsetY = -hiddenOffsetY;
        this->hide();
    }

    // Position it at the bottom center
    if (parentWidget()) {
        int x = (parentWidget()->width() - width()) / 2;
        int y = videoHeight - height() - offsetY;
        move(x, y);
    }
}
