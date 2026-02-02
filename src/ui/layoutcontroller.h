#pragma once

#include <QObject>
#include <QWidget>
#include <QTimer>
#include <QMenuBar>
#include <QMouseEvent>

class VideoPlayer;
class QSlider;
class ClickableLabel;
class VideoProgressBar;

class LayoutController : public QObject
{
    Q_OBJECT

public:
    LayoutController(VideoPlayer* player,
                     QWidget* parent,
                     QSlider* volumeSlider,
                     VideoProgressBar* progressBar,
                     ClickableLabel* playingIcon,
                     ClickableLabel* stoppedIcon,
                     ClickableLabel* forwardIcon,
                     ClickableLabel* backwardIcon,
                     QMenuBar* menuBar);

    void handleResize();
    void handleMouseMove(int mouseY);
    void handleFullScreenChange(bool fullScreen);
    void updatePositions();

    void handleClick();

private:
    VideoPlayer* videoPlayer = nullptr;
    QWidget* parentWidget = nullptr;
    QSlider* volumeSlider = nullptr;
    VideoProgressBar* progressBar = nullptr;
    ClickableLabel* playingIcon = nullptr;
    ClickableLabel* stoppedIcon = nullptr;
    ClickableLabel* forwardIcon = nullptr;
    ClickableLabel* backwardIcon = nullptr;
    QMenuBar* menuBar = nullptr;

    QTimer* menuHideTimer = nullptr;
    QTimer* singleClickTimer = nullptr;
    bool isVideoPlaying = false;
    int mouseY = 0;

    void showMenu();
    void hideMenu();
};
