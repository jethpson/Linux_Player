#include "layoutcontroller.h"
#include "videoplayer.h"
#include "videoprogressbar.h"
#include "clickablelabel.h"

#include <QLabel>
#include <QApplication>
#include <QTimer>
#include <QResizeEvent>
#include <QDebug>

LayoutController::LayoutController(VideoPlayer* player,
                                   QWidget* parent,
                                   QSlider* volSlider,
                                   VideoProgressBar* pBar,
                                   ClickableLabel* playIcon,
                                   ClickableLabel* stopIcon,
                                   ClickableLabel* fIcon,
                                   ClickableLabel* bIcon,
                                   ClickableLabel* volumeIcon,
                                   ClickableLabel* volumeMuteIcon,
                                   ClickableLabel* loopSIcon,
                                   ClickableLabel* loopHIcon,
                                   ClickableLabel* spacerLIcon,
                                   ClickableLabel* spacerRIcon,
                                   QLabel* timeLabel,
                                   QMenuBar* mb)
    : QObject(parent),
      videoPlayer(player),
      parentWidget(parent),
      volumeSlider(volSlider),
      progressBar(pBar),
      playingIcon(playIcon),
      stoppedIcon(stopIcon),
      forwardIcon(fIcon),
      backwardIcon(bIcon),
      volumeIcon(volumeIcon),
      volumeMuteIcon(volumeMuteIcon),
      loopSIcon(loopSIcon),
      loopHIcon(loopHIcon),
      spacerLIcon(spacerLIcon),
      spacerRIcon(spacerRIcon),
      timeLabel(timeLabel),
      menuBar(mb)
{
    // Timers
    menuHideTimer = new QTimer(this);
    menuHideTimer->setInterval(50);
    connect(menuHideTimer, &QTimer::timeout, this, &LayoutController::hideMenu);

    singleClickTimer = new QTimer(this);
    singleClickTimer->setSingleShot(true);
    singleClickTimer->setInterval(QApplication::doubleClickInterval());

    connect(singleClickTimer, &QTimer::timeout, this, [this]() {
        if (!videoPlayer)
            return;

        videoPlayer->isPlaying()
            ? videoPlayer->pause()
            : videoPlayer->play();
    });

    cursorHideTimer = new QTimer(this);
    cursorHideTimer->setSingleShot(true);
    cursorHideTimer->setInterval(CURSOR_IDLE_MS);

    connect(cursorHideTimer, &QTimer::timeout, this, [this]() {
        if (parentWidget->window()->isFullScreen())
            videoPlayer->setCursor(Qt::BlankCursor);
    });
}

// Called from PlayerWindow::resizeEvent
void LayoutController::handleResize()
{
    updatePositions();
}

// Called from PlayerWindow::mouseMoveEvent
void LayoutController::handleMouseMove(int y)
{
    videoPlayer->unsetCursor();
    cursorHideTimer->start();

    mouseY = y;

    if (!menuBar) return;

    if (parentWidget->window()->isFullScreen()) {
        if (y <= 30) {
            
            showMenu();
            menuHideTimer->stop();
        } else {

            if (!menuHideTimer->isActive())
                menuHideTimer->start();
            else {
                menuHideTimer->stop();
                menuHideTimer->start();
            }
        }
    } else {
        
        showMenu();
        menuHideTimer->stop();
    }

    updatePositions();
}


// Fullscreen change
void LayoutController::handleFullScreenChange(bool fullScreen)
{
    if (fullScreen)
        hideMenu();
    else
        showMenu();

    updatePositions();
}

// Single / Double click
void LayoutController::handleClick()
{
    if (singleClickTimer->isActive()) {
        // Double click
        singleClickTimer->stop();
        if (parentWidget->window()->isFullScreen()) parentWidget->window()->showNormal();
        else parentWidget->window()->showFullScreen();
    } else {
        // Start timer for single click
        singleClickTimer->start();
    }
}

// -------------------------------------------------
// Icon positions
// -------------------------------------------------
void LayoutController::updatePositions()
{
    if (!videoPlayer || !parentWidget) return;

    int vpWidth = parentWidget->width();
    int vpHeight = parentWidget->height();

    // Video stretches to fill container
    videoPlayer->setGeometry(0, 0, vpWidth, vpHeight);

    // Determine if mouse is near bottom
    bool mouseNearBottom = (mouseY >= vpHeight - 150);
    
        int slideUpAmount = 0;
        int hiddenOffsetY = 0;

    if (!parentWidget->window()->isFullScreen()){
        slideUpAmount = 20;
        hiddenOffsetY = 600;
    } else {
        slideUpAmount = 20;
        hiddenOffsetY = 600;
    }

    // Progress Bar
    if (progressBar) {
        int pbHeight = progressBar->height();
        progressBar->setFixedWidth(vpWidth);

        int offsetY = mouseNearBottom ? slideUpAmount + 15 : -hiddenOffsetY; 
        progressBar->move(0, vpHeight - pbHeight - offsetY);

        if (mouseNearBottom) progressBar->show();
        else progressBar->hide();
    }

    // Volume Slider
    if (volumeSlider) {
        int sliderX = (vpWidth - volumeSlider->width()) / 2 + 180;
        int sliderY = vpHeight - volumeSlider->height() - (mouseNearBottom ? slideUpAmount - 20: -hiddenOffsetY);
        volumeSlider->move(sliderX, sliderY);
    }

    // Volume Icons
    if (volumeIcon && volumeMuteIcon && volumeSlider)
    {
        int iconY = volumeSlider->y()
                    + (volumeSlider->height() - volumeIcon->height() + 5) / 2;

        int iconX = volumeSlider->x() - volumeIcon->width();

        volumeIcon->move(iconX, iconY);
        volumeMuteIcon->move(iconX, iconY);

    }

    // Icons    
    if (playingIcon && stoppedIcon && forwardIcon && backwardIcon && loopSIcon && loopHIcon && spacerLIcon && spacerRIcon) {
        int centerX = vpWidth / 2;

        int y       = vpHeight - playingIcon->height() - (mouseNearBottom ? slideUpAmount : -hiddenOffsetY);

        playingIcon->move(centerX - playingIcon->width() / 2, y + 20);
        stoppedIcon->move(centerX - stoppedIcon->width() / 2, y + 20);

        int spacing = 15;
        backwardIcon->move(centerX - backwardIcon->width() - spacing, y + (playingIcon->height() - backwardIcon->height()) / 2 + 20);
        forwardIcon->move(centerX + playingIcon->width() - spacing - 5, y + (playingIcon->height() - forwardIcon->height()) / 2 + 20);

        loopSIcon->move(centerX - loopSIcon->width() / 2 - 65, y + 20);
        loopHIcon->move(centerX - loopHIcon->width() / 2 - 65, y + 20);

        if (spacerLIcon) {
            int timeLabelX = timeLabel->x();
            int spacerWidth = timeLabelX;

            spacerLIcon->setFixedWidth(qMax(spacerWidth, 0));  
            spacerLIcon->move(0, y + 20);
        }

        if (spacerRIcon && volumeSlider) {
            int volumeSliderRightX = volumeSlider->x() + volumeSlider->width();
            int spacerWidth = vpWidth - volumeSliderRightX;

            spacerRIcon->setFixedWidth(qMax(spacerWidth, 0));  
            spacerRIcon->move(volumeSliderRightX, y + 20);
        }

        timeLabel->move(centerX - loopHIcon->width() / 2 - 230, y + 20);
    }
}

// -------------------------------------------------
// Menu visibility
// -------------------------------------------------
void LayoutController::showMenu()
{
    if (menuBar && !menuBar->isVisible())
        menuBar->show();
}

void LayoutController::hideMenu()
{
    if (menuBar && menuBar->isVisible())
        menuBar->hide();
}
