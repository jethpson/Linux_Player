#pragma once

#include <QWidget>
#include <QResizeEvent>
#include <vlc/vlc.h>

class VideoPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayer(const QString &filePath, QWidget *parent = nullptr);
    ~VideoPlayer();

    libvlc_media_player_t* getLibVLCPlayer() const { return mediaPlayer; }

    void play();
    void pause();
    void stop();
    void loadFile(const QString& filePath);
    void seekForward(int ms);
    void seekBackward(int ms);
    void setVolume(int value);

    void setLoop(bool loop) { loopEnabled = loop; }

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;

signals:
    void playing();   // emitted when playback starts/resumes
    void stopped();   // emitted when playback pauses/stops
    void clicked();
    void doubleClicked();

private:
    libvlc_instance_t* vlcInstance = nullptr;
    libvlc_media_t* media = nullptr;
    libvlc_media_player_t* mediaPlayer = nullptr;
    bool vlcSet = false;
    bool loopEnabled = false;

    void setupEndReachedHandler();
};