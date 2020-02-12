# File:		main.py
# Author: 	Lei Kuang
# Date:		11th Feb 2020
# @ Imperial College London

from PyQt5.QtWidgets import QApplication
from Lacewing_dialog import Lacewing_dialog

if __name__ == "__main__":
    import sys
    
    app = QApplication(sys.argv)

    ui = Lacewing_dialog()
    ui.show()
    
    sys.exit(app.exec_())
