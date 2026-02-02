#pragma once

#include <QWidget>
#include <QLabel>
#include <QPixmap>

class VideoPlayer;
class ClickableLabel;

class IconController : public QObject
{
    Q_OBJECT

public:
    IconController(VideoPlayer* player, QWidget* parent = nullptr);

    ClickableLabel* getPlayingIcon() const { return playingIcon; }
    ClickableLabel* getStoppedIcon() const { return stoppedIcon; }
    ClickableLabel* getForwardIcon() const { return forwardIcon; }
    ClickableLabel* getBackwardIcon() const { return backwardIcon; }

private:
    VideoPlayer* videoPlayer = nullptr;
    QWidget* parentWidget = nullptr;

    ClickableLabel* playingIcon = nullptr;
    ClickableLabel* stoppedIcon = nullptr;
    ClickableLabel* forwardIcon = nullptr;
    ClickableLabel* backwardIcon = nullptr;

    void setupIcons();
    void setupConnections();
};
