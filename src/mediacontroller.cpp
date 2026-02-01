#include "mediacontroller.h"
#include "videoprogressbar.h"
#include "videoplayer.h"
#include "clickablelabel.h"

#include <QApplication>
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
    // ----- Menu Bar -----
    QMenuBar* menuBar = this->menuBar();

    QMenu* fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("&Open...", this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(
            this,
            "Open Video",
            QString(),
            "Video Files (*.mp4 *.mkv *.avi *.mov *.webm);;All Files (*)"
        );
        if (!filePath.isEmpty()) {
            videoPlayer->setLoop(true);
            videoPlayer->loadFile(filePath);
        }
    });
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", this, &QMainWindow::close);

    QMenu* optionsMenu = menuBar->addMenu("&Options");
    optionsMenu->addAction("&Fullscreen Toggle", this, [this]() {
        if (isFullScreen()) showNormal();
        else showFullScreen();
    });

    QMenu* helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction("&About", []() { qInfo("LinuxPlayer v0.1"); });

    // ----- Shortcuts -----
    new QShortcut(Qt::Key_F, this, [this]() {
        if (isFullScreen()) showNormal();
        else showFullScreen();
    });
    new QShortcut(Qt::Key_Escape, this, [this]() {
        if (isFullScreen()) showNormal();
    });
    new QShortcut(Qt::Key_Space, this, [this]() {
        if (isVideoPlaying) {
            videoPlayer->pause();
            showStoppedIcon();
        } else {
            videoPlayer->play();
            showPlayingIcon();
        }
    });

    // ----- Video Player -----
    videoPlayer = new VideoPlayer("/home/jakei/Projects/Linux_Player/Unselected.mp4", this);
    videoPlayer->setAttribute(Qt::WA_OpaquePaintEvent, false);
    videoPlayer->setAttribute(Qt::WA_TranslucentBackground, true);
    videoPlayer->setAutoFillBackground(false);

    // ----- Container & Layout -----
    QWidget* container = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(0,0,0,0);

    mainLayout->addWidget(videoPlayer, 1);

    // ----- Progress Bar -----
    progressBar = new VideoProgressBar(videoPlayer->getLibVLCPlayer(), this);
    mainLayout->addWidget(progressBar);

    // ----- Volume Slider -----
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(1000);
    volumeSlider->setFixedWidth(200);
    volumeSlider->setFixedHeight(120);
    volumeSlider->setAttribute(Qt::WA_OpaquePaintEvent, false);
    volumeSlider->setAttribute(Qt::WA_TranslucentBackground, true);

    connect(volumeSlider, &QSlider::valueChanged, this, [this](int value){
        if(videoPlayer) videoPlayer->setVolume(value);
    });

    container->setLayout(mainLayout);
    setCentralWidget(container);

    // ----- Icons -----
    QString workspacePath = QCoreApplication::applicationDirPath() + "/../resources/";

    QPixmap playPix(workspacePath + "play.png");
    QPixmap stopPix(workspacePath + "pause.png");
    QPixmap forwardPix(workspacePath + "forward.png");
    QPixmap backwardPix(workspacePath + "backward.png");

    playingIcon = new ClickableLabel(this);
    stoppedIcon = new ClickableLabel(this);
    forwardIcon = new ClickableLabel(this);
    backwardIcon = new ClickableLabel(this);

    playingIcon->setPixmap(playPix.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    stoppedIcon->setPixmap(stopPix.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    forwardIcon->setPixmap(forwardPix.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    backwardIcon->setPixmap(backwardPix.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    playingIcon->setFixedSize(120, 120);
    stoppedIcon->setFixedSize(120, 120);
    forwardIcon->setFixedSize(120, 120);
    backwardIcon->setFixedSize(120, 120);

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

    connect(playingIcon, &ClickableLabel::clicked, this, [this]() {
        videoPlayer->pause();
        showStoppedIcon();
    });
    connect(stoppedIcon, &ClickableLabel::clicked, this, [this]() {
        videoPlayer->play();
        showPlayingIcon();
    });
    connect(forwardIcon, &ClickableLabel::clicked, this, [this]() {
        if(videoPlayer) videoPlayer->seekForward(5000);
    });
    connect(backwardIcon, &ClickableLabel::clicked, this, [this]() {
        if(videoPlayer) videoPlayer->seekBackward(5000);
    });

    // ----- Mouse tracking -----
    setMouseTracking(true);
    videoPlayer->setMouseTracking(true);
    videoPlayer->installEventFilter(this);

    // ----- Window scaling -----
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    resize(screenGeometry.width() * 0.8, screenGeometry.height() * 0.8);
    move((screenGeometry.width() - width()) / 2, (screenGeometry.height() - height()) / 2);

    // ----- Auto-hide menu -----
    menuHideTimer = new QTimer(this);
    menuHideTimer->setInterval(1500);
    connect(menuHideTimer, &QTimer::timeout, [this]() {
        this->menuBar()->hide();
        QResizeEvent dummy(size(), size());
        this->resizeEvent(&dummy);
    });

    if (isFullScreen()) menuBar->hide();
    else menuBar->show();

    // ----- Connect video signals -----
    connect(videoPlayer, &VideoPlayer::playing, this, &PlayerWindow::showPlayingIcon);
    connect(videoPlayer, &VideoPlayer::stopped, this, &PlayerWindow::showStoppedIcon);

    // ----- Single vs Double Click on Video -----
    singleClickTimer = new QTimer(this);
    singleClickTimer->setSingleShot(true);
    singleClickTimer->setInterval(QApplication::doubleClickInterval());

    // Single click confirmed
    connect(singleClickTimer, &QTimer::timeout, this, [this]() {
        if (isVideoPlaying) {
            videoPlayer->pause();
            showStoppedIcon();
        } else {
            videoPlayer->play();
            showPlayingIcon();
        }
    });

    // Handle clicked signal from videoPlayer
    connect(videoPlayer, &VideoPlayer::clicked, this, [this]() {
        if (singleClickTimer->isActive()) {
            // Double click detected
            singleClickTimer->stop();
            if (isFullScreen()) showNormal();
            else showFullScreen();
        } else {
            // Start timer to wait for possible double click
            singleClickTimer->start();
        }
    });

    qDebug() << "Successfully opened Player";
}


// ----- Resize icons dynamically -----
void PlayerWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    if (volumeSlider)
    {

        int sliderNormalOffsetY = 0;

        if (isFullScreen()){
            sliderNormalOffsetY = 15;
        } else {
            sliderNormalOffsetY = -20;
        }

        int sliderHiddenOffsetY = 500;
        int sliderOffsetY = (mouseY >= videoPlayer->height() - 150) ? sliderNormalOffsetY : -sliderHiddenOffsetY;

        int sliderX = (videoPlayer->width() - volumeSlider->width()) / 2 + 280;
        int sliderY = videoPlayer->height() - volumeSlider->height() - sliderOffsetY;
        volumeSlider->move(sliderX, sliderY);
    }

    if (playingIcon && stoppedIcon && videoPlayer && forwardIcon && backwardIcon) {
        int normalOffsetY = 0;    // default distance from bottom

        if (isFullScreen()){
            normalOffsetY = 15;
        } else {
            normalOffsetY = -20;
        }
        
        int hiddenOffsetY = 500;   // move down when mouse not near bottom
        int offsetY;

        // Check if mouse is near bottom
        if (mouseY >= videoPlayer->height() - 150) {
            offsetY = normalOffsetY;
        } else {
            offsetY = -hiddenOffsetY;
        }

        int x = (videoPlayer->width() - playingIcon->width()) / 2;
        int y = videoPlayer->height() - playingIcon->height() - offsetY;

        playingIcon->move(x, y);
        stoppedIcon->move(x, y);

        int spacing = 0; // distance from play/pause button

        backwardIcon->move(x - backwardIcon->width() - spacing, y + (playingIcon->height() - backwardIcon->height()) / 2);
        forwardIcon->move(x + playingIcon->width() + spacing, y + (playingIcon->height() - forwardIcon->height()) / 2);
    }
}

// ----- Show/hide play/pause icons -----
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

// ----- Mouse move to show menu -----
void PlayerWindow::mouseMoveEvent(QMouseEvent* event)
{
    QMenuBar* menuBar = this->menuBar();

    mouseY = event->pos().y();

    if (isFullScreen()) {
        if (event->y() <= 30) {
            if (!menuBar->isVisible())
                menuBar->show();
            menuHideTimer->start();
        }
    } else {
        if (!menuBar->isVisible())
            menuBar->show();
        menuHideTimer->stop();
    }

    // Recalculate icon positions whenever mouse moves
    QResizeEvent dummy(size(), size());
    resizeEvent(&dummy);

    if (progressBar) {
        progressBar->setVisibleBasedOnMouseY(mouseY, videoPlayer->height());
    }

    QMainWindow::mouseMoveEvent(event);
}

// ----- Forward mouse from videoPlayer -----
bool PlayerWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == videoPlayer && event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint mapped = videoPlayer->mapTo(this, mouseEvent->pos());
        QMouseEvent translated(mouseEvent->type(), mapped, mouseEvent->button(),
                               mouseEvent->buttons(), mouseEvent->modifiers());
        mouseMoveEvent(&translated);
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

// ----- Handle fullscreen changes dynamically -----
void PlayerWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::WindowStateChange) {
        QMenuBar* menuBar = this->menuBar();
        if (isFullScreen()) {
            menuBar->hide();
            menuHideTimer->start();
        } else {
            menuBar->show();
            menuHideTimer->stop();

            QTimer::singleShot(0, this, [this]() {
                QResizeEvent ev(size(), size());
                resizeEvent(&ev);
            });
        }
    }

    QMainWindow::changeEvent(event);
}

void PlayerWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    // Optionally, do nothing here, since we handle double click via the clicked signal
}