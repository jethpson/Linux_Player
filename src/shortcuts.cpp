#include "shortcuts.h"
#include "videoplayer.h"

#include <QMenuBar>
#include <QMenu>
#include <QMouseEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QShortcut>
#include <QScreen>
#include <QGuiApplication>
#include <QUrl>
#include <QDebug>

#include <QFileDialog>

PlayerWindow::PlayerWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ----- Menu Bar -----
    QMenuBar *menuBar = this->menuBar();

    // File menu
    QMenu *fileMenu = menuBar->addMenu("&File");

    fileMenu->addAction("&Open...", this, [this]() {
        QString filePath = QFileDialog::getOpenFileName(
            this,
            "Open Video",
            QString(),
            "Video Files (*.mp4 *.mkv *.avi *.mov *.webm);;All Files (*)"
        );

        if (!filePath.isEmpty()) {
            videoPlayer->loadFile(filePath);
        }
    });

    fileMenu->addSeparator();

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
    videoPlayer = new VideoPlayer("/home/jakei/Projects/Linux_Player/test4.mp4", this);

    // Create a container widget as central widget
    QWidget* container = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(videoPlayer, 1);  // stretch factor ensures filling
    container->setLayout(layout);        // assign layout to container
    setCentralWidget(container);

    // Enable mouse tracking
    setMouseTracking(true);
    videoPlayer->setMouseTracking(true);
    videoPlayer->installEventFilter(this);   // forward mouse events properly

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

void PlayerWindow::mouseMoveEvent(QMouseEvent* event)
{
    QMenuBar* menuBar = this->menuBar();

    if (isFullScreen()) {
        // Only show menu when near the top
        if (event->y() <= 30) {  // <-- this is the top edge trigger
            if (!menuBar->isVisible())
                menuBar->show();
            menuHideTimer->start();  // restart auto-hide timer
        }
    } else {
        // Always show menu in windowed mode
        if (!menuBar->isVisible())
            menuBar->show();
        menuHideTimer->stop();
    }

    QMainWindow::mouseMoveEvent(event);
}

bool PlayerWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == videoPlayer && event->type() == QEvent::MouseMove) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint globalPos = videoPlayer->mapTo(this, mouseEvent->pos());
        QMouseEvent translatedEvent(mouseEvent->type(), globalPos, mouseEvent->button(),
                                    mouseEvent->buttons(), mouseEvent->modifiers());
        mouseMoveEvent(&translatedEvent);
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
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
