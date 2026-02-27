#pragma once

#include <QObject>
#include <QString>
#include <vlc/vlc.h>

class VLCLoadWorker : public QObject
{
    Q_OBJECT
public:
    explicit VLCLoadWorker(const QString& file,
                           libvlc_instance_t* instance,
                           QObject* parent = nullptr);

public slots:
    void process();

signals:
    void finished(libvlc_media_player_t* player);
    void videoFileChanged(const QString& filePath);

private:
    QString filePath;
    libvlc_instance_t* vlcInstance;
};