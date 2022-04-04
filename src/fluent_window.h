#ifndef FLUENT_WINDOW_H
#define FLUENT_WINDOW_H

#include <QWidget>
#include "ui_fluent_window.h"

#if defined(FLW_LIBRARY)
#define FLW_SHARED_EXPORT Q_DECL_EXPORT
#else
#define FLW_SHARED_EXPORT Q_DECL_IMPORT
#endif

#if defined(WIN32)
    #include <windows.h>
#endif

// LEFT_MENU_MIN_WIDTH = 70
// LEFT_MENU_MAX_WIDTH = 250
// RIGHT_MENU_MIN_WIDTH = 0
// RIGHT_MENU_MAX_WIDTH = 270

enum resizeDirection{
    default, 
    top, left, right, bottom, 
    top_left, top_right, bottom_left, bottom_right
};


class FLW_SHARED_EXPORT FluentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FluentWindow(QMainWindow *parent = nullptr);

public slots:
    void moveWindow(QEvent *event);

protected:
    bool eventFilter( QObject *dest, QEvent *event );

private:
    void setRestoreMaxButtonState();
    void enableNativeAnimations();
    void maximizeRestore();
    void mousePressEvent(QMouseEvent *event);
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    void handleResizeCursor(QHoverEvent *event , uint8_t x_offset=10, uint8_t y_offset=8);
    void resizeWindow(QMouseEvent *event);

    Ui::MainWindow _ui;

    std::string _title_text; // save app title (hide for collapse)

    // window resize
    bool _resize_pressed = false;
    resizeDirection _resize_direction;
    QPoint _resize_point;
    QRect _last_geometry;

    // window move
    QPoint _drag_position;

};


#endif // FLUENT_WINDOW_H
