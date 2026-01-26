#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit VideoPlayer(const QString &filePath, QWidget *parent = nullptr);

private:
    QMediaPlayer* player;
    QVideoWidget* videoWidget;
};

#endif // VIDEOPLAYER_H