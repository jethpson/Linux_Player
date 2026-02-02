#pragma once

#include <QObject>
#include <QSlider>
#include "videoplayer.h"
#include "clickablelabel.h"

class VolumeSliderController : public QObject
{
    Q_OBJECT
public:
    // Constructor: do not create a slider here
    explicit VolumeSliderController(VideoPlayer* player, QWidget* parent = nullptr);
    
    void updateIcon();
    int getLastVolume() const { return lastVolume; }
    void setSliderValue(int value)
    {
        if (volumeSlider)
            volumeSlider->setValue(value);
    }

    // Link an existing slider to this controller
    void setSlider(QSlider* slider);

    void toggleMute();

    ClickableLabel* volumeIcon = nullptr;
    ClickableLabel* volumeMuteIcon = nullptr;

private:
    VideoPlayer* videoPlayer = nullptr;
    QSlider* volumeSlider = nullptr;
    int lastVolume = 50;
    bool isMuted = false;
};