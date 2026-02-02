#pragma once

#include <QMainWindow>
#include <QTimer>
#include <QSlider>
#include "videoplayer.h"
#include "videoprogressbar.h"
#include "clickablelabel.h"
#include "menubar.h"
#include "volumeslider.h"
#include "iconcontroller.h"
#include "layoutcontroller.h"

class PlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlayerWindow(QWidget* parent = nullptr);
    ~PlayerWindow() = default;

signals:
    void mediaReady();

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
    VideoProgressBar* progressBar = nullptr;
    QSlider* volumeSlider = nullptr;
    ClickableLabel* playingIcon = nullptr;
    ClickableLabel* stoppedIcon = nullptr;
    ClickableLabel* forwardIcon = nullptr;
    ClickableLabel* backwardIcon = nullptr;

    MenuBarController* menuController = nullptr;
    VolumeSliderController* volumeController = nullptr;
    IconController* iconController = nullptr;
    LayoutController* layoutController = nullptr;

    QTimer* menuHideTimer = nullptr;
    QTimer* singleClickTimer = nullptr;
    bool isVideoPlaying = false;
    int mouseY = 0;
};