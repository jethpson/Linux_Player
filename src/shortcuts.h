#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QTimer>

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
    QLabel* playingIcon = nullptr;
    QLabel* stoppedIcon = nullptr;
    QTimer* menuHideTimer = nullptr;

};