#pragma once

#include <QWidget>
#include <QtCore/qglobal.h>
#include "ui_fluent_window.h"

#if defined(FLW_LIBRARY)
#define FLW_SHARED_EXPORT Q_DECL_EXPORT
#else
#define FLW_SHARED_EXPORT Q_DECL_IMPORT
#endif

class FLW_SHARED_EXPORT FluentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FluentWindow(QMainWindow *parent = nullptr);

    // QWidget get_ui_element();

private:
    Ui::MainWindow _ui;
};
