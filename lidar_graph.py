import sys
from time import sleep
from math import cos, sin
from PyQt5.QtWidgets import QApplication, QWidget, QMainWindow, QLabel
from PyQt5.QtGui import QPainter, QPixmap
from PyQt5.QtCore import Qt
from PyQt5.QtCore import QTimer
from rplidar import RPLidar

lidar = RPLidar('/dev/ttyUSB0')
lidar.stop()
sleep(1)


class MainWindow(QMainWindow):
    def __init__(self):
        QMainWindow.__init__(self)

        self.i = 0
            
        self.setGeometry(1000,1000,1000,1000)
        self.setWindowTitle("Lidar")
        
        self.label = QLabel()

        canvas = QPixmap(1000,1000)
        self.label.setPixmap(canvas)

        self.setCentralWidget(self.label)
        
        self.qTimer = QTimer(self)
        self.qTimer.setInterval(100)
        self.qTimer.timeout.connect(self.getLidarValue)
        
        self.show()


    def getLidarValue(self):
        painter = QPainter(self.label.pixmap())
        painter.setPen(Qt.red)
        i = 0
        painter.fillRect(0,0,1000,1000, Qt.black)
        lidar.clear_input()
        for scan in lidar.iter_scans():
            for point in scan:
                print(point)
                if point[0] < 15:
                    continue
                else:
                    x = point[2] * cos(point[1]) / 2
                    y = point[2] * sin(point[1]) / 2
                    #print((x,y))
                    painter.drawPoint(500 + int(x), 500 + int(y))
            i += 1
            if i == 5:
                break
        lidar.stop()
        self.update()
        painter.end
        
         

if __name__ == '__main__':

    app = QApplication(sys.argv)
    
    qW = MainWindow()
    qW.qTimer.start()

    sys.exit(app.exec_())
    
