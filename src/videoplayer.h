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

    void play();
    void pause();
    void stop();
    void loadFile(const QString& filePath);

    void setLoop(bool loop) { loopEnabled = loop; }

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;

signals:
    void playing();   // emitted when playback starts/resumes
    void stopped();   // emitted when playback pauses/stops

private:
    libvlc_instance_t* vlcInstance = nullptr;
    libvlc_media_t* media = nullptr;
    libvlc_media_player_t* mediaPlayer = nullptr;
    bool vlcSet = false;
    bool loopEnabled = false;

    void setupEndReachedHandler();
};