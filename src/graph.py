import sys
import matplotlib.pyplot as plt
import numpy as np
import math
import os.path as os

ELEMNUM = 30
progress = 0

plt.figure(figsize=(15,10), tight_layout=True)
plt.rcParams.update({
		"axes.labelsize": 13,
		"axes.titlesize": 18,
		"xtick.labelsize": 15,
		"ytick.labelsize": 12,
		"lines.linewidth": 4
})

def bar(xList, yList, yLabel, title, saveName):
	plt.bar([i for i in range(len(xList))], yList)
	plt.xticks([i for i in range(len(xList))], xList, rotation=90)
	plt.xlabel("function")
	plt.ylabel(yLabel)
	plt.title(title)
	plt.savefig(saveName)
	plt.clf()

def sort(xList, yList):
	oy = yList
	ox = xList
	yList.sort(reverse=True)
# slice
	if ELEMNUM < len(xList):
		yList = yList[:ELEMNUM]
		xList = ELEMNUM*[0]
# sort
	for yi, y in enumerate(yList):
		oi = oy.index(y)
		xList[yi] = ox[oi]
	return xList, yList

def draw(foldername, filename, yLabel):
	xList = [] # x value (function name) list of graph
	yList = [] # y value list of graph
	xListOld = []
	yListOld = []
	yListPerRec = [] # y value per record
	recordNum = 0
	global progress
	print(f'Drawing graph... {math.floor(progress*100/7)}%', end="\r")
	progress = progress + 1
# read file 
	fx = open("../record/" + foldername + "/" + filename, "r")
	while True:
		line = fx.readline()
		t = line.split(" ")
		if t[0] == "#" or not line:
			if recordNum > 0:
# get y or one record
				for xi, x in enumerate(xList):
					if x in xListOld:
						oldXIdx = xListOld.index(x)
						yListPerRec.append(yList[xi] - yListOld[oldXIdx])
					else:
						yListPerRec.append(yList[xi])
				xListOld = xList
				yListOld = yList
# sort
				xList, yList = sort(xList, yList)
				xListPerRec, yListPerRec = sort(xListOld, yListPerRec)
# get ratio of y
				sumY = np.sum(yList)
				sumYPerRec = np.sum(yListPerRec)
				yRatioList = [i/sumY for i in yList]
				yRatioListPerRec = [i/sumYPerRec for i in yListPerRec]
# graph
				bar(xList, yList, yLabel, yLabel + " of Functions", "../record/" + foldername + "/" + str(recordNum)+"_"+filename + ".png")
				bar(xList, yListPerRec, yLabel, yLabel + " of Functions", "../record/" + foldername + "/" + str(recordNum)+"_"+filename + "_perRecord.png")
				bar(xList, yRatioList, yLabel, yLabel + " Ratio of Functions", "../record/" + foldername + "/" + str(recordNum)+"_"+filename + "_ratio.png")
				bar(xList, yRatioListPerRec, yLabel, yLabel + " Ratio of Functions", "../record/" + foldername + "/" + str(recordNum)+"_"+filename + "_ratio_perRecord.png")

				xList = []
				yList = []
				yListPerRec = []
		if not line:
			break
		if t[0] == '#':
			recordNum+=1
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
	print(f"Drawing graph... 100%\n")
	print("Complete\n")

if __name__ == "__main__":
	if len(sys.argv) == 2:
		print(f"Drawing graph... 0%", end="\r");
		folder_name = sys.argv[1]
		draw_all(folder_name)
	else:
		print("You need to enter a folder name for the only parmeter\n")
