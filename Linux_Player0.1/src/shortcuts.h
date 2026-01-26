#ifndef SHORTCUTS_H
#define SHORTCUTS_H

#include <QMainWindow>
#include <QTimer>
#include <QMediaPlayer>
#include <QVideoWidget>

class PlayerWindow : public QMainWindow
{
    Q_OBJECT

public:
    PlayerWindow(QWidget *parent = nullptr);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    QTimer* menuHideTimer;

    // Add these member variables
    QVideoWidget* videoWidget;
    QMediaPlayer* player;
};

#endif // SHORTCUTS_H