#include "menubar.h"
#include "videoplayer.h"
#include "iconcontroller.h"
#include "volumeslider.h"

#include <QMainWindow>
#include <QMenuBar>
#include <QFileDialog>
#include <QShortcut>
#include <QDebug>

MenuBarController::MenuBarController(QMainWindow* w, VideoPlayer* player, VolumeSliderController* vc, QObject* parent)
    : QObject(parent), window(w), videoPlayer(player), volumeController(vc)
{
    setupMenuBar();
    setupShortcuts();
}

void MenuBarController::setupMenuBar()
{
    QMenuBar* menuBar = window->menuBar();

    // File Menu
    QMenu* fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("&Open...", window, [this]() {
        QFileDialog dialog(window, "Open Video");
        dialog.setOption(QFileDialog::DontUseNativeDialog, false);
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setNameFilter(
            "Video Files (*.mp4 *.mkv *.avi *.mov *.webm);;All Files (*)"
        );
        dialog.setDirectory("/run/media/");

        if (dialog.exec() == QDialog::Accepted) {
            QString filePath = dialog.selectedFiles().first();
            videoPlayer->setLoop(true);
            videoPlayer->loadFile(filePath);
        }
    });

    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", window, &QMainWindow::close);

    // Options Menu
    QMenu* optionsMenu = menuBar->addMenu("&Options");
    optionsMenu->addAction("&Fullscreen Toggle", window, [this]() {
        if (window->isFullScreen()) window->showNormal();
        else window->showFullScreen();
    });

    // Help Menu
    QMenu* helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction("&About", []() { qInfo("LinuxPlayer v0.1"); });
}

void MenuBarController::setupShortcuts()
{
    new QShortcut(Qt::Key_F, window, [this]() {
        if (window->isFullScreen()) window->showNormal();
        else window->showFullScreen();
    });

    new QShortcut(Qt::Key_Escape, window, [this]() {
        if (window->isFullScreen()) window->showNormal();
    });

    new QShortcut(Qt::Key_Right, window, [this]() {
        if(videoPlayer) videoPlayer->seekForward(5000);
    });

    new QShortcut(Qt::Key_Left, window, [this]() {
        if(videoPlayer) videoPlayer->seekBackward(5000);
    });

    new QShortcut(Qt::Key_M, window, [this]() {
        if(volumeController)
        {
            volumeController->toggleMute();
        }
    });

    new QShortcut(Qt::Key_Space, window, [this]() {
        if (videoPlayer->isPlaying()) {
            videoPlayer->pause();
            
            QMetaObject::invokeMethod(window, "showStoppedIcon");
        } else {
            videoPlayer->play();
            QMetaObject::invokeMethod(window, "showPlayingIcon");
        }
    });
}

