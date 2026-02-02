#include "iconcontroller.h"
#include "videoplayer.h"
#include "clickablelabel.h"
#include <QCoreApplication>

IconController::IconController(VideoPlayer* player, QWidget* parent)
    : QObject(parent), videoPlayer(player), parentWidget(parent)
{
    setupIcons();
    setupConnections();
}

void IconController::setupIcons()
{
    QString workspacePath = QCoreApplication::applicationDirPath() + "/../resources/";

    QPixmap playPix(workspacePath + "play.png");
    QPixmap stopPix(workspacePath + "pause.png");
    QPixmap forwardPix(workspacePath + "forward.png");
    QPixmap backwardPix(workspacePath + "backward.png");

    playingIcon = new ClickableLabel(parentWidget);
    stoppedIcon = new ClickableLabel(parentWidget);
    forwardIcon = new ClickableLabel(parentWidget);
    backwardIcon = new ClickableLabel(parentWidget);

    playingIcon->setPixmap(playPix.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    stoppedIcon->setPixmap(stopPix.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    forwardIcon->setPixmap(forwardPix.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    backwardIcon->setPixmap(backwardPix.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    playingIcon->setFixedSize(60, 60);
    stoppedIcon->setFixedSize(60, 60);
    forwardIcon->setFixedSize(60, 60);
    backwardIcon->setFixedSize(60, 60);

    playingIcon->setStyleSheet("background: transparent; border: none;");
    stoppedIcon->setStyleSheet("background: transparent; border: none;");
    forwardIcon->setStyleSheet("background: transparent; border: none;");
    backwardIcon->setStyleSheet("background: transparent; border: none;");

    playingIcon->show();
    stoppedIcon->hide();
    forwardIcon->show();
    backwardIcon->show();

    playingIcon->raise();
    stoppedIcon->raise();
    forwardIcon->raise();
    backwardIcon->raise();
}

void IconController::setupConnections()
{
    connect(playingIcon, &ClickableLabel::clicked, this, [this]() {
        videoPlayer->pause();
        QMetaObject::invokeMethod(parentWidget, "showStoppedIcon");
    });

    connect(stoppedIcon, &ClickableLabel::clicked, this, [this]() {
        videoPlayer->play();
        QMetaObject::invokeMethod(parentWidget, "showPlayingIcon");
    });

    connect(forwardIcon, &ClickableLabel::clicked, this, [this]() {
        if(videoPlayer) videoPlayer->seekForward(5000);
    });

    connect(backwardIcon, &ClickableLabel::clicked, this, [this]() {
        if(videoPlayer) videoPlayer->seekBackward(5000);
    });
}
