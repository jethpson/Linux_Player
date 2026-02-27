#pragma once

#include <QObject>
#include <QSlider>
#include "videoplayer.h"
#include "clickablelabel.h"

class VolumeSliderController : public QObject
{
    Q_OBJECT
public:

    explicit VolumeSliderController(VideoPlayer* player, QWidget* parent = nullptr);
    
    void updateIcon();
    int getLastVolume() const { return lastVolume; }
    bool getIsMuted() const { return isMuted; }
    void setSliderEnabled(bool enabled);
    void setSliderValue(int value)
    {
        if (volumeSlider)
            volumeSlider->setValue(value);
    }

    void setSlider(QSlider* slider);

    void toggleMute();

    ClickableLabel* volumeIcon = nullptr;
    ClickableLabel* volumeMuteIcon = nullptr;
    QSlider* volumeSlider = nullptr;

private:
    VideoPlayer* videoPlayer = nullptr;
    int lastVolume = 50;
    bool isMuted = false;
};