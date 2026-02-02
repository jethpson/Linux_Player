#pragma once

#include <QWidget>
#include <QLabel>
#include <QPixmap>

class VideoPlayer;
class ClickableLabel;
class VolumeSliderController;

class IconController : public QObject
{
    Q_OBJECT

public:
    IconController(VideoPlayer* player, QWidget* parent, VolumeSliderController* vController);

    ClickableLabel* getPlayingIcon() const { return playingIcon; }
    ClickableLabel* getStoppedIcon() const { return stoppedIcon; }
    ClickableLabel* getForwardIcon() const { return forwardIcon; }
    ClickableLabel* getBackwardIcon() const { return backwardIcon; }
    ClickableLabel* getVolumeIcon() const { return volumeIcon; }
    ClickableLabel* getVolumeMuteIcon() const { return volumeMuteIcon; }

private:
    VideoPlayer* videoPlayer = nullptr;
    QWidget* parentWidget = nullptr;

    ClickableLabel* playingIcon = nullptr;
    ClickableLabel* stoppedIcon = nullptr;
    ClickableLabel* forwardIcon = nullptr;
    ClickableLabel* backwardIcon = nullptr;
    ClickableLabel* volumeIcon  = nullptr;
    ClickableLabel* volumeMuteIcon  = nullptr;

    VolumeSliderController* volumeController;

    void setupIcons();
    void setupConnections();
};
