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

protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;   // <-- add this line

private:
    libvlc_instance_t* vlcInstance = nullptr;
    libvlc_media_t* media = nullptr;
    libvlc_media_player_t* mediaPlayer = nullptr;
    bool vlcSet = false;
};