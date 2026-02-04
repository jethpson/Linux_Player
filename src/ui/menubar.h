#pragma once
#include <QObject>

class QMainWindow;
class VideoPlayer;
class VolumeSliderController;

class MenuBarController : public QObject
{
    Q_OBJECT
public:
    explicit MenuBarController(QMainWindow* w, VideoPlayer* player, VolumeSliderController* vc, QObject* parent = nullptr);

private:
    QMainWindow* window;
    VideoPlayer* videoPlayer;

    void setupMenuBar();
    void setupShortcuts();

    VolumeSliderController* volumeController;
};