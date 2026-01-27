#pragma once

#include <QMainWindow>
#include <QTimer>

class VideoPlayer;

class PlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlayerWindow(QWidget* parent = nullptr);
    ~PlayerWindow() = default;
    void loadFile(const QString& filePath);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;  // <-- must have override
    void mouseMoveEvent(QMouseEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    VideoPlayer* videoPlayer;
    QTimer* menuHideTimer;
};