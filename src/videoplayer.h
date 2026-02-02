#pragma once

#include <QWidget>
#include <QResizeEvent>
#include <QShowEvent>
#include <QThread>
#include <vlc/vlc.h>
#include <QUrl>

class VLCLoadWorker : public QObject
{
    Q_OBJECT
public:
    VLCLoadWorker(const QString& file, libvlc_instance_t* instance)
        : filePath(file), vlcInstance(instance) {}



public slots:
    void process();

signals:
    void finished(libvlc_media_player_t* player);

private:
    QString filePath;
    libvlc_instance_t* vlcInstance;
};

class VideoPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayer(const QString &filePath, QWidget *parent = nullptr);
    ~VideoPlayer();

    void play();
    void pause();
    void stop();
    void loadFile(const QString& filePath);
    void seekForward(int ms);
    void seekBackward(int ms);
    void setVolume(int value);
    void setLoop(bool loop) { loopEnabled = loop; }
    bool isPlaying() const { return playingState; }
    QString getCurrentFile() const { return currentFile; }
    libvlc_media_player_t* getMediaPlayer() const { return mediaPlayer; }


signals:
    void playing();
    void stopped();
    void clicked();
    void doubleClicked();
    void mediaReady();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    
private:
    libvlc_instance_t* vlcInstance = nullptr;
    libvlc_media_player_t* mediaPlayer = nullptr;
    bool vlcSet = false;
    bool loopEnabled = false;
    bool playingState = false;
    bool isLoading = false;
    QString currentFile;
    QString pendingFile;

    QThread* loaderThread = nullptr;

    void setupEndReachedHandler();
    void updateVideoSize();
};
