import sys
import matplotlib.pyplot as plt
import numpy as np
import os.path as os

elemNum = 30

plt.figure(figsize=(15,10), tight_layout=True)
plt.rcParams.update({
		"axes.labelsize": 13,
		"axes.titlesize": 18,
		"xtick.labelsize": 15,
		"ytick.labelsize": 12,
		"lines.linewidth": 4
})

def draw(foldername, filename, yLabel):
	xList = [] # x value (function name) list of graph
	yList = [] # y value list of graph
	yRatioList = [] # y ratio value list of graph
	recordNum = 0
	# read file 
	fx = open("../record/" + foldername + "/" + filename, "r")
	while True:
		line = fx.readline()
		t = line.split(" ")
		if t[0] == "#" or not line:
			if recordNum > 0:
				# sort
				oy = yList
				ox = xList
				yList.sort(reverse=True)
				yList = yList[:elemNum]
				xList = elemNum*[0]
				for yi, y in enumerate(yList):
					oi = oy.index(y)
					xList[yi] = ox[oi]
				# get ratio of y
				sum = np.sum(yList)
				yRatioList = [i/sum for i in yList]
				# graph
				plt.bar([i for i in range(len(xList))], yList)
				plt.xticks([i for i in range(len(xList))], xList, rotation=90)
				plt.xlabel("function")
				plt.ylabel(yLabel)
				plt.title(yLabel + " of Functions")
				plt.savefig(foldername + "/" + str(recordNum)+"_"+filename + ".png")
				plt.clf()
				plt.bar([i for i in range(len(xList))], yRatioList)
				plt.xticks([i for i in range(len(xList))], xList, rotation=90)
				plt.xlabel("function")
				plt.ylabel(yLabel)
				plt.title(yLabel + " Ratio of Functions")
				plt.savefig(foldername + "/" + str(recordNum)+"_"+filename + "_ratio.png")
				plt.clf()
				xList = []
				yList = []
		if not line:
			break
		if t[0] == '#':
			recordNum+=1
			xList = []
			yList = []
		else:
			xList.append(t[0])
			yList.append(int(t[1]))
	fx.close()

def draw_all(folder_name):
	draw(folder_name, "total_time", "Total Time")
	draw(folder_name, "avg_time", "Average Time")
	draw(folder_name, "cnt", "Execution Count")
	draw(folder_name, "total_tlb_dm", "Total Data TLB")
	draw(folder_name, "total_tlb_im", "Total Instruction TLB")
	draw(folder_name, "avg_tlb_dm", "Average Data TLB")
	draw(folder_name, "avg_tlb_im", "Average Instruction TLB")

if __name__ == "__main__":
	if len(sys.argv) == 2:
		print("Working...\n");
		folder_name = sys.argv[1]
		draw_all(folder_name)
	else:
		print("You need to enter a folder name for the only parmeter\n")
