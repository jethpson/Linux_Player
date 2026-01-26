#include "shortcuts.h"
#include <QMenuBar>
#include <QMouseEvent>
#include <QTimer>
#include <QShortcut>
#include <QScreen>
#include <QGuiApplication>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QUrl>
#include <QDebug>

PlayerWindow::PlayerWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ----- Menu Bar -----
    QMenuBar *menuBar = this->menuBar();

    // File menu
    QMenu *fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("E&xit", this, &QMainWindow::close);

    // Options menu
    QMenu *optionsMenu = menuBar->addMenu("&Options");
    optionsMenu->addAction("&Fullscreen Toggle", this, [this]() {
        if (isFullScreen()) showNormal();
        else showFullScreen();
    });

    // Help menu
    QMenu *helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction("&About", []() {
        qInfo("LinuxPlayer v0.1");
    });

    // ----- Shortcuts -----
    new QShortcut(Qt::Key_F, this, [this]() {
        if (isFullScreen()) showNormal();
        else showFullScreen();
    });

    new QShortcut(Qt::Key_Escape, this, [this]() {
        if (isFullScreen()) showNormal();
    });

    // ----- Video Widget -----
    videoWidget = new QVideoWidget(this);
    videoWidget->setAspectRatioMode(Qt::IgnoreAspectRatio);
    setCentralWidget(videoWidget);

    player = new QMediaPlayer(this);
    player->setVideoOutput(videoWidget);
    player->setMedia(QUrl::fromLocalFile("/home/jakei/Projects/Linux_Player/test.mp4"));
    player->play();

    // Enable mouse tracking for menu auto-hide
    setMouseTracking(true);
    videoWidget->setMouseTracking(true);

    // ----- Window scaling -----
    QScreen* screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    resize(screenGeometry.width() * 0.8, screenGeometry.height() * 0.8);
    move((screenGeometry.width() - width()) / 2,
         (screenGeometry.height() - height()) / 2);

    // ----- Auto-hide menu bar -----
    menuHideTimer = new QTimer(this);
    menuHideTimer->setInterval(1500); // 1.5 seconds
    connect(menuHideTimer, &QTimer::timeout, [menuBar, this]() {
        if (isFullScreen() && !menuBar->activeAction()) {
            menuBar->hide();
        }
    });

    // Initial menu bar state
    if (isFullScreen())
        menuBar->hide();
    else
        menuBar->show();
}

// ----- Mouse move shows menu bar -----
void PlayerWindow::mouseMoveEvent(QMouseEvent* event)
{
    QMenuBar* menuBar = this->menuBar();

    if (isFullScreen()) {
        if (event->y() <= 30) {  // near top
            menuBar->show();
            menuHideTimer->start();
        }
    } else {
        menuBar->show();
        menuHideTimer->stop();
    }

    QMainWindow::mouseMoveEvent(event);
}

// ----- Detect fullscreen/windowed changes dynamically -----
void PlayerWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::WindowStateChange) {
        QMenuBar* menuBar = this->menuBar();
        if (isFullScreen())
            menuBar->hide();
        else {
            menuBar->show();
            menuHideTimer->stop();
        }
    }

    QMainWindow::changeEvent(event);
}
