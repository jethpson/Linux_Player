#include "videoplayer.h"
#include <QDebug>
#include <QTimer>
#include <QResizeEvent>
#include <QShowEvent>
#include <QMouseEvent>


// VLCLoadWorker Implementation
void VLCLoadWorker::process()
{
    qDebug() << "[Worker] Loading media:" << filePath;

    libvlc_media_t* media = libvlc_media_new_path(vlcInstance, filePath.toUtf8().constData());
    if (!media) {
        qWarning() << "[Worker] Failed to load media:" << filePath;
        emit finished(nullptr);
        return;
    }

    libvlc_media_player_t* player = libvlc_media_player_new_from_media(media);
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
}


// VideoPlayer Implementation
VideoPlayer::VideoPlayer(const QString& filePath, QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(0,0);

    const char* vlc_args[] = {"--avcodec-hw=none", "--quiet"};
    vlcInstance = libvlc_new(2, vlc_args);
    if (!vlcInstance) {
        qWarning() << "Failed to initialize VLC";
        return;
    }

    loadFile(filePath);
}

VideoPlayer::~VideoPlayer()
{
    qDebug() << "[VideoPlayer] Destructor called";

    if (loaderThread) {
        loaderThread->quit();
        loaderThread->wait();
    }

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

void VideoPlayer::loadFile(const QString& filePath)
{
    qDebug() << "[VideoPlayer] loadFile requested:" << filePath;

    // Check if a loader thread is already active
    if (loaderThread) {
        qDebug() << "[VideoPlayer] Warning: a loader thread is already running!";

    }

    if (isLoading) {
        qDebug() << "[VideoPlayer] Already loading, saving pending file";
        pendingFile = filePath;
        return;
    }

    isLoading = true;

    // Stop old mediaPlayer
    if (mediaPlayer) {
        qDebug() << "[VideoPlayer] Stopping old media player";
        libvlc_media_player_stop(mediaPlayer);
        libvlc_media_player_release(mediaPlayer);
        mediaPlayer = nullptr;
        vlcSet = false;
    }

    // Create worker + thread
    loaderThread = new QThread;
    VLCLoadWorker* worker = new VLCLoadWorker(filePath, vlcInstance);
    worker->moveToThread(loaderThread);

    connect(loaderThread, &QThread::started, worker, &VLCLoadWorker::process);

    connect(worker, &VLCLoadWorker::finished, this, [this, worker](libvlc_media_player_t* newPlayer) {
        qDebug() << "[VideoPlayer] Worker finished";

        if (newPlayer) {
            mediaPlayer = newPlayer;

            // Attach end reached handler to the new mediaPlayer
            setupEndReachedHandler();

            if (isVisible() && !vlcSet) {
                libvlc_media_player_set_xwindow(mediaPlayer, winId());
                libvlc_media_player_play(mediaPlayer);
                vlcSet = true;
                updateVideoSize();
                emit playing();
            }

            qDebug() << "[VideoPlayer] Video loaded & playing";
        }

        isLoading = false;

        loaderThread->quit();
        loaderThread->wait();
        loaderThread->deleteLater();
        worker->deleteLater();
        loaderThread = nullptr;

        // Check for pending file
        if (!pendingFile.isEmpty()) {
            QString next = pendingFile;
            pendingFile.clear();
            qDebug() << "[VideoPlayer] Loading pending file:" << next;
            loadFile(next);
        }

        emit mediaReady();
    });

    loaderThread->start();
}

void VideoPlayer::play()
{
    if (mediaPlayer) {
        libvlc_media_player_play(mediaPlayer);
        playingState = true;
        emit playing();
    }
}

void VideoPlayer::pause()
{
    if (mediaPlayer) {
        libvlc_media_player_set_pause(mediaPlayer, 1);
        playingState = false;
        emit stopped();
    }
}

void VideoPlayer::stop()
{
    if (mediaPlayer) {
        libvlc_media_player_stop(mediaPlayer);
        emit stopped();
    }
}

void VideoPlayer::seekForward(int ms)
{
    if (!mediaPlayer) return;
    libvlc_time_t curr = libvlc_media_player_get_time(mediaPlayer);
    libvlc_media_player_set_time(mediaPlayer, curr + ms);
}

void VideoPlayer::seekBackward(int ms)
{
    if (!mediaPlayer) return;
    libvlc_time_t curr = libvlc_media_player_get_time(mediaPlayer);
    if (curr < ms) curr = 0;
    else curr -= ms;
    libvlc_media_player_set_time(mediaPlayer, curr);
}

void VideoPlayer::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateVideoSize();
}

void VideoPlayer::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (!vlcSet && mediaPlayer) {
        libvlc_media_player_set_xwindow(mediaPlayer, winId());
        libvlc_media_player_play(mediaPlayer);
        vlcSet = true;
        updateVideoSize();
        emit playing();
    }
}

void VideoPlayer::updateVideoSize()
{
    if (!mediaPlayer) return;
    int w = width();
    int h = height();
    QString ar = QString("%1:%2").arg(w).arg(h);
    libvlc_video_set_aspect_ratio(mediaPlayer, ar.toUtf8().constData());
}

void VideoPlayer::setupEndReachedHandler()
{
    if (!mediaPlayer) return;

    libvlc_event_manager_t* em = libvlc_media_player_event_manager(mediaPlayer);
    libvlc_event_attach(em, libvlc_MediaPlayerEndReached,
        [](const libvlc_event_t* /*event*/, void* userData) {
            VideoPlayer* player = static_cast<VideoPlayer*>(userData);
            if (!player || !player->mediaPlayer) return;

            if (player->loopEnabled) {
                // Use Qt event loop to safely restart playback
                QMetaObject::invokeMethod(player, [player]() {
                    qDebug() << "[VideoPlayer] Looping video";

                    // Stop first to reset video properly
                    libvlc_media_player_stop(player->mediaPlayer);

                    // tiny delay to ensure video output is ready
                    QTimer::singleShot(50, [player]() {
                        libvlc_media_player_set_position(player->mediaPlayer, 0.0);
                        libvlc_media_player_play(player->mediaPlayer);
                        emit player->playing();
                    });
                }, Qt::QueuedConnection);
            }
        }, this);
}

void VideoPlayer::setVolume(int value)
{
    if (mediaPlayer)
        libvlc_audio_set_volume(mediaPlayer, value);
}

void VideoPlayer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) emit clicked();
    QWidget::mousePressEvent(event);
}

void VideoPlayer::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) emit doubleClicked();
    QWidget::mouseDoubleClickEvent(event);
}
