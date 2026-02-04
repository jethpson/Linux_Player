#include "iconcontroller.h"
#include "videoplayer.h"
#include "clickablelabel.h"
#include <QCoreApplication>
#include "volumeslider.h"

IconController::IconController(VideoPlayer* player, QWidget* parent, VolumeSliderController* vController)
    : QObject(parent), videoPlayer(player), parentWidget(parent), volumeController(vController)
{
    setupIcons();

    if(volumeController) {
        volumeController->volumeIcon = volumeIcon;
        volumeController->volumeMuteIcon = volumeMuteIcon;
        volumeController->updateIcon();
    }

    setupConnections();
}

void IconController::setupIcons()
{
    QString workspacePath = QCoreApplication::applicationDirPath() + "/../resources/";

    QPixmap playPix(workspacePath + "play.png");
    QPixmap stopPix(workspacePath + "pause.png");
    QPixmap forwardPix(workspacePath + "forward.png");
    QPixmap backwardPix(workspacePath + "backward.png");
    QPixmap volumePix(workspacePath + "volume-up.png");
    QPixmap volumeMutePix(workspacePath + "volume.png");
    QPixmap loopSPix(workspacePath + "loopSolid.png");
    QPixmap loopHPix(workspacePath + "loopHollow.png");

    playingIcon = new ClickableLabel(parentWidget);
    stoppedIcon = new ClickableLabel(parentWidget);
    forwardIcon = new ClickableLabel(parentWidget);
    backwardIcon = new ClickableLabel(parentWidget);
    volumeIcon = new ClickableLabel(parentWidget);
    volumeMuteIcon = new ClickableLabel(parentWidget);
    loopSIcon = new ClickableLabel(parentWidget);
    loopHIcon = new ClickableLabel(parentWidget);

    playingIcon->setPixmap(playPix.scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    stoppedIcon->setPixmap(stopPix.scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    forwardIcon->setPixmap(forwardPix.scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    backwardIcon->setPixmap(backwardPix.scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    volumeIcon->setPixmap(volumePix.scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    volumeMuteIcon->setPixmap(volumeMutePix.scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    loopSIcon->setPixmap(loopSPix.scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    loopHIcon->setPixmap(loopHPix.scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    playingIcon->setFixedSize(35, 35);
    stoppedIcon->setFixedSize(35, 35);
    forwardIcon->setFixedSize(35, 35);
    backwardIcon->setFixedSize(35, 35);
    volumeIcon->setFixedSize(35, 35);
    volumeMuteIcon->setFixedSize(35, 35);
    loopSIcon->setFixedSize(35, 35);
    loopHIcon->setFixedSize(35, 35);

    playingIcon->setStyleSheet("background: transparent; border: none;");
    stoppedIcon->setStyleSheet("background: transparent; border: none;");
    forwardIcon->setStyleSheet("background: transparent; border: none;");
    backwardIcon->setStyleSheet("background: transparent; border: none;");
    volumeIcon->setStyleSheet("background: transparent; border: none;");
    volumeMuteIcon->setStyleSheet("background: transparent; border: none;");
    loopSIcon->setStyleSheet("background: transparent; border: none;");
    loopHIcon->setStyleSheet("background: transparent; border: none;");

    playingIcon->show();
    stoppedIcon->hide();
    forwardIcon->show();
    backwardIcon->show();
    volumeIcon->show();
    volumeMuteIcon->hide();
    loopSIcon->show();
    loopHIcon->hide();

    playingIcon->raise();
    stoppedIcon->raise();
    forwardIcon->raise();
    backwardIcon->raise();
    volumeIcon->raise();
    volumeMuteIcon->raise();
    loopSIcon->raise();
    loopHIcon->raise();
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

    connect(volumeIcon, &ClickableLabel::clicked, this, [this]() {
        if(volumeController && !videoPlayer->getCurrentFile().endsWith("Unselected.mp4")) volumeController->toggleMute();
    });

    connect(volumeMuteIcon, &ClickableLabel::clicked, this, [this]() {
        if(volumeController && !videoPlayer->getCurrentFile().endsWith("Unselected.mp4")) volumeController->toggleMute();
    });

    connect(loopSIcon, &ClickableLabel::clicked, this, [this]() {
        videoPlayer->setLoop(false);
        QMetaObject::invokeMethod(parentWidget, "showloopHollowIcon");
    });

    connect(loopHIcon, &ClickableLabel::clicked, this, [this]() {
        videoPlayer->setLoop(true);
        QMetaObject::invokeMethod(parentWidget, "showloopSolidIcon");
    });
}
