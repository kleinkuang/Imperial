# File:		Lacewing_dialog.py
# Author: 	Lei Kuang
# Date:		11th Feb 2020
# @ Imperial College London

# Dialog for Lacewing GUI v0.0
'''
    _t: thread
    _s: signal
'''

# ----------------------------------------------------------------
# UI Import
# ----------------------------------------------------------------
#from PyQt5 import QtCore
from PyQt5.QtGui import QTextCursor
from PyQt5.QtCore import pyqtSignal, pyqtSlot, QThread
from PyQt5.QtWidgets import QMainWindow, QApplication

from Ui_Lacewing import Ui_Lacewing_dialog

# ----------------------------------------------------------------
# System Import
# ----------------------------------------------------------------

# ----------------------------------------------------------------
# User Import
# ----------------------------------------------------------------
from debug_cmd import *

# ----------------------------------------------------------------
# QThread
# ----------------------------------------------------------------
class Update_Serial_Port(QThread):
    
    status_s    = pyqtSignal(bool)
    update_s    = pyqtSignal(list)
    serial_obj  = None

    def __init__(self, serial, parent=None):
        super(Update_Serial_Port, self).__init__(parent)
        self.serial_obj = serial

    def __del__(self):
        pass

    def run(self):
        self.status_s.emit(True)
        serial_port = self.serial_obj.list_serial()
        self.update_s.emit(serial_port)
        self.status_s.emit(False)

# ----------------------------------------------------------------
# UI Dialog
# ----------------------------------------------------------------
class Lacewing_dialog(QMainWindow, Ui_Lacewing_dialog):
    # Variable
    serial_obj = None
    serial_port = ['Refresh']
    
    # Thread
    list_port_t = None
    
    # Signal
    console_t = pyqtSignal(str)
    cmd_t     = pyqtSignal(str)

    def __init__(self, parent=None):
        super(Lacewing_dialog, self).__init__(parent)
        self.setupUi(self)
        
        # UI
        self.ui_status_label.setStyleSheet("background-color: #00FF00")

        # Serial
        self.serial_obj = Debug_Command()
        self.serial_obj.set_console(self.console_t)
        
        # Thread
        # - Update the list of serial port
        self.list_port_t = Update_Serial_Port(self.serial_obj)
        self.list_port_t.status_s.connect(self.ui_status)
        self.list_port_t.update_s.connect(self.update_serial_port)
        self.list_port_t.start()
        
        # Concole
        self.console_t.connect(self.print)
        
    # ----------------------------------------------------------------
    # System Indicator
    # ----------------------------------------------------------------
    # UI Status: RUNNING(red) vs IDLE(green)
    def ui_status(self, status):
        self.ui_status_label.setText('RUNNING' if status else 'DILE')
        self.ui_status_label.setStyleSheet("background-color: %s" %("#FF0000" if status else "#00FF00"))

    # ----------------------------------------------------------------
    # Console
    # ----------------------------------------------------------------
    def print(self, msg):
        self.console_textEdit.moveCursor(QTextCursor.End)
        self.console_textEdit.insertPlainText(msg)
        
    @pyqtSlot()
    def on_console_clear_pushButton_clicked(self):
        self.console_textEdit.clear()
        
    @pyqtSlot()
    def on_user_cmd_pushButton_clicked(self):
        user_cmd = self.user_cmd_lineEdit.text()
        self.user_cmd_lineEdit.clear()
        self.print(">> %s\n" % user_cmd)
        self.serial_obj.execute_cmd(user_cmd)

    # ----------------------------------------------------------------
    # Serial Port
    # ----------------------------------------------------------------
    def update_serial_port(self, serial_port):
        self.serial_port_comboBox.clear()
        self.serial_port = serial_port
        self.serial_port.append('Refresh')
        self.serial_port_comboBox.addItems(self.serial_port)
        self.serial_port_comboBox.setCurrentIndex(len(self.serial_port)-1)
 
    @pyqtSlot(int)
    def on_serial_port_comboBox_activated(self, index):
        if(self.serial_obj.is_connected()):
            self.serial_obj.close_serial()
        if(index==len(self.serial_port)-1):
            self.list_port_t.start()
        else:
            self.serial_obj.open_serial(self.serial_port_comboBox.itemText(index))

# ----------------------------------------------------------------
# UI Entrance, same as main.py
# ----------------------------------------------------------------
if __name__ == "__main__":
    import sys

    app = QApplication(sys.argv)
    ui = Lacewing_dialog()
    ui.show()
    
    sys.exit(app.exec_())
    
