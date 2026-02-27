#include "mediacontroller.h"
#include "videoplayer.h"

#include "videoprogressbar.h"
#include "clickablelabel.h"
#include "menubar.h"
#include "volumeslider.h"
#include "iconcontroller.h"
#include "layoutcontroller.h"

#include <QLabel>
#include <QApplication>
#include <QDir>
#include <QMenuBar>
#include <QScreen>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QShortcut>
#include <QTimer>
#include <QDebug>
#include <QCoreApplication>
#include <QGraphicsDropShadowEffect>

// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------
PlayerWindow::PlayerWindow(QWidget* parent)
    : QMainWindow(parent)
{
    
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    QString defaultVideo = dir.filePath("Unselected.mp4");

    // Video Player
    videoPlayer = new VideoPlayer(this);
    videoPlayer->setAttribute(Qt::WA_OpaquePaintEvent, false);
    videoPlayer->setAttribute(Qt::WA_TranslucentBackground, true);
    videoPlayer->setAutoFillBackground(false);
    videoPlayer->setLoop(true);

    // Volume Slider
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setEnabled(false);
    volumeSlider->setValue(50);
    volumeSlider->setFixedSize(200, 40);
    volumeSlider->setFocusPolicy(Qt::NoFocus);

    // Progress Bar
    progressBar = new VideoProgressBar(nullptr, this);

    // Volume Controller
    volumeController = new VolumeSliderController(videoPlayer, this);
    volumeController->setSlider(volumeSlider);

    // Menu Controller
    menuController = new MenuBarController(this, videoPlayer, volumeController);
    menuBar()->setNativeMenuBar(false);

    // Icons
    iconController = new IconController(videoPlayer, this, volumeController);

    playingIcon     = iconController->getPlayingIcon();
    stoppedIcon     = iconController->getStoppedIcon();
    forwardIcon     = iconController->getForwardIcon();
    backwardIcon    = iconController->getBackwardIcon();
    volumeIcon      = iconController->getVolumeIcon();
    volumeMuteIcon  = iconController->getVolumeMuteIcon();
    loopSolidIcon   = iconController->getLoopSIcon();
    loopHollowIcon  = iconController->getLoopHIcon();
    spacerLIcon     = iconController->getSpacerLIcon();
    spacerRIcon     = iconController->getSpacerRIcon();

    // Central Container
    QWidget* container = new QWidget(this);
    container->setContentsMargins(0, 0, 0, 0);
    container->setAttribute(Qt::WA_StyledBackground, true);
    setCentralWidget(container);

    // Time Label
    timeLabel = new QLabel("0:00 – 0:00", this);
    timeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    timeLabel->setContentsMargins(0, 0, 10, 0);
    timeLabel->setFont(QFont("Arial", 14, QFont::Bold));
    timeLabel->setFixedSize(170, 35);
    timeLabel->setStyleSheet("color: black;");

    auto* shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(2);
    shadow->setColor(Qt::gray);
    shadow->setOffset(0, 0);
    timeLabel->setGraphicsEffect(shadow);

    // setup progress bar & volume
    connect(videoPlayer, &VideoPlayer::mediaReady, this, [this, defaultVideo]() {
        auto* mp = videoPlayer->getMediaPlayer();
        if (!mp) return;

        progressBar->setPlayer(mp);

        if (videoPlayer->getCurrentFile() == defaultVideo)
            volumeSlider->setEnabled(false);
        else
            volumeSlider->setEnabled(!volumeController->getIsMuted());
    });

    // Forward core events to plugins
    connect(videoPlayer, &VideoPlayer::videoLoaded,
            this, &PlayerWindow::videoLoaded);
    connect(progressBar, &VideoProgressBar::seekRequested,
        videoPlayer, &VideoPlayer::setPositionMs);

    // Timer: update time label
    auto* timeTimer = new QTimer(this);
    connect(timeTimer, &QTimer::timeout, this, [this]() {
        timeLabel->setText(
            QString("%1 – %2")
                .arg(msToTimeString(videoPlayer->getCurrentTime()))
                .arg(msToTimeString(videoPlayer->getDuration()))
        );
    });
    timeTimer->start(200);

    // Parent overlays
    QList<QWidget*> overlays = {
        videoPlayer, progressBar, volumeSlider,
        playingIcon, stoppedIcon,
        forwardIcon, backwardIcon,
        volumeIcon, volumeMuteIcon,
        loopSolidIcon, loopHollowIcon,
        spacerLIcon, spacerRIcon,
        timeLabel
    };

    for (auto* w : overlays)
        w->setParent(container);

    videoPlayer->setGeometry(container->rect());
    videoPlayer->show();

    progressBar->setFixedHeight(20);
    progressBar->move(0, container->height() - progressBar->height());
    progressBar->show();

    // Layout Controller
    layoutController = new LayoutController(
        videoPlayer, container,
        volumeSlider, progressBar,
        playingIcon, stoppedIcon,
        forwardIcon, backwardIcon,
        volumeIcon, volumeMuteIcon,
        loopSolidIcon, loopHollowIcon,
        spacerLIcon, spacerRIcon,
        timeLabel,
        menuBar()
    );
    layoutController->updatePositions();

    // Mouse tracking
    setMouseTracking(true);
    videoPlayer->setMouseTracking(true);
    videoPlayer->installEventFilter(this);

    // UI + plugin signals
    connect(videoPlayer, &VideoPlayer::playing,
            this, &PlayerWindow::showPlayingIcon);

    connect(videoPlayer, &VideoPlayer::stopped,
            this, &PlayerWindow::showStoppedIcon);

    // Single / Double click logic
    connect(videoPlayer, &VideoPlayer::clicked, this, [this]() {
        if (layoutController)
            layoutController->handleClick();
    });

    // Window sizing
    QRect screen = QGuiApplication::primaryScreen()->geometry();
    resize(screen.width() * 0.8, screen.height() * 0.8);
    move((screen.width() - width()) / 2,
         (screen.height() - height()) / 2);

    // Auto-load default video
    QTimer::singleShot(0, this, [this, defaultVideo]() {
        if (QFile::exists(defaultVideo)) {
            videoPlayer->loadFile(defaultVideo);
            videoPlayer->play();
        }
    });

    qDebug() << "PlayerWindow initialized";
}

// -------------------------------------------------
// Events
// -------------------------------------------------
void PlayerWindow::resizeEvent(QResizeEvent*)
{
    if (layoutController)
        layoutController->handleResize();
}

void PlayerWindow::mouseMoveEvent(QMouseEvent* e)
{
    mouseY = e->pos().y();
    if (layoutController)
        layoutController->handleMouseMove(mouseY);
}

bool PlayerWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == videoPlayer && event->type() == QEvent::MouseMove) {
        auto* me = static_cast<QMouseEvent*>(event);
        QMouseEvent translated(
            me->type(),
            videoPlayer->mapTo(this, me->position().toPoint()),
            me->globalPosition(),
            me->button(),
            me->buttons(),
            me->modifiers(),
            me->pointingDevice()
        );
        mouseMoveEvent(&translated);
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

void PlayerWindow::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::WindowStateChange && layoutController)
        layoutController->handleFullScreenChange(isFullScreen());
}

// ------------------------------------------------------------
// UI Slots
// ------------------------------------------------------------
void PlayerWindow::showPlayingIcon()
{
    playingIcon->setVisible(true);
    stoppedIcon->setVisible(false);
    isVideoPlaying = true;
    emit playbackStarted();
}

void PlayerWindow::showStoppedIcon()
{
    playingIcon->setVisible(false);
    stoppedIcon->setVisible(true);
    isVideoPlaying = false;
    emit playbackStopped();
}

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------
QString PlayerWindow::msToTimeString(int ms)
{
    int s = ms / 1000;
    return QString("%1:%2")
        .arg(s / 60)
        .arg(s % 60, 2, 10, QChar('0'));
}
