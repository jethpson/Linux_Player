#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include "clickablelabel.h"
#include "videoprogressbar.h"

class VideoPlayer;

class PlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlayerWindow(QWidget* parent = nullptr);
    ~PlayerWindow() = default;

protected:
    void resizeEvent(QResizeEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void changeEvent(QEvent* event) override;


private slots:
    void showPlayingIcon();
    void showStoppedIcon();

private:
    VideoPlayer* videoPlayer = nullptr;
    QTimer* menuHideTimer = nullptr;
    ClickableLabel* playingIcon;
    ClickableLabel* stoppedIcon;
    ClickableLabel* forwardIcon;
    ClickableLabel* backwardIcon;
    bool isVideoPlaying = false;
    int mouseY = 0;
    VideoProgressBar* progressBar = nullptr;


};