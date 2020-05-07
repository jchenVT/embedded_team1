import sys
from PyQt5.QtWidgets import QApplication, QWidget, QMainWindow, QLabel
from PyQt5.QtGui import QPainter, QPixmap
from PyQt5.QtCore import Qt
from PyQt5.QtCore import QTimer
from pixy import *
import pixy 
from ctypes import *


class Blocks (Structure):
    _fields_ = [ ("m_signature", c_uint),
        ("m_x", c_uint),
        ("m_y", c_uint),
        ("m_width", c_uint),
        ("m_height", c_uint),
        ("m_angle", c_uint),
        ("m_index", c_uint),
        ("m_age", c_uint) ]

blocks = BlockArray(100)
frame = 0


class MainWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)

        self.i = 0
            
        self.setGeometry(765,765,765,765)
        self.setWindowTitle("Pixy")
        
        self.label = QLabel()

        canvas = QPixmap(765,765)
        self.label.setPixmap(canvas)

        self.setCentralWidget(self.label)
        
        self.qTimer = QTimer(self)
        self.qTimer.setInterval(10)
        self.qTimer.timeout.connect(self.getPixyValue)
        
        self.show()


    def getPixyValue(self):
        count = pixy.ccc_get_blocks (10, blocks)
        painter = QPainter(self.label.pixmap())

        painter.fillRect(0,0,765,765, Qt.black)
        if count > 0:
            blocko = blocks[0]
            if blocko.m_signature == 1:
                color = Qt.red
            else:
                color = Qt.green
            painter.fillRect(blocko.m_x * 3, blocko.m_y * 3,
                             blocko.m_width * 3,
                             blocko.m_height * 3, color)

        self.update()
        painter.end
        
         

if __name__ == '__main__':
    pixy.init ()
    pixy.change_prog ("color_connected_components");

    app = QApplication(sys.argv)
    
    qW = MainWindow()
    qW.qTimer.start()

    sys.exit(app.exec_())
    
