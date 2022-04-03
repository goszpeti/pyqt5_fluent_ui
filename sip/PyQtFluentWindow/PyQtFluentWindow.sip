%Import QtGui/QtGuimod.sip
%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

class FluentWindow : public QMainWindow {

%TypeHeaderCode
#include "fluent_window.h"
%End

public:
    FluentWindow(QMainWindow *parent = nullptr);
};

%End