#include "volumeslider.h"
#include "videoplayer.h"
#include "clickablelabel.h"
#include <QDebug>

VolumeSliderController::VolumeSliderController(VideoPlayer* player, QWidget* parent)
    : QObject(parent), videoPlayer(player), volumeSlider(nullptr), isMuted(false), lastVolume(50)
{

}

void VolumeSliderController::setSlider(QSlider* slider)
{
    volumeSlider = slider;
    if(volumeSlider)
    {
        // Connect the existing slider to update video volume and icons
        connect(volumeSlider, &QSlider::valueChanged, this, [this](int value){
            if(!videoPlayer) return;

            videoPlayer->setVolume(value);
            

            if(value == 0)
            {
                if(volumeIcon) volumeIcon->hide();
                if(volumeMuteIcon) volumeMuteIcon->show();
            }
            else if (isMuted)
            {
                if(volumeIcon) volumeIcon->hide();
                if(volumeMuteIcon) volumeMuteIcon->show();
            }
            else
            {
                if(volumeMuteIcon) volumeMuteIcon->hide();
                if(volumeIcon) volumeIcon->show();
            }
        });
    }
}

void VolumeSliderController::toggleMute()
{
    isMuted = !isMuted;

    if(volumeSlider)
    {
        if(isMuted)
            lastVolume = volumeSlider->value();

        volumeSlider->setValue(isMuted ? 0 : lastVolume);
    }

    updateIcon();
}

void VolumeSliderController::setSliderEnabled(bool enabled)
{
    if (!volumeSlider)
        return;

    volumeSlider->setEnabled(enabled);
}


void VolumeSliderController::updateIcon()
{
    if(!volumeIcon || !volumeMuteIcon) return;

    if(isMuted || (volumeSlider && volumeSlider->value() == 0))
    {
        volumeIcon->hide();
        volumeMuteIcon->show();
    }
    else
    {
        volumeMuteIcon->hide();
        volumeIcon->show();
    }
}