#include "videoplayer.h"
#include <QDebug>
#include <QResizeEvent>
#include <QShowEvent>

VideoPlayer::VideoPlayer(const QString& filePath, QWidget* parent)
    : QWidget(parent)
    , vlcInstance(nullptr)
    , mediaPlayer(nullptr)
    , vlcSet(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(0, 0);

    const char* vlc_args[] = {"--avcodec-hw=none", "--quiet"};

    vlcInstance = libvlc_new(2, vlc_args);
    if (!vlcInstance) {
        qWarning() << "Failed to initialize libVLC";
        return;
    }

    libvlc_media_t* media = libvlc_media_new_path(vlcInstance, filePath.toUtf8().constData());
    if (!media) {
        qWarning() << "Failed to create media";
        libvlc_release(vlcInstance);
        vlcInstance = nullptr;
        return;
    }

    mediaPlayer = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

    if (!mediaPlayer) {
        qWarning() << "Failed to create media player";
        libvlc_release(vlcInstance);
        vlcInstance = nullptr;
        return;
    }

    libvlc_video_set_scale(mediaPlayer, 0);
    libvlc_video_set_aspect_ratio(mediaPlayer, nullptr);
}

VideoPlayer::~VideoPlayer()
{
    if (mediaPlayer) {
        libvlc_media_player_stop(mediaPlayer);
        libvlc_media_player_release(mediaPlayer);
        mediaPlayer = nullptr;
    }

    if (vlcInstance) {
        libvlc_release(vlcInstance);
        vlcInstance = nullptr;
    }
}

// Playback controls
void VideoPlayer::play()
{
    if (mediaPlayer) {
        libvlc_media_player_play(mediaPlayer);
        emit playing();  // <-- emit signal
    }
}

void VideoPlayer::pause()
{
    if (mediaPlayer) {
        libvlc_media_player_set_pause(mediaPlayer, 1);
        emit stopped();  // <-- emit signal
    }
}

void VideoPlayer::stop()
{
    if (mediaPlayer) {
        libvlc_media_player_stop(mediaPlayer);
        emit stopped();  // <-- emit signal
    }
}

// Load a new file dynamically
void VideoPlayer::loadFile(const QString& filePath)
{
    if (!vlcInstance) return;

    // Tear down old player
    if (mediaPlayer) {
        libvlc_media_player_stop(mediaPlayer);
        libvlc_media_player_release(mediaPlayer);
        mediaPlayer = nullptr;
        vlcSet = false;
    }

    // Create media
    libvlc_media_t* media =
        libvlc_media_new_path(vlcInstance, filePath.toUtf8().constData());

    if (!media) {
        qWarning() << "Failed to load media:" << filePath;
        return;
    }

    // 🔁 LOOP FOREVER (THIS is the magic)
    if (loopEnabled) {
        libvlc_media_add_option(media, "input-repeat=65535");
        qDebug() << "Loop enabled via input-repeat";
    }

    // Create player from media
    mediaPlayer = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

    if (!mediaPlayer) {
        qWarning() << "Failed to create media player";
        return;
    }

    libvlc_video_set_scale(mediaPlayer, 0);
    libvlc_video_set_aspect_ratio(mediaPlayer, nullptr);

    // Attach to window + play
    if (isVisible()) {
        libvlc_media_player_set_xwindow(mediaPlayer, winId());
        libvlc_media_player_play(mediaPlayer);
        vlcSet = true;
        resizeEvent(nullptr);
    }

    qDebug() << "Video Loaded & Playing";
}


// Resize handling
void VideoPlayer::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if (!mediaPlayer) return;

    int w = width();
    int h = height();
    QString ar = QString("%1:%2").arg(w).arg(h);
    libvlc_video_set_aspect_ratio(mediaPlayer, ar.toUtf8().constData());
}

void VideoPlayer::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (!vlcSet && mediaPlayer) {
        libvlc_media_player_set_xwindow(mediaPlayer, winId());
        resizeEvent(nullptr);
        libvlc_media_player_play(mediaPlayer);
        emit playing();   // <-- emit playing when widget first shows
        vlcSet = true;
    }
}

void VideoPlayer::setupEndReachedHandler()
{
    if (!mediaPlayer) return;

    libvlc_event_manager_t* em = libvlc_media_player_event_manager(mediaPlayer);
    libvlc_event_attach(em, libvlc_MediaPlayerEndReached,
        [](const libvlc_event_t* /*event*/, void* userData) {
            VideoPlayer* player = static_cast<VideoPlayer*>(userData);
            if (!player || !player->mediaPlayer) return;

            qDebug() << "Video ended. Looping? " << player->loopEnabled;

            if (player->loopEnabled) {
                libvlc_media_player_set_position(player->mediaPlayer, 0.0);
                libvlc_media_player_play(player->mediaPlayer);
                emit player->playing();
            }
        },
        this
    );
}
