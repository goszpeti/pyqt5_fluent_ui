#pragma once
#include <QWidget>
#include <QPainter>
#include <QTime>
#include <QTimer>
#include <QString>
#include <QtCore>
#include <cstdio>
#include <QtPlugin>
#include "fluent_window.h"

// #if defined(WIN32)
//     Q_IMPORT_PLUGIN (QWindowsIntegrationPlugin)
// #else
//     Q_IMPORT_PLUGIN(QXcbIntegrationPlugin) // TODO needed?
// #endif

FluentWindow::FluentWindow(QMainWindow *parent): QMainWindow(parent)
{
    _ui.setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
                            Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
}

// QWidget FluentWindow::get_ui_element(){
//     return this->findChild("centralwidget");
// }

