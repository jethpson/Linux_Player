#pragma once
#include <QObject>

class QMainWindow;
class VideoPlayer;

class MenuBarController : public QObject
{
    Q_OBJECT
public:
    explicit MenuBarController(QMainWindow* window, VideoPlayer* player, QObject* parent = nullptr);

private:
    QMainWindow* window;
    VideoPlayer* videoPlayer;

    void setupMenuBar();
    void setupShortcuts();
};