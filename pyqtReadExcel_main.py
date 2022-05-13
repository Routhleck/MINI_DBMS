#pyqt读取xlsx
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtGui import QIcon
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
import pandas as pd
import numpy as np


class Ui_MainWindow(QMainWindow):

    def __init__(self):
        super(QtWidgets.QMainWindow, self).__init__()
        self.setupUi(self)
        self.retranslateUi(self)


    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(666, 488)
        self.centralWidget = QtWidgets.QWidget(MainWindow)
        self.centralWidget.setObjectName("centralWidget")
        self.retranslateUi(MainWindow)

        self.tableWidget = QtWidgets.QTableWidget(self.centralWidget)
        self.tableWidget.setGeometry(QtCore.QRect(0, 60, 813, 371))
        self.tableWidget.setObjectName("tableWidget")
        self.tableWidget.setColumnCount(0)
        self.tableWidget.setRowCount(0)
        self.tableWidget.setStyleSheet("selection-background-color:pink")
        self.tableWidget.setEditTriggers(QAbstractItemView.NoEditTriggers)
        self.tableWidget.setSelectionBehavior(QTableWidget.SelectRows)

        self.tableWidget.raise_()

        # 设置图标


        self.pushButton = QtWidgets.QPushButton(self.centralWidget)
        self.pushButton.setGeometry(QtCore.QRect(90, 20, 75, 23))
        self.pushButton.setObjectName("pushButton")
        self.pushButton.setText("打开")
        MainWindow.setCentralWidget(self.centralWidget)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

        self.pushButton.clicked.connect(self.openfile)
        self.pushButton.clicked.connect(self.creat_table_show)

        # 确定
        self.okButton = QtWidgets.QPushButton(self.centralWidget)
        self.okButton.setGeometry(QtCore.QRect(180, 20, 75, 23))
        self.okButton.setObjectName("okButton")
        self.okButton.setText("确定")
        MainWindow.setCentralWidget(self.centralWidget)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

        self.okButton.clicked.connect(self.get_select)


    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "测试数据"))
        MainWindow.setWindowIcon(QIcon("./head.jpg"))
        # MainWindow.show()


    def get_select(self):
        # print(self.tableWidget.columnCount()) # 返回列数
        # print(self.tableWidget.rowCount())  # 返回行数

        colomn = self.tableWidget.columnCount()
        row_list = set()
        for i in self.tableWidget.selectionModel().selection().indexes():
            row_list.add(i.row())
        # print(row_list)
        select_data = []
        for row in row_list:
            row_data = [self.tableWidget.item(row, p).text() for p in range(colomn)]
            select_data.append(row_data)
        print(select_data)


    def openfile(self):

        # 获取路径
        openfile_name = QFileDialog.getOpenFileName(self, '选择文件', '', 'Excel files(*.xlsx , *.xls)')

        #print(openfile_name)
        global path_openfile_name
        path_openfile_name = openfile_name[0]


    def creat_table_show(self):
        # 读取表格，转换表格
        if len(path_openfile_name) > 0:
            input_table = pd.read_excel(path_openfile_name)
            # print(1,input_table)
            input_table_rows = input_table.shape[0]
            input_table_colunms = input_table.shape[1]
            # print(2,input_table_rows)
            # print(3,input_table_colunms)
            input_table_header = input_table.columns.values.tolist()
            #print(input_table_header)

            #读取表格，转换表格,给tablewidget设置行列表头

            self.tableWidget.setColumnCount(input_table_colunms)
            self.tableWidget.setRowCount(input_table_rows)
            self.tableWidget.setHorizontalHeaderLabels(input_table_header)

            #给tablewidget设置行列表头

            #遍历表格每个元素，同时添加到tablewidget中
            for i in range(input_table_rows):
                input_table_rows_values = input_table.iloc[[i]]
                #print(input_table_rows_values)
                input_table_rows_values_array = np.array(input_table_rows_values)
                input_table_rows_values_list = input_table_rows_values_array.tolist()[0]
                 #print(input_table_rows_values_list)
                for j in range(input_table_colunms):
                    input_table_items_list = input_table_rows_values_list[j]
                    #print(input_table_items_list)
                    # print(type(input_table_items_list))

                    #将遍历的元素添加到tablewidget中并显示

                    input_table_items = str(input_table_items_list)
                    newItem = QTableWidgetItem(input_table_items)
                    newItem.setTextAlignment(Qt.AlignHCenter | Qt.AlignVCenter)
                    self.tableWidget.setItem(i, j, newItem)

        #遍历表格每个元素，同时添加到tablewidget中
        else:
            self.centralWidget.show()

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())