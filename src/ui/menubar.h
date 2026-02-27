#pragma once

#include <QObject>
#include <QAction>
#include <vector>
#include <QPluginLoader>
#include "plugininterface.h"


class QMainWindow;
class VideoPlayer;
class VolumeSliderController;


class MenuBarController : public QObject
{
    Q_OBJECT

public:
    explicit MenuBarController(
        QMainWindow* w,
        VideoPlayer* player,
        VolumeSliderController* vc,
        QObject* parent = nullptr
    );

    ~MenuBarController() override;

    void unloadPlugins();

private:
    struct LoadedPlugin {
        QPluginLoader* loader = nullptr;
        PluginInterface* plugin = nullptr;
    };

    QMainWindow* window = nullptr;
    VideoPlayer* videoPlayer = nullptr;
    VolumeSliderController* volumeController = nullptr;

    QAction* connectAction = nullptr;
    std::vector<LoadedPlugin> loadedPlugins;

    void setupMenuBar();
    void setupShortcuts();
    void loadPlugins();
};