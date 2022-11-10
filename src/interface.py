import sys
import math
from PyQt5.QtWidgets import *
from PyQt5.QtGui import QIcon, QPixmap
import json
from graph import draw_all


class App(QWidget):
	def __init__(self, folder_name):
		super().__init__()
		self.ctg_names = ["Count", "Total Time", "Average Time", "Total TLB DM", "Average TLB DM", "Total TLB IM", "Average TLB IM"] 
		self.ctgs = ["cnt", "total_time", "avg_time", "total_tlb_dm", "avg_tlb_dm", "total_tlb_im", "avg_tlb_im"]
		self.folder_name = folder_name
		self.rn = 1
		self.ratio = False
		self.per_rec = False
		self.ctg = "cnt"
# init
		self.init_ui()
		self.append_img()
	def init_ui(self):
# display
		self.setWindowTitle("Display of Profiler")
		self.display_center()
		self.resize(1700, 1200)
# ratio radio button
		self.ratio_btn = QCheckBox("Show Ratio Instead", self)
		self.ratio_btn.clicked.connect(self.ratio_clk)
		self.ratio_btn.resize(210, 40)
		self.ratio_btn.move(1390, 20)
		self.ratio_btn.setStyleSheet("QCheckBox{font-size: 17px;} QCheckBox::indicator{ width: 30px; height: 30px; };")
# perRecord radio button
		self.per_rec_btn = QCheckBox("Not Cumulative per Record", self)
		self.per_rec_btn.clicked.connect(self.per_rec_clk)
		self.per_rec_btn.resize(280, 40)
		self.per_rec_btn.move(1105, 20)
		self.per_rec_btn.setStyleSheet("QCheckBox{font-size: 17px;} QCheckBox::indicator{ width: 30px; height: 30px; };")
# labels
		ctg_label = QLabel(self)
		ctg_label.setText("Category")
		ctg_label.resize(100, 40)
		ctg_label.move(100, 70)
		ctg_label.setStyleSheet("font-size: 17px;")
		rn_label = QLabel(self)
		rn_label.setText("Record No.")
		rn_label.resize(100, 40)
		rn_label.move(100, 125)
		rn_label.setStyleSheet("font-size: 17px;")
# get info
		info_f = open("../record/" + self.folder_name + "/info.json", "r");
		info_str = info_f.read()
		info_obj = json.loads(info_str)
		self.record_num = info_obj["record_num"]
		self.call_cnt = info_obj["call_cnt"]
		self.unique_cnt = info_obj["unique_cnt"]
# category buttons
		for i in range(len(self.ctgs)):
			self.create_category_btn(i)
# record buttons
		for i in range(self.record_num):
			self.create_record_btn(i+1)
	def create_category_btn(self, i):
		new_btn = QPushButton(self.ctg_names[i], self)
		new_btn.clicked.connect(lambda: self.category_btn_clk(self.ctgs[i]))
		new_btn.resize(150, 40)
		new_btn.move(155*i + 200, 70)
		new_btn.setStyleSheet("background-color: white;" "border-color: black;" "font-size: 17px;")
	def create_record_btn(self, rn):
		new_btn = QPushButton(str(rn), self)
		new_btn.clicked.connect(lambda: self.record_btn_clk(rn))
		new_btn.resize(40, 40)
		new_btn.move(45*rn + 160, 125)
		new_btn.setStyleSheet("background-color: white;" "border-color: black;" "font-size: 17px;")
	def ratio_clk(self):
		self.ratio = self.ratio_btn.isChecked()
		self.append_img()
	def per_rec_clk(self):
		self.per_rec = self.per_rec_btn.isChecked()
		self.append_img()
	def category_btn_clk(self, ctg):
		self.ctg = ctg
		self.append_img()
	def record_btn_clk(self, rn):
		self.rn = rn
		self.append_img()
	def append_img(self):
		pic = QLabel(self)
		png_name = "../record/" + self.folder_name + "/" + str(self.rn) + "_" + self.ctg
		if self.ratio:
			png_name = png_name + "_ratio"
		if self.per_rec:
			png_name = png_name + "_perRecord"
		png_name = png_name + ".png"
		pic.setPixmap(QPixmap(png_name))
		pic.show()
		pic.resize(1500, 1000)
		pic.move(100, 210)
	def display_center(self):
		qr = self.frameGeometry()
		cp = QDesktopWidget().availableGeometry().center()
		qr.moveCenter(cp)
		self.move(qr.center())

if __name__ == '__main__':
	if len(sys.argv) == 2:
		print("Displaying...\n");
		app = QApplication(sys.argv)
		ex = App(sys.argv[1])
		ex.show()
		app.exec_()
	else:
		print("You need to enter a folder name for the only parmeter\n")
