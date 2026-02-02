#include "mediacontroller.h"
#include "videoplayer.h"

#include "videoprogressbar.h"
#include "clickablelabel.h"
#include "menubar.h"
#include "volumeslider.h"
#include "iconcontroller.h"

#include <QApplication>
#include <QDir>
#include <QMenuBar>
#include <QMenu>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QScreen>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QShortcut>
#include <QTimer>
#include <QDebug>
#include <QCoreApplication>

PlayerWindow::PlayerWindow(QWidget* parent)
    : QMainWindow(parent)
{

    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    QString videoPath = dir.filePath("Unselected.mp4");

    // ----- Video Player -----
    videoPlayer = new VideoPlayer(videoPath, this);
    videoPlayer->setAttribute(Qt::WA_OpaquePaintEvent, false);
    videoPlayer->setAttribute(Qt::WA_TranslucentBackground, true);
    videoPlayer->setAutoFillBackground(false);
    videoPlayer->setLoop(true);

    // ----- Menu & Shortcuts -----
    menuController = new MenuBarController(this, videoPlayer, this);

    // ----- Volume Slider -----
    volumeController = new VolumeSliderController(videoPlayer, this);
    volumeSlider = volumeController->getSlider();

    // ----- Progress Bar -----
    progressBar = new VideoProgressBar(nullptr, this);

    // Connect after mediaPlayer is ready
    connect(videoPlayer, &VideoPlayer::mediaReady, this, [this]() {
        libvlc_media_player_t* mp = videoPlayer->getMediaPlayer();
        if (mp)
        {
            qDebug() << "[PlayerWindow] Assigning mediaPlayer to progress bar";
            progressBar->setPlayer(mp);
        }
    });

    // ----- Icons -----
    iconController = new IconController(videoPlayer, this);
    playingIcon = iconController->getPlayingIcon();
    stoppedIcon = iconController->getStoppedIcon();
    forwardIcon = iconController->getForwardIcon();
    backwardIcon = iconController->getBackwardIcon();

    // ----- Container Layout -----
    QWidget* container = new QWidget(this);
    container->setContentsMargins(0,0,0,0);
    container->setAttribute(Qt::WA_StyledBackground, true);
    setCentralWidget(container);

    // Set parents
    videoPlayer->setParent(container);
    progressBar->setParent(container);
    volumeSlider->setParent(container);
    playingIcon->setParent(container);
    stoppedIcon->setParent(container);
    forwardIcon->setParent(container);
    backwardIcon->setParent(container);

    // Let LayoutController handle all positioning
    layoutController = new LayoutController(
        videoPlayer, container,
        volumeSlider, progressBar,
        playingIcon, stoppedIcon,
        forwardIcon, backwardIcon,
        this->menuBar()
    );

    // Trigger initial positioning
    layoutController->updatePositions();

    // Video fills entire container
    videoPlayer->setParent(container);
    videoPlayer->setGeometry(container->rect());
    videoPlayer->show();

    // Progress bar overlay
    progressBar->setParent(container);
    progressBar->setFixedHeight(20);
    progressBar->installEventFilter(this);
    progressBar->move(0, container->height() - progressBar->height());
    progressBar->show();

    // Volume slider overlay
    volumeSlider->setParent(container);
    volumeSlider->setFixedHeight(60);
    volumeSlider->setFocusPolicy(Qt::NoFocus);
    volumeSlider->move((container->width() - volumeSlider->width()) / 2,
                       container->height() - volumeSlider->height());
    volumeSlider->show();

    // Icons overlay
    playingIcon->setParent(container);
    stoppedIcon->setParent(container);
    forwardIcon->setParent(container);
    backwardIcon->setParent(container);

    playingIcon->raise();
    stoppedIcon->raise();
    forwardIcon->raise();
    backwardIcon->raise();

    // ----- Layout Controller -----
    layoutController = new LayoutController(
        videoPlayer, this,
        volumeSlider, progressBar,
        playingIcon, stoppedIcon,
        forwardIcon, backwardIcon,
        this->menuBar()
    );

    // ----- Mouse Tracking -----
    setMouseTracking(true);
    videoPlayer->setMouseTracking(true);
    videoPlayer->installEventFilter(this);

    // ----- Window Scaling -----
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    resize(screenGeometry.width() * 0.8, screenGeometry.height() * 0.8);
    move((screenGeometry.width() - width()) / 2, (screenGeometry.height() - height()) / 2);

    // ----- Connect video signals -----
    connect(videoPlayer, &VideoPlayer::playing, this, &PlayerWindow::showPlayingIcon);
    connect(videoPlayer, &VideoPlayer::stopped, this, &PlayerWindow::showStoppedIcon);

    // ----- Single vs Double Click -----
    singleClickTimer = new QTimer(this);
    singleClickTimer->setSingleShot(true);
    singleClickTimer->setInterval(QApplication::doubleClickInterval());

    connect(singleClickTimer, &QTimer::timeout, this, [this]() {
        if (isVideoPlaying) {
            videoPlayer->pause();
            showStoppedIcon();
        } else {
            videoPlayer->play();
            showPlayingIcon();
        }
    });

    connect(videoPlayer, &VideoPlayer::clicked, this, [this]() {
        if (layoutController)
            layoutController->handleClick();
    });

    qDebug() << "Successfully opened Player";
}

// ----- Resize Event -----
void PlayerWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    if (layoutController) layoutController->handleResize();
}

// ----- Mouse Move Event -----
void PlayerWindow::mouseMoveEvent(QMouseEvent* event)
{
    mouseY = event->pos().y();
    if (layoutController) layoutController->handleMouseMove(mouseY);
    QMainWindow::mouseMoveEvent(event);
}

// ----- Event Filter -----
bool PlayerWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == videoPlayer)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            auto* e = static_cast<QMouseEvent*>(event);
            if (e->button() == Qt::LeftButton)
            {
                singleClickTimer->start();
                return false; // let Qt see it
            }
        }

        if (event->type() == QEvent::MouseButtonDblClick)
        {
            singleClickTimer->stop();

            if (layoutController)
                layoutController->handleClick();

            return false;
        }

        if (event->type() == QEvent::MouseMove)
        {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            QPointF mapped = videoPlayer->mapTo(this, mouseEvent->position().toPoint());

            QMouseEvent translated(
                mouseEvent->type(),
                mapped,
                mouseEvent->globalPosition(),
                mouseEvent->button(),
                mouseEvent->buttons(),
                mouseEvent->modifiers(),
                mouseEvent->pointingDevice()
            );

            mouseMoveEvent(&translated);
            return true;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

// ----- Fullscreen Change -----
void PlayerWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::WindowStateChange) {
        if (layoutController)
            layoutController->handleFullScreenChange(isFullScreen());
    }
    QMainWindow::changeEvent(event);
}

// ----- Play/Pause Icons -----
void PlayerWindow::showPlayingIcon()
{
    if (playingIcon && stoppedIcon) {
        playingIcon->setVisible(true);
        stoppedIcon->setVisible(false);
        playingIcon->raise();
        stoppedIcon->raise();
        isVideoPlaying = true;
    }
}

void PlayerWindow::showStoppedIcon()
{
    if (playingIcon && stoppedIcon) {
        playingIcon->setVisible(false);
        stoppedIcon->setVisible(true);
        playingIcon->raise();
        stoppedIcon->raise();
        isVideoPlaying = false;
    }
}