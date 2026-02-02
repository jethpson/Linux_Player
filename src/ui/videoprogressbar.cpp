#include "videoprogressbar.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QEvent>
#include <QMouseEvent>

VideoProgressBar::VideoProgressBar(libvlc_media_player_t* player, QWidget* parent)
    : QWidget(parent), mediaPlayer(player)
{
    setFixedHeight(20); // height fixed

    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0, 1000);
    slider->setSingleStep(1);
    slider->setPageStep(10);

    // Layout for automatic scaling
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(slider);

    // Timer to update slider from VLC
    updateTimer = new QTimer(this);
    updateTimer->setInterval(200);
    connect(updateTimer, &QTimer::timeout, this, &VideoProgressBar::updateSlider);
    updateTimer->start();

    // Handle dragging the handle
    connect(slider, &QSlider::sliderPressed, this, &VideoProgressBar::sliderPressed);
    connect(slider, &QSlider::sliderReleased, this, &VideoProgressBar::sliderReleased);

    // Handle clicks anywhere on the slider
    slider->installEventFilter(this);
}

bool VideoProgressBar::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == slider && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton && mediaPlayer)
        {

            // Prepare style option to get the handle rect
            QStyleOptionSlider opt;
            opt.initFrom(slider);
            opt.orientation = slider->orientation();
            opt.minimum = slider->minimum();
            opt.maximum = slider->maximum();
            opt.sliderPosition = slider->value();
            opt.sliderValue = slider->value();
            opt.pageStep = slider->pageStep();

            // Check if click is on the handle
            QRect handleRect = slider->style()->subControlRect(
                QStyle::CC_Slider,
                &opt,
                QStyle::SC_SliderHandle,
                slider
            );

            // If the click is NOT on the handle, treat as click-to-seek
            if (!handleRect.contains(mouseEvent->position().toPoint()))
            {
                isSeeking = true; // block updateSlider temporarily

                int newValue = slider->style()->sliderValueFromPosition(
                    slider->minimum(),
                    slider->maximum(),
                    mouseEvent->position().x(),
                    slider->width()
                );
                slider->setValue(newValue);

                libvlc_time_t duration = libvlc_media_player_get_length(mediaPlayer);
                libvlc_time_t newTime = (newValue * duration) / slider->maximum();
                libvlc_media_player_set_time(mediaPlayer, newTime);
                emit seekRequested(newTime);

                isSeeking = false;
                return true; // event handled
            }
            // else: click is on handle, let normal dragging happen
        }
    }
    return QWidget::eventFilter(obj, event);
}

void VideoProgressBar::updateSlider()
{
    if (!mediaPlayer || isDragging || isSeeking) return;

    libvlc_time_t current = libvlc_media_player_get_time(mediaPlayer);
    libvlc_time_t duration = libvlc_media_player_get_length(mediaPlayer);

    if (duration > 0)
    {
        int value = static_cast<int>((current * 1000) / duration);
        slider->setValue(value);
    }
}

void VideoProgressBar::setPlayer(libvlc_media_player_t* player)
{
    mediaPlayer = player;
    if (!updateTimer->isActive())
        updateTimer->start(200);

    qDebug() << "[VideoProgressBar] Timer started for mediaPlayer" << mediaPlayer;
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