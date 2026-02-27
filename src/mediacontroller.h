#pragma once

#include <QMainWindow>
#include <QString>
#include <QEvent>
#include <QResizeEvent>
#include <QMouseEvent>

class VideoPlayer;
class VideoProgressBar;
class ClickableLabel;
class MenuBarController;
class VolumeSliderController;
class IconController;
class LayoutController;
class QLabel;
class QTimer;
class QSlider;

class PlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlayerWindow(QWidget* parent = nullptr);
    ~PlayerWindow() override = default;

    void updateVolumeSliderState(const QString& filePath);

signals:
    void videoLoaded(const QString& path);
    void playbackStarted();
    void playbackStopped();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    void showPlayingIcon();
    void showStoppedIcon();

private:
    QString msToTimeString(int ms);

    VideoPlayer* videoPlayer = nullptr;
    VideoProgressBar* progressBar = nullptr;
    QSlider* volumeSlider = nullptr;

    ClickableLabel* playingIcon = nullptr;
    ClickableLabel* stoppedIcon = nullptr;
    ClickableLabel* forwardIcon = nullptr;
    ClickableLabel* backwardIcon = nullptr;
    ClickableLabel* volumeIcon = nullptr;
    ClickableLabel* volumeMuteIcon = nullptr;
    ClickableLabel* loopSolidIcon = nullptr;
    ClickableLabel* loopHollowIcon = nullptr;
    ClickableLabel* spacerLIcon = nullptr;
    ClickableLabel* spacerRIcon = nullptr;

    MenuBarController* menuController = nullptr;
    VolumeSliderController* volumeController = nullptr;
    IconController* iconController = nullptr;
    LayoutController* layoutController = nullptr;

    QLabel* timeLabel = nullptr;
    QTimer* singleClickTimer = nullptr;

    bool isVideoPlaying = false;
    int mouseY = 0;
};