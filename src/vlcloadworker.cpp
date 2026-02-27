#include "vlcloadworker.h"
#include <QDebug>

VLCLoadWorker::VLCLoadWorker(const QString& file,
                             libvlc_instance_t* instance,
                             QObject* parent)
    : QObject(parent),
      filePath(file),
      vlcInstance(instance)
{
}

void VLCLoadWorker::process()
{
    qDebug() << "[Worker] Loading media:" << filePath;

    libvlc_media_t* media =
        libvlc_media_new_path(vlcInstance, filePath.toUtf8().constData());

    if (!media) {
        qWarning() << "[Worker] Failed to load media:" << filePath;
        emit finished(nullptr);
        return;
    }

    libvlc_media_player_t* player =
        libvlc_media_player_new_from_media(media);

    libvlc_media_release(media);

    if (!player) {
        qWarning() << "[Worker] Failed to create media player";
        emit finished(nullptr);
        return;
    }

    libvlc_video_set_scale(player, 0);
    libvlc_video_set_aspect_ratio(player, nullptr);

    qDebug() << "[Worker] Media loaded successfully";

    emit finished(player);
    emit videoFileChanged(filePath);
}