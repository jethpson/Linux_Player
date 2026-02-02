#pragma once

#include <QWidget>
#include <QSlider>
#include <QTimer>
#include <vlc/vlc.h>

class VideoProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit VideoProgressBar(libvlc_media_player_t* player, QWidget* parent = nullptr);
    void setPlayer(libvlc_media_player_t* newPlayer);

signals:
    void seekRequested(qint64 newTime);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    QSlider* slider;
    QTimer* updateTimer;
    libvlc_media_player_t* mediaPlayer;
    bool isDragging = false;
    bool isSeeking = false;

private slots:
    void updateSlider();
    void sliderReleased();
    void sliderPressed();

};