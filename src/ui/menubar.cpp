#include "menubar.h"
#include "videoplayer.h"
#include "volumeslider.h"

#include <QMainWindow>
#include <QMenuBar>
#include <QFileDialog>
#include <QShortcut>
#include <QFileInfo>
#include <QDebug>
#include <QPluginLoader>
#include <QDir>
#include <plugininterface.h>

MenuBarController::MenuBarController(
    QMainWindow* w,
    VideoPlayer* player,
    VolumeSliderController* vc,
    QObject* parent)
    : QObject(parent),
      window(w),
      videoPlayer(player),
      volumeController(vc)
{
    setupMenuBar();
    setupShortcuts();
    loadPlugins();
}

MenuBarController::~MenuBarController()
{
    unloadPlugins();
}

// -------------------------------------------------
// Populate top bar
// -------------------------------------------------
void MenuBarController::setupMenuBar()
{
    QMenuBar* menuBar = window->menuBar();

    QMenu* fileMenu = menuBar->addMenu("&File");
    fileMenu->addAction("&Open...", window, [this]() {
        QFileDialog dialog(window, "Open Video");
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setNameFilter(
            "Video Files (*.mp4 *.mkv *.avi *.mov *.webm);;All Files (*)");
        if (dialog.exec() != QDialog::Accepted)
            return;
        const QString videoPath = dialog.selectedFiles().first();
        if (videoPlayer)
            videoPlayer->loadFile(videoPath);
    });
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", window, &QMainWindow::close);

    // Options
    QMenu* optionsMenu = menuBar->addMenu("&Options");
    optionsMenu->addAction("&Fullscreen Toggle", window, [this]() {
        window->isFullScreen()
            ? window->showNormal()
            : window->showFullScreen();
    });

    // Plugins
    QMenu* pluginsMenu = optionsMenu->addMenu("&Plugins");
    pluginsMenu->setObjectName("PluginsMenu"); // <-- Important!

    // Help
    QMenu* helpMenu = menuBar->addMenu("&Help");
    helpMenu->addAction("&About", []() { qInfo() << "LinuxPlayer v0.1"; });
}

// -------------------------------------------------
// Keyboard inputs
// -------------------------------------------------
void MenuBarController::setupShortcuts()
{
    new QShortcut(Qt::Key_F, window, [this]() {
        window->isFullScreen()
            ? window->showNormal()
            : window->showFullScreen();
    });

    new QShortcut(Qt::Key_Escape, window, [this]() {
        if (window->isFullScreen())
            window->showNormal();
    });

    new QShortcut(Qt::Key_Right, window, [this]() {
        if (videoPlayer)
            videoPlayer->seekForward(5000);
    });

    new QShortcut(Qt::Key_Left, window, [this]() {
        if (videoPlayer)
            videoPlayer->seekBackward(5000);
    });

    new QShortcut(Qt::Key_M, window, [this]() {
        if (volumeController)
            volumeController->toggleMute();
    });

    new QShortcut(Qt::Key_Space, window, [this]() {
        if (!videoPlayer)
            return;

        if (videoPlayer->isPlaying()) {
            videoPlayer->pause();
            QMetaObject::invokeMethod(window, "showStoppedIcon");
        } else {
            videoPlayer->play();
            QMetaObject::invokeMethod(window, "showPlayingIcon");
        }
    });
}

// -------------------------------------------------
// Load Plugins from folder
// -------------------------------------------------
void MenuBarController::loadPlugins()
{
    QDir pluginsDir(QCoreApplication::applicationDirPath());
    pluginsDir.cd("plugins");

    QMenu* pluginsMenu = window->menuBar()->findChild<QMenu*>("PluginsMenu");
    if (!pluginsMenu) {
        qWarning() << "[Plugins] Plugins menu not found!";
        return;
    }

    const QStringList pluginFiles = pluginsDir.entryList({ "*.so", "*.dll" }, QDir::Files);
    qDebug() << "[Plugins] Found plugin files:" << pluginFiles;

    for (const QString& file : pluginFiles) {
        QString fullPath = pluginsDir.absoluteFilePath(file);
        QPluginLoader* loader = new QPluginLoader(fullPath);

        QObject* instance = loader->instance();
        if (!instance) {
            qWarning() << "[Plugins] Failed to load plugin:" << file
                       << "->" << loader->errorString();
            delete loader;
            continue;
        }

        PluginInterface* plugin = qobject_cast<PluginInterface*>(instance);
        if (!plugin) {
            qWarning() << "[Plugins] Invalid plugin:" << file;
            loader->unload();
            delete loader;
            continue;
        }

        plugin->load();
        plugin->setVideoPlayer(videoPlayer);
        plugin->initialize();

        // Create submenu for plugin
        QMenu* pluginMenu = pluginsMenu->addMenu(plugin->name());

        // Populate menu via Q_INVOKABLE slot
        if (QObject* obj = dynamic_cast<QObject*>(plugin)) {
            bool invoked = QMetaObject::invokeMethod(
                obj,
                "populateMenu",
                Qt::DirectConnection,
                Q_ARG(QMenu*, pluginMenu)
            );

            if (!invoked)
                qWarning() << "[Plugins] Failed to invoke populateMenu for" << plugin->name();
        }

        qDebug() << "[Plugins] Loaded plugin:" << plugin->name();

        loadedPlugins.push_back({loader, plugin});
    }
}

// -------------------------------------------------
// Unload plugins before closing app
// -------------------------------------------------
void MenuBarController::unloadPlugins()
{
    for (auto& lp : loadedPlugins) {
        if (!lp.plugin)
            continue;

        lp.plugin->shutdown();

        lp.plugin->unload();
        lp.loader->unload();
        delete lp.loader;
    }
    loadedPlugins.clear();
}