#pragma once

#include <QWidget>
#include <QSlider>
#include <QTimer>
#include <vlc/vlc.h>

class VideoProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit VideoProgressBar(libvlc_media_player_t* player, QWidget* parent = nullptr);
    void setVisibleBasedOnMouseY(int mouseY, int videoHeight);

private:
    QSlider* slider;
    QTimer* updateTimer;
    libvlc_media_player_t* mediaPlayer;
    bool isDragging = false;

private slots:
    void updateSlider();
    void sliderReleased();
    void sliderPressed();

signals:
    void seekRequested(qint64 newTime);
};