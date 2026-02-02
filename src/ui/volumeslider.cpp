#include "volumeslider.h"

VolumeSliderController::VolumeSliderController(VideoPlayer* player, QWidget* parent)
    : QObject(parent), videoPlayer(player)
{
    volumeSlider = new QSlider(Qt::Horizontal, parent);

    // Slider range and default value
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);  // Max volume default

    // Recommended size for horizontal slider
    volumeSlider->setFixedWidth(200);
    volumeSlider->setFixedHeight(60);

    // Connect slider to video volume
    connect(volumeSlider, &QSlider::valueChanged, this, [this](int value){
        if(videoPlayer) videoPlayer->setVolume(value);
    });
}
