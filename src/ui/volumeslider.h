#pragma once

#include <QSlider>
#include <QObject>
#include "videoplayer.h"

class VolumeSliderController : public QObject
{
    Q_OBJECT

public:
    explicit VolumeSliderController(VideoPlayer* player, QWidget* parent = nullptr);

    QSlider* getSlider() const { return volumeSlider; }

private:
    QSlider* volumeSlider = nullptr;
    VideoPlayer* videoPlayer = nullptr;
};
