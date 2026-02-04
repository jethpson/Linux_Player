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
    void updateVolumeSliderState(const QString& filePath);
    ~PlayerWindow() = default;

signals:
    void mediaReady();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void changeEvent(QEvent* event) override;
    QString msToTimeString(int ms);

private slots:
    void showPlayingIcon();
    void showStoppedIcon();
    void showloopSolidIcon();
    void showloopHollowIcon();

private:
    VideoPlayer* videoPlayer = nullptr;
    VideoProgressBar* progressBar = nullptr;
    QSlider* volumeSlider = nullptr;
    ClickableLabel* playingIcon = nullptr;
    ClickableLabel* stoppedIcon = nullptr;
    ClickableLabel* forwardIcon = nullptr;
    ClickableLabel* backwardIcon = nullptr;
    ClickableLabel* volumeIcon = nullptr;
    ClickableLabel* volumeMuteIcon = nullptr;
    ClickableLabel* loopSIcon = nullptr;
    ClickableLabel* loopHIcon = nullptr;

    MenuBarController* menuController = nullptr;
    VolumeSliderController* volumeController = nullptr;
    IconController* iconController = nullptr;
    LayoutController* layoutController = nullptr;
    

    QTimer* menuHideTimer = nullptr;
    QTimer* singleClickTimer = nullptr;
    QLabel* timeLabel = nullptr;
    bool isVideoPlaying = false;
    int mouseY = 0;
    
};