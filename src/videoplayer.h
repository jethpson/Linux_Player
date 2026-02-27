#pragma once

#include <QWidget>
#include <QString>
#include <QThread>
#include <vlc/vlc.h>

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit VideoPlayer(QWidget* parent = nullptr);
    VideoPlayer(const QString& filePath, QWidget* parent = nullptr);
    ~VideoPlayer();

    void loadFile(const QString& filePath);
    void play();
    void pause();
    void stop();
    bool isPlaying() const;

    void seekForward(int ms);
    void seekBackward(int ms);
    void setVolume(int value);
    void setLoop(bool enabled);
    void setPositionMs(int timeMs);

    int getCurrentTime() const;
    int getDuration() const;
    libvlc_media_player_t* getMediaPlayer() const;
    QString getCurrentFile() const { return m_currentFile; }

signals:
    void playing();
    void stopped();
    void mediaReady();
    void clicked();
    void doubleClicked();
    void videoLoaded(const QString& filePath);
    void videoPositionChanged(int timeMs);
    

protected:
    void resizeEvent(QResizeEvent*) override;
    void showEvent(QShowEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseDoubleClickEvent(QMouseEvent*) override;

private:
    void setupEndReachedHandler();
    void updateVideoSize();
    bool isPhoneResolution(int videoWidth, int videoHeight);
    void attachVLC();

    libvlc_instance_t* vlcInstance = nullptr;
    libvlc_media_player_t* mediaPlayer = nullptr;

    QString m_currentFile;

    QThread* loaderThread = nullptr;

    QString currentPath;
    QString pendingFile;

    bool vlcSet = false;
    bool loopEnabled = false;
    bool isLoading = false;
};