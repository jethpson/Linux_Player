Minimal Plugin Interface

All plugins must implement the shared interface.

plugininterface.h
#pragma once

#include <QObject>
#include <QString>

class PluginInterface
{
public:
    virtual ~PluginInterface() = default;

    // Human-readable plugin name
    virtual QString name() const = 0;

    // Called after the plugin is loaded
    virtual void initialize() = 0;
};

#define PluginInterface_iid "com.example.PluginInterface"

Q_DECLARE_INTERFACE(PluginInterface, PluginInterface_iid)
