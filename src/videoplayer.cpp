#include "videoplayer.h"
#include <QDebug>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>

VideoPlayer::VideoPlayer(const QString& filePath, QWidget* parent)
    : QWidget(parent)
    , vlcInstance(nullptr)
    , mediaPlayer(nullptr)
    , vlcSet(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(0, 0);

    const char* vlc_args[] = {
        "--avcodec-hw=none",
        "--quiet"
    };

    vlcInstance = libvlc_new(2, vlc_args);
    if (!vlcInstance)
    {
        qWarning() << "Failed to initialize libVLC";
        return;
    }

    libvlc_media_t* media = libvlc_media_new_path(vlcInstance, filePath.toUtf8().constData());
    if (!media)
    {
        qWarning() << "Failed to create media";
        libvlc_release(vlcInstance);
        vlcInstance = nullptr;
        return;
    }

    mediaPlayer = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

    if (!mediaPlayer)
    {
        qWarning() << "Failed to create media player";
        libvlc_release(vlcInstance);
        vlcInstance = nullptr;
        return;
    }

    libvlc_video_set_scale(mediaPlayer, 0);               // auto-scale to widget
    libvlc_video_set_aspect_ratio(mediaPlayer, nullptr); // ignore source AR
}

VideoPlayer::~VideoPlayer()
{
    if (mediaPlayer)
    {
        libvlc_media_player_stop(mediaPlayer);
        libvlc_media_player_release(mediaPlayer);
        mediaPlayer = nullptr;
    }

    if (vlcInstance)
    {
        libvlc_release(vlcInstance);
        vlcInstance = nullptr;
    }
}

// Playback controls
void VideoPlayer::play()
{
    if (mediaPlayer)
        libvlc_media_player_play(mediaPlayer);
}

void VideoPlayer::pause()
{
    if (mediaPlayer)
        libvlc_media_player_set_pause(mediaPlayer, 1);
}

void VideoPlayer::stop()
{
    if (mediaPlayer)
        libvlc_media_player_stop(mediaPlayer);
}

// Load a new file dynamically
void VideoPlayer::loadFile(const QString& filePath)
{
    if (!vlcInstance)
        return;

    // Stop & release old media player if it exists
    if (mediaPlayer) {
        libvlc_media_player_stop(mediaPlayer);
        libvlc_media_player_release(mediaPlayer);
        mediaPlayer = nullptr;
        vlcSet = false;
    }

    // Create new media
    libvlc_media_t* media = libvlc_media_new_path(vlcInstance, filePath.toUtf8().constData());
    if (!media) {
        qWarning() << "Failed to load media:" << filePath;
        return;
    }

    mediaPlayer = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

    if (!mediaPlayer) {
        qWarning() << "Failed to create media player";
        return;
    }

    libvlc_video_set_scale(mediaPlayer, 0);
    libvlc_video_set_aspect_ratio(mediaPlayer, nullptr);

    // Bind immediately if widget already shown
    if (isVisible()) {
        libvlc_media_player_set_xwindow(mediaPlayer, winId());
        libvlc_media_player_play(mediaPlayer);
        vlcSet = true;

        // Force scaling to widget size immediately
        resizeEvent(nullptr);
    }
}

// Resize handling
void VideoPlayer::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if (!mediaPlayer)
        return;

    int w = width();
    int h = height();
    QString ar = QString("%1:%2").arg(w).arg(h);
    libvlc_video_set_aspect_ratio(mediaPlayer, ar.toUtf8().constData());
}

void VideoPlayer::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    // Bind VLC only once, after the widget is mapped
    if (!vlcSet && mediaPlayer)
    {
        libvlc_media_player_set_xwindow(mediaPlayer, winId());

        // Trigger resize logic immediately
        resizeEvent(nullptr);

        libvlc_media_player_play(mediaPlayer);
        vlcSet = true;
    }
}