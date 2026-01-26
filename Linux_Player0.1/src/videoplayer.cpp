#include "videoplayer.h"
#include <QVBoxLayout>
#include <QUrl>
#include <QMediaContent>

VideoPlayer::VideoPlayer(const QString &filePath, QWidget *parent)
    : QWidget(parent)
{
    // Video widget
    videoWidget = new QVideoWidget(this);

    // Fill the entire parent space
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0); // no empty space
    layout->addWidget(videoWidget);
    setLayout(layout);

    // Media player
    player = new QMediaPlayer(this);
    player->setVideoOutput(videoWidget);
    player->setMedia(QMediaContent(QUrl::fromLocalFile(filePath)));
    player->play();
}