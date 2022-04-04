#include "fluent_window.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, const_cast<char**>(argv));
    QWidget window;
    //auto main_window = loadUiFile(NULL)
    // Ui::fluent_window_ui ui;
    // ui.setupUi(&widget);
    
    auto main_window = FluentWindow();

    //AnalogClock clock(&window);
    //window.resize(250, 150);
    //window.setWindowTitle("Simple example");
    main_window.show();
    //return 0;
    auto file  = QFile("C:\\Repos\\pyqt5_fluent_ui\\src\\light_style.qss");
    file.open(QIODevice::ReadOnly);

    auto content = file.readAll().toStdString();
    file.close();
    app.setStyleSheet(QString::fromStdString(content));
    return app.exec();
}