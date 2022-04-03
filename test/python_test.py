from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5 import uic
from pathlib import Path
#PyQt5.QtCore.pyqtWrapperType is not a registered type

from PyQtFluentWindow import FluentWindow

QtWidgets.QApplication.setAttribute(QtCore.Qt.AA_EnableHighDpiScaling)
QtWidgets.QApplication.setAttribute(QtCore.Qt.AA_UseHighDpiPixmaps)

qt_app = QtWidgets.QApplication([])
current_dir = Path(__file__).parent
# ui = uic.loadUi(current_dir / "fluent_window_ui.ui")
pac = FluentWindow()
#ui.show()  # show first, then load appsgrid with progress bar
pac.show()
qt_app.exec_()
