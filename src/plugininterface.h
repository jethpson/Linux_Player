#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QString>
#include <QObject>

class VideoPlayer;

class PluginInterface
{

public:
    virtual ~PluginInterface() {}

    virtual void load() = 0;
    virtual void unload() = 0;
    virtual QString name() const = 0;
    virtual void initialize() = 0;
    virtual void activate() = 0;

    virtual void setVideoPlayer(VideoPlayer* player) {}

    virtual void shutdown() {}  
};

#define PluginInterface_iid "com.example.PluginInterface"
Q_DECLARE_INTERFACE(PluginInterface, PluginInterface_iid)

#endif // PLUGININTERFACE_H