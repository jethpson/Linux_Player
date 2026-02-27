#include "videoplayer.h"
#include "vlcloadworker.h"

#include <QDebug>
#include <QTimer>
#include <QResizeEvent>
#include <QShowEvent>
#include <QMouseEvent>

VideoPlayer::VideoPlayer(QWidget* parent)
    : QWidget(parent),
      vlcInstance(nullptr),
      mediaPlayer(nullptr),
      loaderThread(nullptr),
      vlcSet(false),
      isLoading(false),
      loopEnabled(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(0, 0);

    const char* vlc_args[] = { "--avcodec-hw=none", "--quiet" };
    vlcInstance = libvlc_new(2, vlc_args);
    if (!vlcInstance)
        qWarning() << "[VideoPlayer] Failed to initialize VLC";
}

VideoPlayer::VideoPlayer(const QString& filePath, QWidget* parent)
    : QWidget(parent),
      vlcInstance(nullptr),
      mediaPlayer(nullptr),
      loaderThread(nullptr),
      vlcSet(false),
      isLoading(false),
      loopEnabled(false)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(0, 0);

    const char* vlc_args[] = { "--avcodec-hw=none", "--quiet" };
    vlcInstance = libvlc_new(2, vlc_args);
    if (!vlcInstance) {
        qWarning() << "[VideoPlayer] Failed to initialize VLC";
        return;
    }

    loadFile(filePath);
}

VideoPlayer::~VideoPlayer()
{
    qDebug() << "[VideoPlayer] Destructor called";

    if (mediaPlayer) {
        libvlc_media_player_stop(mediaPlayer);
        libvlc_media_player_release(mediaPlayer);
        mediaPlayer = nullptr;
    }

    if (vlcInstance) {
        libvlc_release(vlcInstance);
        vlcInstance = nullptr;
    }

    if (loaderThread) {
        loaderThread->quit();
        loaderThread = nullptr;
    }

    qDebug() << "[VideoPlayer] Resources cleaned up";
}

// -------------------------------------------------
// Media Loading
// -------------------------------------------------
void VideoPlayer::loadFile(const QString& filePath)
{
    qDebug() << "[VideoPlayer] loadFile requested:" << filePath;

    m_currentFile = filePath;

    if (isLoading) {
        qDebug() << "[VideoPlayer] Already loading, queueing pending file";
        pendingFile = filePath;
        return;
    }

    isLoading = true;

    if (mediaPlayer) {
        libvlc_media_player_stop(mediaPlayer);
        libvlc_media_player_release(mediaPlayer);
        mediaPlayer = nullptr;
        vlcSet = false;
    }

    loaderThread = new QThread;
    VLCLoadWorker* worker = new VLCLoadWorker(filePath, vlcInstance);
    worker->moveToThread(loaderThread);

    connect(loaderThread, &QThread::started,
            worker, &VLCLoadWorker::process);

    connect(worker, &VLCLoadWorker::finished,
            this,
            [this, worker](libvlc_media_player_t* newPlayer)
    {
        qDebug() << "[VideoPlayer] Worker finished";

        if (newPlayer) {
            mediaPlayer = newPlayer;
            setupEndReachedHandler();
            attachVLC();
        }

        isLoading = false;

        connect(loaderThread, &QThread::finished,
                worker, &QObject::deleteLater);
        connect(loaderThread, &QThread::finished,
                loaderThread, &QObject::deleteLater);

        loaderThread->quit();
        loaderThread = nullptr;

        if (!pendingFile.isEmpty()) {
            QString next = pendingFile;
            pendingFile.clear();
            loadFile(next);
        }

        emit mediaReady();
        emit videoLoaded(m_currentFile);

        QTimer::singleShot(100, this, &VideoPlayer::updateVideoSize);
    });

    loaderThread->start();
}

// -------------------------------------------------
// Playback
// -------------------------------------------------
void VideoPlayer::play()
{
    if (!mediaPlayer) return;

    libvlc_media_player_play(mediaPlayer);
    emit playing();
}

void VideoPlayer::pause()
{
    if (!mediaPlayer) return;

    libvlc_media_player_set_pause(mediaPlayer, 1);
    emit stopped();
}

void VideoPlayer::stop()
{
    if (!mediaPlayer) return;

    libvlc_media_player_stop(mediaPlayer);
    emit stopped();
}

bool VideoPlayer::isPlaying() const
{
    return mediaPlayer &&
           libvlc_media_player_is_playing(mediaPlayer);
}

// -------------------------------------------------
// Seeking
// -------------------------------------------------
void VideoPlayer::seekForward(int ms)
{
    if (!mediaPlayer) return;

    libvlc_time_t curr = libvlc_media_player_get_time(mediaPlayer);
    libvlc_time_t next = curr + ms;

    libvlc_media_player_set_time(mediaPlayer, next);

    emit videoPositionChanged(static_cast<int>(next));
}

void VideoPlayer::seekBackward(int ms)
{
    if (!mediaPlayer) return;

    libvlc_time_t curr = libvlc_media_player_get_time(mediaPlayer);
    libvlc_time_t next = qMax<libvlc_time_t>(0, curr - ms);

    libvlc_media_player_set_time(mediaPlayer, next);

    emit videoPositionChanged(static_cast<int>(next));
}

// -------------------------------------------------
// QWidget Overrides
// -------------------------------------------------
void VideoPlayer::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    QTimer::singleShot(100, this, &VideoPlayer::updateVideoSize);
}

void VideoPlayer::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    attachVLC();
}

void VideoPlayer::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        emit clicked();

    QWidget::mousePressEvent(event);
}

void VideoPlayer::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit doubleClicked();
        QTimer::singleShot(100, this, &VideoPlayer::updateVideoSize);
    }
    QWidget::mouseDoubleClickEvent(event);
}

// -------------------------------------------------
// Helpers
// -------------------------------------------------
void VideoPlayer::attachVLC()
{
    if (!mediaPlayer || vlcSet || !isVisible()) return;

    libvlc_media_player_set_xwindow(mediaPlayer, winId());
    libvlc_media_player_play(mediaPlayer);
    vlcSet = true;
    QTimer::singleShot(100, this, &VideoPlayer::updateVideoSize);
    emit playing();
}

void VideoPlayer::updateVideoSize() {
    if (!mediaPlayer) return;

    int w = width();
    int h = height();
    if (h == 0) h = 1;

    unsigned int videoWidth, videoHeight;
    libvlc_video_get_size(mediaPlayer, 0, &videoWidth, &videoHeight);

    float aspectRatio = static_cast<float>(videoWidth) / videoHeight;

    int newWidth = static_cast<int>(h * aspectRatio);

    bool isFullscreen = parentWidget()->window()->isFullScreen();

    if (isPhoneResolution(videoWidth, videoHeight))
    {
        QString ar = QString("%1:%2").arg(w).arg(h);
        libvlc_video_set_aspect_ratio(mediaPlayer, ar.toUtf8().constData());
        libvlc_video_set_scale(mediaPlayer, 0);
    } else 
    {
        libvlc_video_set_aspect_ratio(mediaPlayer, QString("%1:%2").arg(newWidth).arg(h).toUtf8().constData());
        libvlc_video_set_scale(mediaPlayer, 0);
    }
    
}

bool VideoPlayer::isPhoneResolution(int videoWidth, int videoHeight) {

    float aspectRatio = static_cast<float>(videoWidth) / videoHeight;

    bool isValidAspectRatio = (aspectRatio >= 1.5 && aspectRatio <= 2.2);

    bool isValidResolution = ((videoWidth >= 720 && videoHeight >= 1280) || 
                              (videoWidth <= 1440 && videoHeight <= 2560));

    return isValidAspectRatio && isValidResolution;
}

void VideoPlayer::setLoop(bool enabled)
{
    loopEnabled = enabled;
}

libvlc_media_player_t* VideoPlayer::getMediaPlayer() const
{
    return mediaPlayer;
}

void VideoPlayer::setupEndReachedHandler()
{
    if (!mediaPlayer) return;

    libvlc_event_manager_t* em =
        libvlc_media_player_event_manager(mediaPlayer);

    libvlc_event_attach(
        em,
        libvlc_MediaPlayerEndReached,
        [](const libvlc_event_t*, void* userData)
        {
            auto* player = static_cast<VideoPlayer*>(userData);
            if (!player || !player->mediaPlayer) return;

            QMetaObject::invokeMethod(player,
                                      [player]()
            {
                if (player->loopEnabled) {
                    libvlc_media_player_stop(player->mediaPlayer);
                    QTimer::singleShot(50, [player]() {
                        libvlc_media_player_set_position(player->mediaPlayer, 0.0f);
                        libvlc_media_player_play(player->mediaPlayer);
                        emit player->playing();
                    });
                } else {
                    emit player->stopped();
                }
            },
            Qt::QueuedConnection);
        },
        this
    );
}

// -------------------------------------------------
// Misc
// -------------------------------------------------
void VideoPlayer::setVolume(int value)
{
    if (mediaPlayer) libvlc_audio_set_volume(mediaPlayer, value);
}

int VideoPlayer::getCurrentTime() const
{
    if (!mediaPlayer) return 0;

    return static_cast<int>(libvlc_media_player_get_time(mediaPlayer));
}

int VideoPlayer::getDuration() const
{
    if (!mediaPlayer) return 0;

    return static_cast<int>(libvlc_media_player_get_length(mediaPlayer));
}

void VideoPlayer::setPositionMs(int timeMs)
{
    if (!mediaPlayer) return;

    timeMs = qMax(0, timeMs);
    libvlc_media_player_set_time(mediaPlayer, timeMs);

    emit videoPositionChanged(timeMs);
}