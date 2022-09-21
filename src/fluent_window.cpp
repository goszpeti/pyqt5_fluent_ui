#include <cstdio>

#include <QString>
#include <QtCore>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>

#include "fluent_window.h"
// using namespace std;

// #if defined(WIN32)
//     Q_IMPORT_PLUGIN (QWindowsIntegrationPlugin)
// #else
//     Q_IMPORT_PLUGIN(QXcbIntegrationPlugin) // TODO needed?
// #endif
 
constexpr int LEFT_MENU_MIN_WIDTH = 80;
constexpr int LEFT_MENU_MAX_WIDTH = 350; // int(310 + 20*(2/get_display_scaling()))
constexpr int RIGHT_MENU_MIN_WIDTH = 0;
constexpr int RIGHT_MENU_MAX_WIDTH = 400; //int(200 + 200*(2/get_display_scaling()))

FluentWindow::FluentWindow(QMainWindow *parent):
    // _resize_direction(resizeDirection::default), _drag_position(0,0), _title_text(""),
     _ui(new Ui::MainWindow)
{
    _ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
                         Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

    // Dropshadow
    // auto effect = QGraphicsDropShadowEffect();
    // effect.setBlurRadius(10);
    // this->setGraphicsEffect(&effect);

    _ui->left_menu_frame->setMinimumWidth(LEFT_MENU_MIN_WIDTH);

    // window buttons
    connect(_ui->restore_max_button,  &QPushButton::clicked, this, &FluentWindow::maximizeRestore);
    connect(_ui->minimize_button,  &QPushButton::clicked, this, &FluentWindow::showMinimized);
    connect(_ui->close_button,  &QPushButton::clicked, this, &FluentWindow::close);

    connect(_ui->toggle_left_menu_button, &QPushButton::clicked, this, &FluentWindow::toggle_left_menu);
    // this->findChild(QObject, name="toggle_left_menu_button").clicked.connect(this->toggle_left_menu)
    // this->findChild(QObject, name="settings_button").clicked.connect(this->toggle_right_menu)
    // this->findChild(QObject, name="page_info_label").setText("")

    // initial maximize state
    this->setRestoreMaxButtonState();
    this->enableNativeAnimations();

    QCoreApplication::instance()->installEventFilter(this);

}


FluentWindow::~FluentWindow()
{
    delete _ui;
    if (_left_anim==NULL){
        delete _left_anim;
    }
}


void FluentWindow::toggle_left_menu(){
        int width = _ui->left_menu_frame->width();
        int width_to_set = 0;
        bool maximize = false;
        // switch extended and minimized state
        if (width == LEFT_MENU_MIN_WIDTH){
            width_to_set = LEFT_MENU_MAX_WIDTH;
            maximize = true;
        }
        else{
            width_to_set = LEFT_MENU_MIN_WIDTH;
            maximize = false;
        }

        _left_anim = new QPropertyAnimation(_ui->left_menu_frame, "minimumWidth");
        _left_anim->setDuration(200);
        _left_anim->setStartValue(width);
        _left_anim->setEndValue(width_to_set);
        _left_anim->setEasingCurve(QEasingCurve::InOutQuart);
        _left_anim->start();

        // hide title
        if (maximize){
            _ui->title_label->setText(_title_text);
        }
        else{
            _ui->title_label->setText("");
        }
        // hide menu button texts
        // name, (button, _) in self.page_entries.items():
        // for button in self.ui.left_menu_middle_subframe.findChildren(QPushButton):
        //     name = self.page_widgets.get_display_name_by_name(button.objectName())
        //     if maximize:
        //         button.setText(name)
        //         button.setStyleSheet("text-align:left;")
        //     else:
        //         button.setText("")
        //         button.setStyleSheet("text-align:middle;")

        // if (_ui->settings_button.isChecked()){
        //     self.toggle_right_menu();
        // }
}

bool FluentWindow::eventFilter( QObject *object, QEvent *event ) {
    // Window resizing
    if(object == (QObject *)this && !this->isMaximized()) { // no resize when maximized
        switch (event->type()) {
            case QEvent::HoverMove: {
                if (!_resize_pressed) {
                    // cursor position control for cursor shape setup
                    this->handleResizeCursor((QHoverEvent *)event);
                    return true;
                }
            }
            case  QEvent::MouseButtonPress: {
                _resize_pressed = true;
                // save the starting point of resize
                _resize_point = this->mapToGlobal(((QMouseEvent *)event)->pos());
                _last_geometry = this->geometry();
                return true;
            }
            case  QEvent::MouseButtonRelease:{
                _resize_pressed = false; // restore cursor
                // this->handleResizeCursor((QHoverEvent *)event); // TODO needed?
                return true;
            }
            case  QEvent::MouseMove: {
                if (this->cursor().shape() != Qt::ArrowCursor) {
                    this->resizeWindow((QMouseEvent *)event);
                }
                break;
            }
            default:;
        }
        return QMainWindow::eventFilter(object, event);
    }
    // Maximize on doubleclick and move witth the top frame
    if(object == (QObject *)_ui->left_menu_top_subframe || object == (QObject *)_ui->top_frame) { // 
        switch (event->type()) {
            case QEvent::MouseButtonDblClick:{
                if (((QMouseEvent *)event)->button() == Qt::LeftButton) {
                    maximizeRestore();
                }
                break;
            }
            case  QEvent::MouseMove:
                moveWindow(event);
                break;
            default:;
        }
    }
    return QMainWindow::eventFilter(object, event);
}


void FluentWindow::maximizeRestore() {
    if (this->isMaximized()) {
        this->showNormal();
    }
    else {
        this->showMaximized();
    }
    this->setRestoreMaxButtonState();
}

void FluentWindow::setRestoreMaxButtonState() {
    if (this->isMaximized()) {
        _ui->restore_max_button->setToolTip("Restore");
        _ui->restore_max_button->setIcon(QIcon(":/fluent/icons/restore.png"));
    }
    else{
        _ui->restore_max_button->setToolTip("Maximize");
        _ui->restore_max_button->setIcon(QIcon(":/fluent/icons/maximize.png"));
    }
}

void FluentWindow::enableNativeAnimations() {
    #if defined(WIN32)
        // Sets up thickframe and other needed flag for WIN-Arrow functions an animations to work
        // Needs custom resizing and border suppresion functions to work correctly
        auto style = GetWindowLongA(HWND(this->winId()), GWL_STYLE);
        SetWindowLongA(HWND(this->winId()), GWL_STYLE,
             style| WS_BORDER | WS_MAXIMIZEBOX | WS_CAPTION | CS_DBLCLKS | WS_THICKFRAME);
    #endif
}

void FluentWindow::moveWindow(QEvent *event) {
    if (this->cursor().shape() != Qt::ArrowCursor) { // TODO needed?
        return;
    }
    // native windows move - enables snap functions
    #ifdef WIN32
        ReleaseCapture();
        //emit native move signal
        SendMessageA(HWND(this->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
        this->setRestoreMaxButtonState();
        return;
    #endif
    // if maximized, return to normal be able to move
    if (this->isMaximized()) {
        this->maximizeRestore();
    }
    // qt move
    auto mouseEvent = (QMouseEvent *)event;
    if (mouseEvent->buttons() == Qt::LeftButton) {
        this->move(this->pos() + mouseEvent->globalPos() - _drag_position);
        _drag_position = mouseEvent->globalPos();
        event->accept();
    }
}


void FluentWindow::mousePressEvent(QMouseEvent *event)
{
    // Helper for moving window to know mouse position
    _drag_position = event->globalPos();
}

bool FluentWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    // Platform native events
    #if defined(WIN32)
        auto msg = (MSG*) message;
        // ignore WM_NCCALCSIZE event. Suppresses native Window drawing of title-bar.
        if (msg->message == WM_NCCALCSIZE) {
            this->setRestoreMaxButtonState();
            *result = 0;
            const auto clientRect = ((static_cast<BOOL>(msg->wParam) == FALSE)
                                    ? reinterpret_cast<LPRECT>(msg->lParam)
                                    : &(reinterpret_cast<LPNCCALCSIZE_PARAMS>(msg->lParam))->rgrc[0]);
            clientRect->bottom += 1;
            return true;
        }
        else if (msg->message == WM_WINDOWPOSCHANGING) {
            // Tell Windows to discard the entire contents of the client area, as re-using
            // parts of the client area would lead to jitter during resize.
            const auto windowPos = reinterpret_cast<LPWINDOWPOS>(msg->lParam);
            windowPos->flags |= SWP_NOCOPYBITS;
            return false;
    } 
    #endif

    return QMainWindow::nativeEvent(eventType, message, result);
}
void FluentWindow::handleResizeCursor(QHoverEvent *event , uint8_t x_offset, uint8_t y_offset) {

    // using relative position, since the event can only be fired inside of the Window
    auto rect = this->rect();
    auto position = event->pos(); // relative pos to window

    if (QRect(rect.topLeft().x() + x_offset, rect.topLeft().y(), this->width() - 2*x_offset, y_offset).contains(position)) {
        _resize_direction = resizeDirection::top;
        this->setCursor(Qt::SizeVerCursor);
    }
    else if(QRect(rect.bottomLeft().x() + x_offset, rect.bottomLeft().y(), this->width() - 2*x_offset, -y_offset).contains(position)) {
        _resize_direction = resizeDirection::bottom;
        this->setCursor(Qt::SizeVerCursor);
    }
    else if(QRect(rect.topRight().x() - x_offset, rect.topRight().y() + y_offset, x_offset, this->height() - 2*y_offset).contains(position)) {
        _resize_direction = resizeDirection::right;
        this->setCursor(Qt::SizeHorCursor);
    }
    else if(QRect(rect.topLeft().x() + x_offset, rect.topLeft().y() + y_offset, -x_offset, this->height() - 2*y_offset).contains(position)) {
        _resize_direction = resizeDirection::left;
        this->setCursor(Qt::SizeHorCursor);
    }
    else if(QRect(rect.topRight().x(), rect.topRight().y(), -x_offset, y_offset).contains(position)) {
        _resize_direction = resizeDirection::top_right;
        this->setCursor(Qt::SizeBDiagCursor);
    }
    else if(QRect(rect.bottomLeft().x(), rect.bottomLeft().y(), x_offset, -y_offset).contains(position)) {
        _resize_direction = resizeDirection::bottom_left;
        this->setCursor(Qt::SizeBDiagCursor);
    }
    else if(QRect(rect.topLeft().x(), rect.topLeft().y(), x_offset, y_offset).contains(position)) {
        _resize_direction = resizeDirection::top_left;
        this->setCursor(Qt::SizeFDiagCursor);
    }
    else if(QRect(rect.bottomRight().x(), rect.bottomRight().y(), -x_offset, -y_offset).contains(position)) {
        _resize_direction = resizeDirection::bottom_right;
        this->setCursor(Qt::SizeFDiagCursor);
    }
    else {// no resize
        this->setCursor(Qt::ArrowCursor);
    }
}

void FluentWindow::resizeWindow(QMouseEvent *event) {
    const auto current_point = this->mapToGlobal(event->pos()) - _resize_point;

    // Sadly, this still flickers on Windows. This will possibly not be able to fix, unless
    // the native resize function would be called
    switch (_resize_direction) {
        case resizeDirection::top: {
            const auto new_height = _last_geometry.height() - current_point.y();
            if (new_height > this->minimumHeight()) {
                this->setGeometry(_last_geometry.x(), _last_geometry.y() + current_point.y(), 
                _last_geometry.width(), new_height
                );
            }
            return;
        }
        case resizeDirection::bottom: {
            const auto new_height = _last_geometry.height() + current_point.y();
            this->resize(_last_geometry.width(), new_height);
            return;
        }
        case resizeDirection::left: {
            const auto new_width = _last_geometry.width() - current_point.x();
            if (new_width > this->minimumWidth()){
                this->setGeometry(_last_geometry.x() + current_point.x(), _last_geometry.y(), 
                                 new_width, _last_geometry.height());
            }
            return;
        }
        case resizeDirection::right: {
            this->resize(_last_geometry.width() + current_point.x(), _last_geometry.height());
            return;
        }
        case resizeDirection::top_right: {
            const auto new_width = _last_geometry.width() + current_point.x();
            const auto new_height = _last_geometry.height() - current_point.y();
            if (new_width > this->minimumWidth() && new_height > this->minimumHeight()){
             this->setGeometry(_last_geometry.x(), _last_geometry.y() +
                                  current_point.y(), new_width, new_height);
            }
            return;
        }
        case resizeDirection::bottom_right: {
            const auto new_width = _last_geometry.width() + current_point.x();
            const auto new_height = _last_geometry.height() + current_point.y();
            this->setGeometry(_last_geometry.x(), _last_geometry.y(), new_width, new_height);
            return;
        }
        case resizeDirection::bottom_left: {
            const auto new_width = _last_geometry.width() - current_point.x();
            const auto new_height = _last_geometry.height() + current_point.y();
            if (new_width > this->minimumWidth() && new_height > this->minimumHeight()){
                this->setGeometry(_last_geometry.x() + current_point.x(),
                                 _last_geometry.y(), new_width, new_height);
            }
            return;
        }
        case resizeDirection::top_left: {
            const auto new_width = _last_geometry.width() - current_point.x();
            const auto new_height = _last_geometry.height() - current_point.y();
            if (new_width > this->minimumWidth() && new_height > this->minimumHeight()){
                this->setGeometry(_last_geometry.x() + current_point.x(),
                                 _last_geometry.y() + current_point.y(), new_width, new_height);
            }
            return;
        }
    }
}